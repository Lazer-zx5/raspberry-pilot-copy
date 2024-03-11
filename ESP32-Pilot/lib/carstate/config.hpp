#pragma once

#define SEND_CAN_STORAGE_SIZE 20
#define BUS_COUNT 2
#define VEHICLE_BUS 0
#define CHASSIS_BUS 1
#define CAN_FRAME_LEN 8

#define VEHICLE_BUS_ID_COUNT 12
#define CHASSIS_BUS_ID_COUNT 3

const uint32_t IGNORE_PIDS[48] = {
    1000, 1005, 1060, 1107,
    1132, 1284, 1316, 1321,
    1359, 1364, 1448, 1508,
    1524, 1541, 1542, 1547,
    1550, 1588, 1651, 1697,
    1698, 1723, 2036, 313,
    504, 532, 555, 637,
    643, 669, 701, 772,
    777, 829, 854, 855,
    858, 859, 866, 871,
    872, 896, 900, 928,
    935, 965, 979, 997
};

// 75, 93, 98, 76,
// 78, 210, 246, 67,
// 170, 249, 131, 70,
// 32, 62, 52, 73

const uint8_t RIGHT_STALK_CRC[16] = {
    0x4B, 0x5D, 0x62, 0x4C,
    0x4E, 0xD2, 0xF6, 0x43,
    0xAA, 0xF9, 0x83, 0x46,
    0x20, 0x3E, 0x34, 0x49
};

typedef struct sendCan_s {
    float tstmp;
    uint32_t frame_id;
    uint8_t bus;
    uint8_t frame_data[CAN_FRAME_LEN];
} sendCan_t;

class CarState;

typedef void (CarState::*CANFrameHandler) (sendCan_t * frame, void * result);

typedef struct frameMAP_s {
    uint32_t frame_id;
    CANFrameHandler frame_handler;
} frameMAP_t;
