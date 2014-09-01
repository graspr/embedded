import wiringpi2 as wpi
import signal
import sys
import time

class SPIError(Exception):
    """
    SPI Error
    """
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return str(self.value)

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

SPI_CLOCK_SPEED = int(1E6) #hz

CHANNELS = {
    14: [1, 1, 0, 1],
    15: [1, 1, 1, 0],
    16: [1, 1, 1, 1]
}

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
    wpi.digitalWrite(CSB, LOW)
    retcode = wpi.wiringPiSPIDataRW(SPI_CHANNEL, val) #drop this it's all 0's

    retcode = wpi.wiringPiSPIDataRW(SPI_CHANNEL, val)
    value_1 = _make_binary(val)
    retcode = wpi.wiringPiSPIDataRW(SPI_CHANNEL, val)
    value_2 = _make_binary(val)

    wpi.digitalWrite(CSB, HIGH)
    if retcode == -1:
        e = SPIError('ERROR PERFORMING SPI DATA RW on channel: {!s}, retcode: {!s}'.format(SPI_CHANNEL, retcode))
        raise e
    return '' + value_1 + '' + value_2

def run():
    """
    Main runloop
    """
    setup()
    print('Start of run: {!s}'.format(time.time()))
    print('Channel 14,Channel 15,Channel 16')

    while True:
        c15 = read(15)
        # print('{},' % read(14),)
        # print('{}' % str(int(c15,2)).zfill(5))
        # print(c15,)
        print(int(c15,2))
        # print('{}'  % read(16))

def setup():
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

    wpi.wiringPiSPISetup(0, SPI_CLOCK_SPEED)
    # spi.openSPI(speed=1000000, mode=0)




def signal_handler(signal, frame):
    """
    For when we hit CTRL+C!
    """
    print('End of run: {!s}'.format(time.time()))
    sys.exit(0)

if __name__ == "__main__":
    run()

