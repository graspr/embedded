import wiringpi2 as wpi
import signal
import sys
import time



HIGH = 1
LOW = 0
OUTPUT = 1
INPUT = 0

A0 = 9 #mapping from mux chan to WPI pin numbers...
A1 = 7
A2 = 5
A3 = 4
EN = 8
MUX_PINS = [A0, A1, A2, A3]

CSB = 10
MISO = 13
MOSI = 12
SCKL = 14

SPI_CHANNEL = 0  #on RPI there are two SPI channels, 0 and 1

SPI_CLOCK_SPEED = 2000000 #hz

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

def read(channel):
    """
    Reads the value of a channel (on the mux) from the adc
    """
    switch_to_channel(channel)
    val = 16777216 #send a 24 bit number

    retcode = wpi.wiringPiSPIDataRW(SPI_CHANNEL, val)
    if retcode == -1:
        raise Exception('ERROR PERFORMING SPI DATA RW on channel: %s, \
                         retcode: %s' % (SPI_CHANNEL, retcode))
    return val

def run():
    """
    Main runloop
    """
    setup()
    print 'Start of run: %s' % time.time()
    print 'Channel 14,Channel 15,Channel 16'
    # print 'HELLO'
    # foo = 0b000000000000000000000000
    # wpi.wiringPiSPIDataRW(0, foo)
    # print '.'
    # print type(foo)
    # print foo
    # print '.'

    while True:
        print '%s,' % read(14),
        print '%s,' % read(15),
        print '%s'  % read(16)

def setup():
    """
    Perform initial board and SPI setup
    """
    wpi.wiringPiSetup()

    for pin in MUX_PINS:
        wpi.pinMode(pin, OUTPUT)
    wpi.pinMode(EN, OUTPUT)
    wpi.digitalWrite(EN, HIGH) #EN should always be high.

    wpi.wiringPiSPISetup(0, SPI_CLOCK_SPEED)


if __name__ == "__main__":
    run()

def signal_handler(signal, frame):
    print 'End of run: %s' % time.time()
    sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)

