#pragma once

#define LOG 0

#define SEND_CAN_STORAGE_SIZE 20
#define BUS_COUNT 2
#define VEHICLE_BUS 0
#define CHASSIS_BUS 1
#define CAN_FRAME_LEN 8

#define VEHICLE_BUS_ID_COUNT            12

#define ID_118_DRIVE_SYSTEM_STATUS      0x118
#define ID_129_STEERING_ANGLE           0x129
#define ID_229_GEAR_LEVER               0x229
#define ID_249_LEFT_STALK               0x249
#define ID_257_DL_SPEED                 0x257
#define ID_293_UI_CHASSIS_CONTROL       0x293
#define ID_334_UI_POWERTRAIN_CONTROL    0x334
#define ID_353_UI_STATUS                0x353
#define ID_39D_IBST_STATUS              0x39D
#define ID_3C2_VCLEFT_SWITCH_STATUS     0x3C2
#define ID_3E9_DAS_BODY_CONTROLS        0x3E9
#define ID_3F5_VCFRONT_LIGHTNING        0x3F5

#define CHASSIS_BUS_ID_COUNT            3

#define ID_239_DAS_LANES                0x239
#define ID_2B9_DAS_CONTROL              0x2B9
#define ID_399_DAS_STATUS               0x399

const uint16_t IGNORE_PIDS[48] = {
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
    double tstmp;
    uint8_t frame_len;
    uint16_t frame_id;
    uint8_t bus;
    uint8_t frame_data[CAN_FRAME_LEN];
} sendCan_t;

class CarState;

typedef void (CarState::*CANFrameHandler) (sendCan_t * frame, void * result);

typedef struct frameMAP_s {
    uint16_t frame_id;
    CANFrameHandler frame_handler;
} frameMAP_t;
