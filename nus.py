import platform
import logging
import asyncio
from bleak import BleakClient
from bleak import BleakClient
from bleak import _logger as logger
from bleak.uuids import uuid16_dict
from sys import argv
from os import popen
import time 


UART_TX_UUID = "6e400002-b5a3-f393-e0a9-e50e24dcca9e" #Nordic NUS characteristic for TX
UART_RX_UUID = "6e400003-b5a3-f393-e0a9-e50e24dcca9e" #Nordic NUS characteristic for RX

global dataFlag
dataFlag = False #global flag to check for new data
global data
data_input = ""


def notification_handler(sender, data):
    """Simple notification handler which prints the data received."""
    curtime = time.strftime('%H%M%S')
    print("{0}: {1}".format(curtime, data))
    global data_input
    data_input = data
    global dataFlag
    dataFlag = True


async def run(address, loop):

    async with BleakClient(address, loop=loop) as client:

        #wait for BLE client to be connected
        x = await client.is_connected()
        print("Connected: {0}".format(x))

        #wait for data to be sent from client
        await client.start_notify(UART_RX_UUID, notification_handler)


        while True : 

            #give some time to do other tasks
            await asyncio.sleep(0.01)
            #check if we received data
            global dataFlag
            if dataFlag :
                dataFlag = False
                if data_input == bytearray(b'1\x00'): # time sync
                    print("Timesync")
                    curtime = time.strftime('%H%M%S')
                    await client.write_gatt_char(UART_TX_UUID,curtime.encode())
                elif data_input == bytearray(b'2\x00'): # calendar
                    print("Calendar")
                    curtime = time.strftime('%H%M')
                    with open('events.csv') as f:
                        while True:
                            try:
                                event = f.readline().split('\n')[0]
                                if event.split(",")[0] > curtime:
                                    await client.write_gatt_char(UART_TX_UUID,event.encode())
                                    print(f"Event Sent: {event}")
                                    break
                            except KeyboardInterrupt:
                                break
                            except:
                                pass
                elif data_input == bytearray(b'3\x00'): # help
                    print("SOS")
                    popen('telegram-send "Help! I fell down!"')
                else:
                    print(f"NR: {(data_input)}")
                


if __name__ == "__main__":
    try:
        address = (argv[1])
    except:
        blelist=[i for i in popen('ble-scan').read().splitlines()[2:-2] if i[-1]!=' ']
        for i in range(len(blelist)):
            print(f"{i+1}: {blelist[i]}")
        while True:
            try:
                address = (blelist[int(input("Enter choice here: "))-1].split()[0])
                break
            except KeyboardInterrupt:
            	exit()
            except:
                pass
    
    while True:
        try:
            loop = asyncio.get_event_loop()
            loop.run_until_complete(run(address, loop))
        except KeyboardInterrupt:
            break
        except:
            pass
