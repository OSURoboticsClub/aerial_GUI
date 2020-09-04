#!/usr/bin/env python3
"""
* Program: hydra_server.py
* Author: Tomas McMonigal
* Date: 2/15/2020
* Description: Multi-threaded server that accepts incoming connections to send
    telemetry data to GUI, CP, and PP programs. Receives telemetry data from
    ttyUSB0. To run it provide the port number as a command line parameter

"""
import asyncio
from mavsdk import System
from concurrent.futures import TimeoutError
from mavsdk import (Attitude, OffboardError)


async def echo_server(reader, writer, drone):
    while True:
            print("waiting on data requests")
            marker = 1

            try:
                data = await asyncio.wait_for(reader.read(100), timeout=10)
                if not data: # client has disconnected
                    print("connection closed by client: closing connection with client")
                    break
            except TimeoutError:
                print("Timeout error: closing connection with client")
                break

            # ******** if data is valid and connection active ****************
            data_in = data.decode() # decodes utf-8 only
            data_in = data_in.replace('\r\n', '')

            if data_in == "latitude":
                print("latitude requested")
                async for position in drone.telemetry.position():
                    data_out = str(position.latitude_deg) + '\n'
                    print(data_out)
                    # data_out = data_out + '\n'
                    writer.write(data_out.encode())
                    await writer.drain()
                    break

            elif data_in == "altitude":
                async for position in drone.telemetry.position():
                    print("altitude requested")
                    data_out = str(position.relative_altitude_m) + '\n'
                    writer.write(data_out.encode())
                    await writer.drain()
                    break

            elif data_in == "longitude":
                async for position in drone.telemetry.position():
                    print("longitude requested")
                    data_out = str(position.longitude_deg) + '\n'
                    print(data_out)
                    # data_out = data_out + '\n'
                    writer.write(data_out.encode())
                    await writer.drain()
                    break

            elif data_in == "battery":
                async for battery in drone.telemetry.battery():
                    print("battery percentage requested")
                    data_out = str(battery.voltage_v) + '\n'
                    writer.write(data_out.encode())
                    await writer.drain()
                    break

            elif data_in == "arm":
                print("-- Arming")
                await drone.action.arm()

            elif data_in == "disarm":
                try:
                    print("-- Disarming")
                    await drone.action.disarm()
                except:
                    print("error disarming")

            elif data_in == "kill":
                print("-- Killing")
                await drone.action.kill()

            elif data_in == "is_armed":
                async for is_armed in drone.telemetry.armed():
                    print("Is_armed requested:", is_armed)
                    data_out = str(is_armed) + '\n'
                    writer.write(data_out.encode())
                    await writer.drain()
                    break

            elif data_in == "flight_mode":
                async for flight_mode in drone.telemetry.flight_mode():
                    print("flight mode requested:", flight_mode)
                    data_out = str(flight_mode) + '\n'
                    writer.write(data_out.encode())
                    await writer.drain()
                    break

            elif data_in == "takeoff":
                print("-- Taking off")
                await drone.action.takeoff()

            elif data_in == "land":
                print("-- Landing")
                await drone.action.land()

            elif data_in == "return":
                print("-- Returning home")
                await drone.action.return_to_launch()

            else:
                print("error: data requested not supported, closing connection")
                output_string = '|' + data_in + '|'
                print(output_string)
    writer.close()

async def main(host, port):
    drone = System()
    await drone.connect(system_address="udp://:14555")
    # await drone.connect(system_address="/dev/ttyUSB0")
    print("Waiting for drone to connect...")
    async for state in drone.core.connection_state():
        if state.is_connected:
            print(f"Drone discovered with UUID: {state.uuid}")
            break

    print("Waiting for drone to have a global position estimate...")
    async for health in drone.telemetry.health():
        if health.is_global_position_ok:
            print("Global position estimate ok")
            break
    print("-- Setting initial setpoint")
    await drone.offboard.set_attitude(Attitude(0.0, 0.0, 0.0, 0.0))

    print("-- Setting takeoff altitude to 5m")
    # await drone.action.set_takeoff_altitude(
    print (await drone.action.get_takeoff_altitude())

    print("Ready to accept incoming connections")
    server = await asyncio.start_server(lambda r, w: echo_server(r, w, drone), host, port)
    async with server:
        await server.serve_forever()

asyncio.run(main('127.0.0.1', 5000))
