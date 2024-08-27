# This script adds the following features for Tesla Model 3 (confirmed for '23+ RWD and Performance), but it might work for all Model 3/Y and possibly some S/X:
# * Auto-reengagement of autosteer after lane change or steering override as long as speed control is still active (i.e. no brake pedal press)
#   NOTE: This gives a "single-click" autosteer engagement, but the option for single/double click engagement MUST be set to double!
# * Temporary throttle override from Chill to Standard/Sport when throttle is pressed more than 78% (NOTE: The "CHILL" message on the screen will not change)
# * Persistent throttle override from Chill to Standard/Sport when the right steering wheel scroll is swiped up and cruise control is not active (swipe down to end throttle override)
# * Dynamic Regen Braking Percentage in Chill mode, which makes driving at highway speeds more "Chill"
# * Satisfies the "Apply steering torque" requirement while autosteer is enabled.  The driver camera still monitors driver attentiveness, which is important!
# * For Dual-CAN mode: After first AP engagement and until AP is cancelled via right stalk (up-click), AP will auto-engage anytime AP is "Ready"
#
# This could run on any device that has a PICAN interface with the vehicle, but has only been tested using the Raspberry-Pilot tech stack (RPi + PICAN2)
# The CAN connect used for development and testing was through these harnesses: 
#       Dual-CAN operation (full features):      https://evoffer.com/product/model-3-y-can-diagnostic-cable/
#       Single-CAN operation (limited features): https://www.gpstrackingamerica.com/shop/hrn-ct20t11/
# 
# TO-DO:
# * Find these CAN packets:
#   * Whether the current road has extra speed restictions (i.e. surface street vs highway)
#   * What is the current cruise control max speed
#
import time
import setproctitle
import os
import can
from carstate import CarState
from threading import Event, Thread
import signal

setproctitle.setproctitle('TeslaCANTool')
CS = CarState()

filterCAN_0 = []
filterCAN_1 = []
for i, channel in enumerate(CS.Update[:-1]):
    if i == 0:
        for pid in channel:
            filterCAN_0.append({"can_id": pid, "can_mask": 0x7FF, "extended": False})
    elif i == 1:
        for pid in channel:
            filterCAN_1.append({"can_id": pid, "can_mask": 0x7FF, "extended": False})

os.system("sudo /sbin/ip link set can0 down")
os.system("sudo /sbin/ip link set can1 down")
time.sleep(0.1)
os.system("sudo /sbin/ip link set can0 up type can bitrate 500000")
os.system("sudo /sbin/ip link set can1 up type can bitrate 500000")
time.sleep(0.1)
bus_0 = can.interface.Bus(channel='can0', bustype='socketcan', receive_own_messages=False, can_filters=filterCAN_0)
bus_1 = can.interface.Bus(channel='can1', bustype='socketcan', receive_own_messages=False, can_filters=filterCAN_1)
time.sleep(0.1)

logging = True
sendCAN = None
research = True
exit = Event()

def signal_handler(sig, frame):
    print("\nExiting...")
    cleanup()
    raise SystemExit

def cleanup():
    print("\nClean Up...")
    exit.set()
    time.sleep(0.1)
    os.system("sudo /sbin/ip link set can0 down")
    os.system("sudo /sbin/ip link set can1 down")


def CAN_Handler(bus, research, logging):
    logData = []

    if logging:
        from Influx_Client import Influx_Client
        IC = Influx_Client('PICAN')

    while not exit.is_set():
        msg = bus.recv()

        bus_number = int(msg.channel[-1:])

        if msg.arbitration_id in CS.Update[bus_number]:
            sendCAN = CS.Update[bus_number][msg.arbitration_id](msg.timestamp, msg.arbitration_id, bus_number, bytearray(msg.data))

            if sendCAN:
                for pid, _, cData in sendCAN:
                    pass
                    #bus.send(can.Message(arbitration_id=pid, data=[d for d in cData], is_extended_id=False, dlc=len(cData)))
                    #print(f"TX : {can.Message(arbitration_id=pid, data=[d for d in cData], is_extended_id=False, dlc=len(cData))}")
                sendCAN = None

        if research:
            CS.Update[-1](msg.timestamp, msg.arbitration_id, bus_number, bytearray(msg.data))

        if logging and msg.arbitration_id not in CS.ignorePIDs:
            logData.append([msg.timestamp, msg.channel, msg.arbitration_id, int.from_bytes(msg.data, byteorder='little', signed=False)])

            if len(logData) > 250:
                IC.InsertData(CS, logData)
                logData = []

    bus.shutdown()


if __name__ =="__main__":
    signal.signal(signal.SIGINT, signal_handler)
    t1 = Thread(target=CAN_Handler, args=(bus_0, research, logging))
    t2 = Thread(target=CAN_Handler, args=(bus_1, research, logging))

    t1.start()
    t2.start()

    t1.join()
    t2.join()

    print("Done!")
