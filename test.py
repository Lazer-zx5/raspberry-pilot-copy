import asyncio
import time
from bleak import BleakClient, BleakScanner

async def connect(device):
    async with BleakClient(device) as client:
        print(f"Connected to device {client.address}")

        # Perform operations on the connected device
        # For example, read characteristics or write to characteristics

async def scan_for_device(address):
    print("scanning")
    start_time = time.time()
    while time.time() - start_time < 10:
        devices = await BleakScanner.discover()
        for device in devices:
            if device.address == address:
                return device
        await asyncio.sleep(1)  # Adjust scan interval as needed
    return None


async def main():
    address = "00:21:7E:71:AA:FA"
    device = await scan_for_device(address)
    if device:
        await connect(device)
    else:
        print("Device not found within 10 seconds")

if __name__ == "__main__":
    asyncio.run(main())
