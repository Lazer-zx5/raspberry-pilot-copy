#include "carstate.hpp"

CarState::CarState() {
    moreBalls = false;
    tempBalls = false;
    APenabled = false;
    TACCenabled = false;
    autoSteer = 0;
    nextClickTime = 0.0;
    speed = 0;
    leftStalkStatus = 0;
    steerAngle = 0;
    lastAPStatus = 0;
    lastAutoSteerTime = 0.0; // maybe this should be changed to int or uint = 0.
    accelPedal = 0;
    parked = true; // True
    gear = PARKED;
    motorPID = 0;
    lastStalk = 0;
    autopilotReady = true; // True
    handsOnState = 0;
    brakePressed = false;
    avgLaneCenter = 100.0; // 100.0
    blinkersOn = false;
    closeToCenter = false;
    hazardButtonPressed = false;
    BLE_condition = READY;
    
    memset(sendCanStorage, 0, sizeof(sendCanStorage));
    sendCAN.setStorage(sendCanStorage, sizeof(sendCanStorage) / sizeof(sendCanStorage[0]));
    sendCAN.clear();
    memset(motor, 0, sizeof(motor));
    memset(chassisControl, 0, sizeof(chassisControl));
    memset(histClickStorage, 0, sizeof(histClickStorage));
    histClick.setStorage(histClickStorage, sizeof(histClickStorage) / sizeof(histClickStorage[0]));
    histClick.clear();
    for (int i = 0; i < histClick.max_size() - 1; i++) {
        histClick.push_back(0);
    }

    motor[0] = 0x20;
    rightStalkCRC = RIGHT_STALK_CRC;
    ignorePIDs = IGNORE_PIDS;

    Update[VEHICLE_BUS] = VEHICLE_BUS_MAP;
    Update[CHASSIS_BUS] = CHASSIS_BUS_MAP;
}

bool CarState::Parked() {
    return this->parked;
}

uint8_t CarState::get_CRC(uint8_t * frame_data, uint8_t frame_counter, uint8_t frame_data_len, uint16_t frame_id) {
    if (frame_data == NULL) {
        return 0;
    }

    uint16_t result = 0;

    for (int i = 0; i < frame_data_len; ++i) {
        result += frame_data[i];
    }

    result += frame_counter;
    result += frame_id >> 8;
    result += frame_id & 0xFF;
    result %= 0x100;

    return result & 0xFF;
}

void CarState::SendCAN(float tstmp, void * result) {
    if (this->sendCAN.size() == 0) {
        return;
    }

    Vector<sendCan_t> *data = (Vector<sendCan_t> *)result;
    for (int i = this->sendCAN.size() - 1; i >= 0; --i) {
        if (this->sendCAN[i].tstmp <= tstmp) {
            data->push_back(this->sendCAN[i]);
            this->sendCAN.remove(i);
        }
    }

    return;
}

void CarState::BiggerBalls(float tstmp, uint8_t bus) {
    if ((this->moreBalls || this->tempBalls) && ((this->motor[0] & 0x20) == 0)) {
        // override throttle mode to Standard / Sport
        this->motor[0] = (this->motor[0] + 0x20) & 0xFF; // UI_pedalMap
        this->motor[6] = (this->motor[6] + 0x10) & 0xFF; // counter value
        this->motor[7] = (this->motor[7] + 0x30) & 0xFF; // CRC value
        sendCan_t temp = {.tstmp=tstmp, .frame_len=CAN_FRAME_LEN, .frame_id=this->motorPID, .bus=bus, .frame_data={0}};
        memcpy(temp.frame_data, this->motor, sizeof(this->motor));
        this->sendCAN.push_back(temp);
        this->motor[0] = 0x20;
    }

    return;
}

void CarState::ChassisControl(sendCan_t * frame, void * result) {
#if 0 // currently not working
    chassisControlPID = frame->frame_id;
    memcpy(this->chassisControl, frame->frame_data, sizeof(this->chassisControl));
    this->chassisControl[3] |= BIT0; // autoLaneChangeEnable
    this->chassisControl[5] |= BIT6; // redLightStopSignEnable
    this->chassisControl[6] = (this->chassisControl[6] + 0x10) & 0xFF; // counter value
    this->chassisControl[7] = this->get_CRC(this->chassisControl, this->chassisControl[6], 6, this->chassisControlPID);

    sendCan_t temp = { .tstmp = frame->tstmp, .frame_len = frame->frame_len, .frame_id = this->chassisControlPID, .bus = frame->bus, .frame_data = {0} };
    memcpy(temp.frame_data, this->chassisControl, sizeof(this->chassisControl));
    this->sendCAN.push_back(temp);

    SendCAN(frame->tstmp, result);
#endif
    return;
}

void CarState::Motor(sendCan_t * frame, void * result) {
    this->motorPID = frame->frame_id;
    memcpy(this->motor, frame->frame_data, sizeof(this->motor));
    BiggerBalls(frame->tstmp, frame->bus);
    SendCAN(frame->tstmp, result);
    return;
}

