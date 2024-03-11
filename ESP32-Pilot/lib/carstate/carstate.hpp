#pragma once

#include <Arduino.h>
#include "config.hpp"
#include <Vector.h>

class CarState {
    private:
        sendCan_t sendCanStorage[SEND_CAN_STORAGE_SIZE];
        frameMAP_t VEHICLE_BUS_MAP[VEHICLE_BUS_ID_COUNT] = {
            /* 0x118 */ {280,   &CarState::DriveState},
            /* 0x129 */ {297,   &CarState::SteerAngle},
            /* 0x229 */ {553,   &CarState::RightStalk},
            /* 0x249 */ {585,   &CarState::LeftStalk},
            /* 0x257 */ {599,   &CarState::VehicleSpeed},
            /* 0x293 */ {659,   &CarState::Throttle},
            /* 0x334 */ {820,   &CarState::Motor},
            /* 0x353 */ {851,   &CarState::PrintBitsAndString},
            /* 0x39D */ {925,   &CarState::BrakePedal},
            /* 0x3C2 */ {962,   &CarState::RightScroll},
            /* 0x3E9 */ {1001,  &CarState::DriverAssistState},
            /* 0x3F5 */ {1013,  &CarState::TurnSignal}
        };

        frameMAP_t CHASSIS_BUS_MAP[CHASSIS_BUS_ID_COUNT] = {
            /* 0x239 */ {569, &CarState::VirtualLane},
            /* 0x2B9 */ {697, &CarState::DASSpeed},
            /* 0x399 */ {921, &CarState::AutoPilotState}
        };

    private:
        bool moreBalls;
        bool tempBalls;
        bool enabled;
        uint8_t autoSteer; // need to recheck based on dbc
        float nextClickTime; // maybe this should be changed to int or uint = 0.
        uint8_t speed;
        uint8_t leftStalkStatus;
        uint32_t steerAngle;
        uint8_t lastAPStatus;
        float lastAutoSteerTime; // maybe this should be changed to int or uint = 0.
        uint8_t accelPedal;
        bool parked; // True
        uint32_t motorPID;
        uint32_t throttlePID;
        uint8_t lastStalk;
        bool autopilotReady; // True
        uint8_t handsOnState;
        bool brakePressed;
        float avgLaneCenter; // 100.0
        bool blinkersOn;
        bool closeToCenter;

        Vector<sendCan_t> sendCAN;
        uint8_t motor[8]; // = [32] --> motor[0] = 0x4B
        uint8_t throttleMode[8]; // = [0,0,0,0,0,16] --> throttleMode[5] = 0x10
        uint8_t histClick[25]; // = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
        const uint8_t * rightStalkCRC; // = [75,93,98,76,78,210,246,67,170,249,131,70,32,62,52,73]
        const uint32_t * ignorePIDs; // = [1000,1005,1060,1107,1132,1284,1316,1321,1359,1364,1448,1508,1524,1541,1542,1547,1550,1588,1651,1697,1698,1723,
                           //2036,313,504,532,555,637,643,669,701,772,777,829,854,855,858,859,866,871,872,896,900,928,935,965,979,997]

    public:
        CarState();
        ~CarState();

        void SendCAN(float tstmp, void * result);
        void BiggerBalls(float tstmp, uint8_t bus);
        void Throttle(sendCan_t * frame, void * result);
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

        frameMAP_t *Update[BUS_COUNT];
};
