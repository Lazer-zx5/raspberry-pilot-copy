import asyncio
import sys
import psutil

from bleak import BleakClient, BleakScanner

GATE_SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
GATE_CHAR_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

GATE_ADDRESS_1 = "00:21:7E:71:AA:FA"

async def gate_opener(address):
    print("Scanning")
    device = await BleakScanner.find_device_by_address(address)
    if device is None:
        print("Device not found.")
        print("Exiting.")
    else:
        print("Device found.")
        print("Connecting.")
        async with BleakClient(device) as client:
            print(f"Connected: {client.is_connected}")

            paired = await client.pair(protection_level=2)
            print(f"Paired: {paired}")

            val2 = [51, 53, 48, 57, 51, 54, 57, 54, 48, 50, 51, 49, 55, 53, 50]

            print("Opening the gate... val2")
            await client.write_gatt_char(GATE_CHAR_UUID, bytes(val2), response=False)
            await asyncio.sleep(1.0)


def main_opener():
    asyncio.run(gate_opener(GATE_ADDRESS_1))



if __name__ == "__main__":
    counter = 0
    for process in psutil.process_iter():
        if ['python', 'ble.py'] == process.cmdline():
            counter += 1

    if counter == 1:
        main_opener()
