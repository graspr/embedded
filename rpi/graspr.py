import wiringpi2 as wpi
import signal
import sys
import time
from thread import *
from collections import deque
import web

class SPIError(Exception):
    """
    SPI Error
    """
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return str(self.value)

PORT = 8080

HIGH = 1
LOW = 0
OUTPUT = 1
INPUT = 0

A0 = 9 #mapping from mux chan to WPI pin numbers
A1 = 7
A2 = 5
A3 = 4
EN = 8
MUX_PINS = [A3, A2, A1, A0]

CSB = 10
MISO = 13
MOSI = 12
SCKL = 14

SPI_CHANNEL = 0  #on RPI there are two SPI channels, 0 and 1

SPI_CLOCK_SPEED = int(2E6) #hz

CHANNELS = {
    14: [1, 1, 0, 1],
    15: [1, 1, 1, 0],
    16: [1, 1, 1, 1]
}

DQ_MAX_LENGTH = 10000
DATA = deque(maxlen=DQ_MAX_LENGTH)


def switch_to_channel(channel):
    """
    Switches on the correct channel on the mux
    """
    conf = CHANNELS[channel]
    for i, pin_value in enumerate(conf):
        wpi.digitalWrite(MUX_PINS[i], pin_value)

def _make_binary(num):
    return format(ord(num), 'b').zfill(8)

def read(channel):
    """
    Reads the value of a channel (on the mux) from the adc
    """
    switch_to_channel(channel)
    val = '\n'
    value_1 = 0
    value_2 = 0
    retcode = 0
    try:
        wpi.digitalWrite(CSB, LOW)
        retcode = wpi.wiringPiSPIDataRW(SPI_CHANNEL, val) #drop this it's all 0's
        retcode = wpi.wiringPiSPIDataRW(SPI_CHANNEL, val)
        value_1 = _make_binary(val)
        retcode = wpi.wiringPiSPIDataRW(SPI_CHANNEL, val)
        value_2 = _make_binary(val)

        wpi.digitalWrite(CSB, HIGH)
    except Exception as err:
        print("Error: {0}".format(err))
    # if retcode == -1:
    #     print((type(retcode)))
    #     print((type(SPI_CHANNEL)))
    #     err = 'ERROR PERFORMING SPI DATA RW on channel: {:s}, retcode: {:s}'.format(str(SPI_CHANNEL),str(retcode))
    #     e = SPIError(err)
    #     print(err)
    #     # raise e
    return '{}{}'.format(value_1,value_2)

def run():
    """
    Main runloop
    """
    while True:
        read_14 = read(14)
        read_15 = read(15)
        read_16 = read(16)
        yield "{},{:s},{:s},{:s},{},{},{}".format(int(time.time()*1000), read_14, read_15, read_16, int(read_14, 2), int(read_15, 2), int(read_16, 2))

def setup(SPI_SPEED=SPI_CLOCK_SPEED):
    """
    Perform initial board and SPI setup
    """
    signal.signal(signal.SIGINT, signal_handler) #handler for keyboard interrupt
    wpi.wiringPiSetup()
    for pin in MUX_PINS:
        wpi.pinMode(pin, OUTPUT)

    wpi.pinMode(EN, OUTPUT)
    wpi.digitalWrite(EN, HIGH) #EN should always be high.
    wpi.pinMode(CSB, OUTPUT)
    wpi.wiringPiSPISetup(0, SPI_SPEED)

def signal_handler(signal, frame):
    """
    For when we hit CTRL+C!
    """
    print(('End of run: {!s}'.format(time.time())))
    sys.exit(0)


def server(args):
    global DATA
    runner = run()
    sys.stdout.write('===============')
    sys.stdout.flush()
    for i in runner:
        DATA.append(i)

def spawn_in_thread():
    global DATA
    setup()
    start_new_thread(server,(None,))
    start_new_thread(web.run,(DATA, PORT))

if __name__ == "__main__":
    print('Setting up')
    setup()
    print(('Start of run: {!s}'.format(time.time())))
    print('Channel 14,Channel 15,Channel 16')

    if len(sys.argv) > 1:
        PORT = sys.argv[1]

    spawn_in_thread()
    
    while True:
        if len(DATA) != DQ_MAX_LENGTH:
            toprint = "{}\r".format(len(DATA))
            sys.stdout.write(toprint)
        else:
            sys.stdout.write('FULL! {}\r'.format(str(int(time.time()))))

        sys.stdout.flush()
        # time.sleep(0.1)
    # runner = run()
    # for i in runner:
    #     print(i)

