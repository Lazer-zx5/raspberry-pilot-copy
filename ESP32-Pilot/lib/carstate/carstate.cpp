#include "carstate.hpp"

CarState::CarState() {
    moreBalls = false;
    tempBalls = false;
    enabled = false;
    autoSteer = 0;
    nextClickTime = 0.0;
    speed = 0;
    leftStalkStatus = 0;
    steerAngle = 0;
    lastAPStatus = 0;
    lastAutoSteerTime = 0.0; // maybe this should be changed to int or uint = 0.
    accelPedal = 0;
    parked = true; // True
    motorPID = 0;
    throttlePID = 0;
    lastStalk = 0;
    autopilotReady = true; // True
    handsOnState = 0;
    brakePressed = 0;
    avgLaneCenter = 100.0; // 100.0
    blinkersOn = false;
    closeToCenter = false;
    
    memset(sendCanStorage, 0, sizeof(sendCanStorage));
    sendCAN.setStorage(sendCanStorage, sizeof(sendCanStorage));
    sendCAN.clear();
    memset(motor, 0, sizeof(motor));
    memset(throttleMode, 0, sizeof(throttleMode));
    memset(histClick, 0, sizeof(histClick));

    motor[0] = 0x20;
    throttleMode[5] = 0x10;
    rightStalkCRC = RIGHT_STALK_CRC;
    ignorePIDs = IGNORE_PIDS;

    Update[VEHICLE_BUS] = VEHICLE_BUS_MAP;
    Update[CHASSIS_BUS] = CHASSIS_BUS_MAP;
}

uint16_t CarState::get_CRC(uint8_t * frame_data, uint8_t frame_counter, uint8_t frame_data_len, uint16_t frame_id) {
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

    return result;
}

void CarState::SendCAN(float tstmp, void * result) {
    return;
}

void CarState::BiggerBalls(float tstmp, uint8_t bus) {
    if ((this->moreBalls || this->tempBalls) && (this->motor[0] & 0x20 == 0) && (this->throttleMode[5] & 0x10 == 0)) {
        // override throttle mode to Standard / Sport
        this->motor[0] = (this->motor[0] + 0x20) & 0xFF; // UI_pedalMap
        this->motor[6] = (this->motor[6] + 0x10) & 0xFF; // counter value
        this->motor[7] = (this->motor[7] + 0x30) & 0xFF; // CRC value

    }
    return;
}

void CarState::Throttle(sendCan_t * frame, void * result) {
    return;
}

void CarState::Motor(sendCan_t * frame, void * result) {
    return;
}

void CarState::DriveState(sendCan_t * frame, void * result) {
    return;
}

void CarState::RightScroll(sendCan_t * frame, void * result) {
    return;
}

void CarState::VehicleSpeed(sendCan_t * frame, void * result) {
    return;
}

void CarState::LeftStalk(sendCan_t * frame, void * result) {
    return;
}

void CarState::TurnSignal(sendCan_t * frame, void * result) {
    return;
}

void CarState::SteerAngle(sendCan_t * frame, void * result) {
    return;
}

void CarState::BrakePedal(sendCan_t * frame, void * result) {
    return;
}

void CarState::VirtualLane(sendCan_t * frame, void * result) {
    return;
}

void CarState::DriverAssistState(sendCan_t * frame, void * result) {
    return;
}

bool CarState::EnoughClicksAlready() {
    return false;
}

void CarState::RightStalk(sendCan_t * frame, void * result) {
    return;
}

void CarState::AutoPilotState(sendCan_t * frame, void * result) {
    return;
}

void CarState::DASSpeed(sendCan_t * frame, void * result) {
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
