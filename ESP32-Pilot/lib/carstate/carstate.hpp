#pragma once

#include <Arduino.h>

typedef struct sendCan_s {
    float tstmp;
    uint32_t frame_id;
    uint8_t bus;
    uint8_t frame_data[8];
} sendCan_t;

typedef void (*CANFrameHandler) (sendCan_t * frame, sendCan_t * result);

typedef struct frameMAP_s {
    uint32_t frame_id;
    CANFrameHandler frame_handler;
} frameMAP_t;

class CarState {
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

        sendCan_t sendCan;
        uint8_t motor[8];// = [32] --> motor[0] = 0x4B
        uint8_t throttleMode[8];// = [0,0,0,0,0,16] --> throttleMode[5] = 0x10
        uint8_t histClick[25];// = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
        uint8_t rightStalkCRC[16];// = [75,93,98,76,78,210,246,67,170,249,131,70,32,62,52,73]
        uint32_t ignorePIDs[48];// = [1000,1005,1060,1107,1132,1284,1316,1321,1359,1364,1448,1508,1524,1541,1542,1547,1550,1588,1651,1697,1698,1723,
                           //2036,313,504,532,555,637,643,669,701,772,777,829,854,855,858,859,866,871,872,896,900,928,935,965,979,997]


    public:
        CarState();
        ~CarState();

        void SendCAN(float tstmp);
        void BiggerBalls(float tstmp, uint8_t bus);
        void Throttle(sendCan_t * frame, sendCan_t * result);
        void Motor(sendCan_t * frame, sendCan_t * result);
        void DriveState(sendCan_t * frame, sendCan_t * result);
        void RightScroll(sendCan_t * frame, sendCan_t * result);
        void VehicleSpeed(sendCan_t * frame, sendCan_t * result);
        void LeftStalk(sendCan_t * frame, sendCan_t * result);
        void TurnSignal(sendCan_t * frame, sendCan_t * result);
        void SteerAngle(sendCan_t * frame, sendCan_t * result);
        void BrakePedal(sendCan_t * frame, sendCan_t * result);
        void VirtualLane(sendCan_t * frame, sendCan_t * result);
        void DriverAssistState(sendCan_t * frame, sendCan_t * result);
        bool EnoughClicksAlready();  // Prevent unintended triggering of the "Rainbow Road" Easter Egg
        void RightStalk(sendCan_t * frame, sendCan_t * result);
        void AutoPilotState(sendCan_t * frame, sendCan_t * result);
        void DASSpeed(sendCan_t * frame, sendCan_t * result);
        void PrintBits(sendCan_t * frame, sendCan_t * result);
        void PrintBytes(sendCan_t * frame, sendCan_t * result);
        void PrintBitsAndString(sendCan_t * frame, sendCan_t * result);


        frameMAP_t *Update[2];

        

};
