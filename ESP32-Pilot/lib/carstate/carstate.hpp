#pragma once

#include <Arduino.h>
#include "config.hpp"
#include <Vector.h>

enum Gears {
    INVALID,
    PARKED,
    REVERSE,
    NEUTRAL,
    DRIVE
};

class CarState {
    private:
        sendCan_t sendCanStorage[SEND_CAN_STORAGE_SIZE];
        uint8_t histClickStorage[25]; // = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
        frameMAP_t VEHICLE_BUS_MAP[VEHICLE_BUS_ID_COUNT] = {
            {ID_118_DRIVE_SYSTEM_STATUS,    &CarState::DriveState},
            {ID_129_STEERING_ANGLE,         &CarState::SteerAngle},
            {ID_229_GEAR_LEVER,             &CarState::RightStalk},
            {ID_249_LEFT_STALK,             &CarState::LeftStalk},
            {ID_257_DL_SPEED,               &CarState::VehicleSpeed},
            {ID_293_UI_CHASSIS_CONTROL,     &CarState::ChassisControl},
            {ID_334_UI_POWERTRAIN_CONTROL,  &CarState::Motor},
            {ID_353_UI_STATUS,              &CarState::PrintBitsAndString},
            {ID_39D_IBST_STATUS,            &CarState::BrakePedal},
            {ID_3C2_VCLEFT_SWITCH_STATUS,   &CarState::RightScroll},
            {ID_3E9_DAS_BODY_CONTROLS,      &CarState::DriverAssistState},
            {ID_3F5_VCFRONT_LIGHTNING,      &CarState::TurnSignal}
        };

        frameMAP_t CHASSIS_BUS_MAP[CHASSIS_BUS_ID_COUNT] = {
            {ID_239_DAS_LANES,              &CarState::VirtualLane},
            {ID_2B9_DAS_CONTROL,            &CarState::DASSpeed},
            {ID_399_DAS_STATUS,             &CarState::AutoPilotState}
        };

    private:
        bool moreBalls;
        bool tempBalls;
        bool APenabled;
        bool TACCenabled;
        uint8_t autoSteer; // need to recheck based on dbc
        double nextClickTime; // maybe this should be changed to int or uint = 0.
        uint8_t speed;
        uint8_t leftStalkStatus;
        int steerAngle;
        uint8_t lastAPStatus;
        double lastAutoSteerTime; // maybe this should be changed to int or uint = 0.
        uint8_t accelPedal;
        Gears gear;
        bool parked; // True
        uint16_t motorPID;
        uint16_t chassisControlPID;
        uint8_t lastStalk;
        bool autopilotReady; // True
        uint8_t handsOnState;
        bool brakePressed;
        float avgLaneCenter; // 100.0
        bool blinkersOn;
        bool hazardButtonPressed;
        bool closeToCenter;

        int BLE_condition;

        Vector<sendCan_t> sendCAN;
        uint8_t motor[8]; // = [32] --> motor[0] = 0x4B
        uint8_t chassisControl[8]; // = [0,0,0,0,0,16] --> throttleMode[5] = 0x10
        Vector<uint8_t> histClick;
        const uint8_t * rightStalkCRC; // = [75,93,98,76,78,210,246,67,170,249,131,70,32,62,52,73]
        const uint16_t * ignorePIDs; // = [1000,1005,1060,1107,1132,1284,1316,1321,1359,1364,1448,1508,1524,1541,1542,1547,1550,1588,1651,1697,1698,1723,
                           //2036,313,504,532,555,637,643,669,701,772,777,829,854,855,858,859,866,871,872,896,900,928,935,965,979,997]
        
    private:
        uint8_t get_CRC(uint8_t* frame_data, uint8_t frame_counter, uint8_t frame_data_len, uint16_t frame_id);

    public:
        CarState();
        ~CarState();

        void SendCAN(float tstmp, void * result);
        void BiggerBalls(float tstmp, uint8_t bus);
        void ChassisControl(sendCan_t * frame, void * result);
        void Motor(sendCan_t * frame, void * result);
        void DriveState(sendCan_t * frame, void * result);
        void RightScroll(sendCan_t * frame, void * result);
        void VehicleSpeed(sendCan_t * frame, void * result);
        void LeftStalk(sendCan_t * frame, void * result);
        void TurnSignal(sendCan_t * frame, void * result);
        void SteerAngle(sendCan_t * frame, void * result);
        void BrakePedal(sendCan_t * frame, void * result);
        void VirtualLane(sendCan_t * frame, void * result);
        void DriverAssistState(sendCan_t * frame, void * result);
        bool EnoughClicksAlready();  // Prevent unintended triggering of the "Rainbow Road" Easter Egg
        void RightStalk(sendCan_t * frame, void * result);
        void AutoPilotState(sendCan_t * frame, void * result);
        void DASSpeed(sendCan_t * frame, void * result);
        void PrintBits(sendCan_t * frame, void * result);
        void PrintBytes(sendCan_t * frame, void * result);
        void PrintBitsAndString(sendCan_t * frame, void * result);
        bool Parked();
        int connect(BLEClient* pClient);
        void gate_open();

        frameMAP_t *Update[BUS_COUNT];
};