void CarState::DriveState(sendCan_t * frame, void * result) {
    // 0x32 -> P 001 1 0010	
    // 0x55 -> R 010 1 0101
    // 0x72 -> N 011 1 0010
    // 0x95 -> D 100 1 0101
    this->gear = (Gears)(frame->frame_data[2] >> 5);
    this->parked = (this->gear == PARKED);
    this->accelPedal = frame->frame_data[4];
    this->tempBalls = this->accelPedal > 200; // override to standard / sport if throttle is above 78%
    BiggerBalls(frame->tstmp, frame->bus);
    SendCAN(frame->tstmp, result);
    return;
}

void CarState::RightScroll(sendCan_t * frame, void * result) {
    if (frame->frame_data[3] != 0x00 && frame->frame_data[3] != 0x55) {
        if (frame->frame_data[3] <= 0x40 && frame->frame_data[3] >= 0x2C && (this->TACCenabled || this->APenabled)) {
            this->nextClickTime = max(this->nextClickTime, frame->tstmp + 10);
        } else if (frame->frame_data[3] < 0x14 && frame->frame_data[3] > 0x01 && !this->APenabled) {
            this->moreBalls = true;
        } else if (frame->frame_data[3] < 0x40 && frame->frame_data[3] > 0x2C && !this->APenabled) {
            this->moreBalls = false;
        }
    }
    BiggerBalls(frame->tstmp, frame->bus);
    SendCAN(frame->tstmp, result);
    return;
}

void CarState::VehicleSpeed(sendCan_t * frame, void * result) {
    this->speed = frame->frame_data[3];
    SendCAN(frame->tstmp, result);
    return;
}

void CarState::LeftStalk(sendCan_t * frame, void * result) {
    if (frame->frame_data[2] & 0x0F > this->leftStalkStatus) {
        this->blinkersOn = true;
        this->closeToCenter = false;
        this->leftStalkStatus = frame->frame_data[2] & 0x0F; // Get left stalk status, and bump the status up for full click vs half click turn signal
        this->nextClickTime = max(this->nextClickTime, frame->tstmp + 1);
    }
    SendCAN(frame->tstmp, result);
    return;
}

void CarState::TurnSignal(sendCan_t * frame, void * result) {
    if ((frame->frame_data[5] & 0x03) == 0x01 || (frame->frame_data[5] & 0x03) == 0x02) {
        this->closeToCenter = false;
        double delay = 0.0;
        if (this->leftStalkStatus == 0x04 || this->leftStalkStatus == 0x08) {
            delay = 0.5;
        } else {
            delay = 1.0;
        }
        this->nextClickTime = max(this->nextClickTime, frame->tstmp + delay);
    }
    this->blinkersOn = ((frame->frame_data[5] & 0x03) == 0x01 || (frame->frame_data[5] & 0x03) == 0x02) ? true : false;
    this->hazardButtonPressed = ((frame->frame_data[0] >> 4) == 1) ? true : false; 
    if (this->hazardButtonPressed && !this->parked) {
        this->gate_open();
    }

    SendCAN(frame->tstmp, result);
    return;
}

void CarState::SteerAngle(sendCan_t * frame, void * result) {
    uint8_t data_bytes[2] = {0};
    data_bytes[0] = frame->frame_data[2];
    data_bytes[1] = frame->frame_data[3] & 0x3F;
    this->steerAngle = (data_bytes[1] << 8 | data_bytes[0]) - 8192;
    SendCAN(frame->tstmp, result);
    return;
}

void CarState::BrakePedal(sendCan_t * frame, void * result) {
    this->brakePressed = (frame->frame_data[2] & 0x02) != 0;
    SendCAN(frame->tstmp, result);
    return;
}

void CarState::VirtualLane(sendCan_t * frame, void * result) {
    // skip for now since there is no phisical connection to chassis bus
    return;
}

void CarState::DriverAssistState(sendCan_t * frame, void * result) {
    this->lastAPStatus = frame->frame_data[3] & 0x21;
    // lastAPStatus = 0x21 -> AP ACTIVE
    // lastAPStatus = 0x20 -> TACC ACTIVE
    // lastAPStatus = 0x00 -> NOT ACTIVE
    if (this->lastAPStatus == 0x21) {
        this->lastAutoSteerTime = frame->tstmp;
        this->APenabled = true;
        this->TACCenabled = false;
    } else if (this->lastAPStatus == 0x20) {
        this->APenabled = false;
        this->TACCenabled = true;
    } else if (this->lastAPStatus == 0x00) {
        this->APenabled = false;
        this->TACCenabled = false;
        this->nextClickTime = max(this->nextClickTime, frame->tstmp + 0.5); // if the car isn't moving or AP isn't engaged, then delay the click
    }

    SendCAN(frame->tstmp, result);
    return;
}

bool CarState::EnoughClicksAlready() {
    // TODO: currently not used: need to investigate

    uint8_t sum = 0;
    for (int i = (this->histClick.max_size() - 1); i >= 0; i--) {
        sum += this->histClick[i];
        if (sum > 1) {
            return true;
        }
    }
    return false;
}

