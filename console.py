#!/usr/bin/python3

import serial
import sys
import select
import tty
import termios
import time

SOH = b'\x01'
EOT = b'\x04'
ACK = b'\x06'
NAK = b'\x15'
CAN = b'\x18'
SUB = b'\x1A'

def main():
    if len(sys.argv) < 2:
        print(f'Usage: {sys.argv[0]} /dev/ttyUSB0 [default xmodem filename]')
        return
    device = sys.argv[1]
    old_settings = termios.tcgetattr(0)
    with serial.Serial(device, baudrate=115200, timeout=1) as term:
        try:
            default_fn = ''
            default_str = ''
            if len(sys.argv) >= 3:
                default_fn = sys.argv[2]
                default_str = f"\nDefault xmodem filename: '{default_fn}'\nPress return to use it.\n"
            print(f"Connected to {device} - Exit via escape char '^]'")
            tty.setraw(sys.stdin.fileno())
            term.write(b"\n")
            while True:
                readable, _, _ = select.select([term.fileno(), sys.stdin.fileno()], [], [])
                for s in readable:
                    if s == term.fileno():
                        data = term.read(1)
                        if not data:
                            print("Connection closed.")
                            return
                        if data == NAK:
                            termios.tcsetattr(0, termios.TCSADRAIN, old_settings)
                            fn = input(f"\nXMODEM requested. {default_str}Filename?\n> ") or default_fn
                            tty.setraw(sys.stdin.fileno())
                            try:
                                xmodem_tx(term, fn)
                            except Exception as e:
                                print(e)
                                pass
                        print(data.decode('ascii', errors='ignore'), end='', flush=True)
                    elif s == sys.stdin.fileno():
                        char = sys.stdin.buffer.read(1)
                        if char == b'\x1d':
                            print("Exiting.")
                            return
                        term.write(char)
        finally:
            termios.tcsetattr(0, termios.TCSADRAIN, old_settings)

def checksum(data):
    return sum(data) % 256

def xmodem_tx(term, fn):
    with open(fn, 'rb') as f:
        def send_packet(packet_number, payload):
            packet = bytearray()
            packet.append(SOH[0])
            packet.append(packet_number & 0xFF)
            packet.append(0xFF - (packet_number & 0xFF))
            packet.extend(payload)
            packet.append(checksum(payload))
            term.write(packet)

        packet_number = 1
        last_packet = False
        term.read_all() # flush NAK XMODEM req from buffer
        while not last_packet:
            payload = bytearray(f.read(128))
            if len(payload) < 128:
                payload.extend(bytearray(SUB) * (128 - len(payload)))
                last_packet = True
            send_packet(packet_number, payload)

            while True:
                response = term.read(1)
                if response == ACK:
                    packet_number += 1
                    print('.', end='', flush=True)
                    break
                elif response == NAK:
                    send_packet(packet_number, payload)
                elif response == CAN:
                    print("Error: Remote canceled XMODEM transfer")
                    return False
                elif response == b'':
                    pass
                else:
                    time.sleep(1)

        term.write(EOT)
        while term.read(1) != ACK:
            term.write(EOT)
        return True

if __name__ == '__main__':
    main()
