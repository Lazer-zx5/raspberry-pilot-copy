#include <Arduino.h>
#include <esp32_can.h>
#include "carstate.hpp"
#include "Vector.h"

CarState CS;
sendCan_t sendCanStorage[SEND_CAN_STORAGE_SIZE];
Vector<sendCan_t> sendCAN;
double loopStart = 0;

#if LOG
void printFrame(CAN_FRAME* message) {
	Serial.printf("ID = 0x%02X\tLEN = %d\tDATA = ", message->id, message->length);
	for (int i = 0; i < message->length; i++) {
		Serial.print(message->data.byte[i], HEX);
		Serial.print(" ");
	}
	Serial.println();
}
#endif

void processFrame(CAN_FRAME* message) {
	sendCan_t frame;
	CAN_FRAME can_message = *message;

	for (int i = 0; i < VEHICLE_BUS_ID_COUNT; ++i) {
		if (can_message.id == CS.Update[VEHICLE_BUS][i].frame_id) {
			frame.bus = VEHICLE_BUS;
			frame.frame_id = can_message.id;
			frame.frame_len = can_message.length;
			memcpy(frame.frame_data, can_message.data.uint8, CAN_FRAME_LEN);
			frame.tstmp = (double)(loopStart / 1000);
			(CS.*(CS.Update[VEHICLE_BUS][i].frame_handler))(&frame, &sendCAN);
			break;
		}
	}

}

void setup() {
	memset(sendCanStorage, 0, sizeof(sendCanStorage));
	sendCAN.setStorage(sendCanStorage, sizeof(sendCanStorage));
	sendCAN.clear();

#if LOG
	Serial.begin(1000000);
	Serial.println(" CAN...............INIT");
#endif

	CAN0.setCANPins(GPIO_NUM_5, GPIO_NUM_4);
	CAN0.begin(500000); // 500Kbps

	uint16_t mask = 0x07FF;

	CAN0.setRXFilter(1, ID_118_DRIVE_SYSTEM_STATUS, mask, false);
	CAN0.setRXFilter(2, ID_129_STEERING_ANGLE, mask, false);
	CAN0.setRXFilter(3, ID_229_GEAR_LEVER, mask, false);
	CAN0.setRXFilter(4, ID_249_LEFT_STALK, mask, false);
	CAN0.setRXFilter(5, ID_257_DL_SPEED, mask, false);
	CAN0.setRXFilter(6, ID_293_UI_CHASSIS_CONTROL, mask, false);
	CAN0.setRXFilter(7, ID_334_UI_POWERTRAIN_CONTROL, mask, false);
	CAN0.setRXFilter(8, ID_353_UI_STATUS, mask, false);
	CAN0.setRXFilter(9, ID_39D_IBST_STATUS, mask, false);
	CAN0.setRXFilter(10, ID_3C2_VCLEFT_SWITCH_STATUS, mask, false);
	CAN0.setRXFilter(11, ID_3E9_DAS_BODY_CONTROLS, mask, false);
	CAN0.setRXFilter(12, ID_3F5_VCFRONT_LIGHTNING, mask, false);
	/*
	CAN1 CHASSIS BUS
	CAN1.setRXFilter(13,	ID_239_DAS_LANES,            	mask, false);
	CAN1.setRXFilter(14,	ID_2B9_DAS_CONTROL,          	mask, false);
	CAN1.setRXFilter(15,	ID_399_DAS_STATUS,           	mask, false);
	*/

	for (int i = 1; i < 13; i++) {
		CAN0.setCallback(i, processFrame); //callback on that first special filter
	}

#ifdef LOG
	Serial.println(" CAN............500Kbps");
#endif
}

void loop() {
	loopStart = millis();

	if (!sendCAN.empty()) {
		for (int i = 0; i < sendCAN.size(); ++i) {
			CAN_FRAME can_frame;
			can_frame.rtr = 0;
			can_frame.id = sendCAN[i].frame_id;
			memcpy(can_frame.data.uint8, sendCAN[i].frame_data, sendCAN[i].frame_len);
			can_frame.extended = false;
			can_frame.length = sendCAN[i].frame_len;
			CAN0.sendFrame(can_frame);
#if LOG
			printFrame(&can_frame);
#endif
		}
		sendCAN.clear();
	}
}