void CarState::RightStalk(sendCan_t * frame, void * result) {
    // TODO: works but with big random delay, needs to be optimized

    if (this->autopilotReady && 
        !this->brakePressed &&
        !this->blinkersOn &&
        this->accelPedal < 100 &&
        (frame->frame_data[1] <= 0x0F) &&
        ((frame->tstmp > this->nextClickTime) || this->closeToCenter) &&
        ((this->lastAPStatus == 0x21) || (abs(this->steerAngle) < 60))) {
        if (this->TACCenabled && !this->APenabled) {
            // Turn the AP on
            sendCan_t temp = { .tstmp = frame->tstmp, .frame_len = frame->frame_len, .frame_id = frame->frame_id, .bus = frame->bus, .frame_data = {0} };
            uint8_t counter = frame->frame_data[1] & 0x0F;
            temp.frame_data[0] = this->rightStalkCRC[counter];
            temp.frame_data[1] = (0x30 | ((counter + 1) % 16));
            temp.tstmp += 0.05;
            this->sendCAN.push_back(temp);
            this->histClick.push_back(1);
            this->nextClickTime = max(this->nextClickTime, frame->tstmp + 0.5);
            this->leftStalkStatus = 0;
            this->lastStalk = temp.frame_data[1];
        } else if (!this->TACCenabled && this->APenabled && !this->EnoughClicksAlready()) {
            // Keep emulating for hands on notification
            sendCan_t temp = { .tstmp = frame->tstmp, .frame_len = frame->frame_len, .frame_id = frame->frame_id, .bus = frame->bus, .frame_data = {0} };
            uint8_t counter = frame->frame_data[1] & 0x0F;
            temp.frame_data[0] = this->rightStalkCRC[counter];
            temp.frame_data[1] = (0x30 | ((counter + 1) % 16));
            temp.tstmp += 0.05;
            this->sendCAN.push_back(temp);
            this->histClick.push_back(1);
            this->nextClickTime = max(this->nextClickTime, frame->tstmp + 0.5);
            this->leftStalkStatus = 0;
            this->lastStalk = temp.frame_data[1];
        } else {
            if (((frame->frame_data[1] >> 4) == 3) && ((this->lastStalk >> 4) != 3)) { 
                // clicked but autopilot is not engaged ex. gear change
                this->histClick.push_back(1);
            } else {
                this->histClick.push_back(0);
            }
            this->lastStalk = frame->frame_data[1];
        }
    }
    histClick.remove(0);
    SendCAN(frame->tstmp, result);
    return;
}

void CarState::AutoPilotState(sendCan_t * frame, void * result) {
    // skip for now since there is no phisical connection to chassis bus
    return;
}

void CarState::DASSpeed(sendCan_t * frame, void * result) {
    // skip for now since there is no phisical connection to chassis bus
    return;
}

void CarState::PrintBits(sendCan_t * frame, void * result) {
    return;
}

void CarState::PrintBytes(sendCan_t * frame, void * result) {
    return;
}

void CarState::PrintBitsAndString(sendCan_t * frame, void * result) {
    return;
}

CarState::~CarState() {
    //data = 0;
}


int CarState::connect(BLEClient* pClient) {
    bool try_connect = true;
    int attempt_count = 0;

#if LOG
    Serial.printf("\nTrying to connect to the gate...\n");
#endif

    while (try_connect && attempt_count != RECON_COUNT) {
        attempt_count += 1;
#if LOG
        Serial.printf("Attempt no %d/%d ...\n", attempt_count, RECON_COUNT);
#endif
        if (pClient->connect(BLEAddress(GATE_ADDRESS_1))) {
            try_connect = false;
            return 0;
        }
    }

    return -1;
}

void CarState::gate_open() {
    if (BLE_condition == READY) {
        BLE_condition = BUSY;

        BLEClient* pClient = BLEDevice::createClient();
        if (this->connect(pClient) != 0) {
#if LOG
            Serial.println("Connection Failed");
#endif
            BLE_condition = READY;
            return;
        } else {
#if LOG
            Serial.println("Connected!");
#endif
        }

        BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
        if (pRemoteService == nullptr) {
#if LOG
            Serial.println("Failed to get service");
#endif
            BLE_condition = READY;
            return;
        } else {
#if LOG
            Serial.println("Got service");
#endif
        }

        BLERemoteCharacteristic* pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
        if (pRemoteCharacteristic == nullptr) {
#if LOG
            Serial.println("Failed to get characteristic");
#endif
            BLE_condition = READY;
            return;
        } else {
#if LOG
            Serial.println("Got characteristic");
#endif
        }

        char value[15] = { '3', '5', '0', '9', '3', '6', '9', '6', '0', '2', '3', '1', '7', '5', '2' };

#if LOG
        Serial.println("Opening the gate!");
#endif
        pRemoteCharacteristic->writeValue(value, sizeof(value));
        BLE_condition = READY;
    }
}
