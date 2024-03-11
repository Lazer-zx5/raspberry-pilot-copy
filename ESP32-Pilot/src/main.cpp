#include <Arduino.h>
#include <esp32_can.h>
#include "carstate.hpp"
#include "Vector.h"

CarState CS;
sendCan_t sendCanStorage[SEND_CAN_STORAGE_SIZE];
Vector<sendCan_t> sendCAN;

void setup() {
	Serial.println(" CAN...............INIT");
	CAN0.setCANPins(GPIO_NUM_5, GPIO_NUM_4);
	CAN0.begin(500000); // 500Kbps
	CAN0.watchFor();
	Serial.println(" CAN............500Kbps");

	memset(sendCanStorage, 0, sizeof(sendCanStorage));
	sendCAN.setStorage(sendCanStorage, sizeof(sendCanStorage));
	sendCAN.clear();
}

void loop() {
	sendCan_t frame;
	CAN_FRAME can_message;

	if (CAN0.read(can_message)) {
		for (int i = 0; i < VEHICLE_BUS_ID_COUNT; ++i) {
			if (can_message.id == CS.Update[VEHICLE_BUS][i].frame_id) {
				frame.bus = VEHICLE_BUS;
				frame.frame_id = can_message.id;
				memcpy(frame.frame_data, can_message.data.uint8, CAN_FRAME_LEN);
				frame.tstmp = 0;
				(CS.*(CS.Update[VEHICLE_BUS][i].frame_handler))(&frame, &sendCAN);
				break;
			}
		}

		if (!sendCAN.empty()) {
			for (int i = 0; i < sendCAN.size(); ++i) {
				CAN_FRAME can_frame;
				can_frame.rtr = 0;
				can_frame.id = sendCAN[i].frame_id;
				memcpy(can_frame.data.uint8, sendCAN[i].frame_data, CAN_FRAME_LEN);
				can_frame.extended = false;
				can_frame.length = CAN_FRAME_LEN;
				//CAN0.sendFrame(can_frame);
			}
			sendCAN.clear();
		}
	}
}
