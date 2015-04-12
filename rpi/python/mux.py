"""
Raspberry Pi B+ Code for muxer ADG_707 or ADG_706
"""
# import wiringpi2 as wpi
from constants import mux as MUX
from constants import rpi as RPI
import spi
import time
import sys
from libbcm2835._bcm2835 import *

READS = range(1,17)

def high_low(pin, HIGH=0):
    if HIGH:
        bcm2835_gpio_set(pin)
    else:
        bcm2835_gpio_clr(pin)

def switch_to_channel(channel):
    """
    Switches on the correct channel on the mux
    """
    conf = MUX.CHANNELS[channel]

    # for i, pin_value in enumerate(conf):
    #     wpi.digitalWrite(MUX.MUX_PINS[i], pin_value)
    high_low(MUX.A3, conf[0])
    high_low(MUX.A2, conf[1])
    high_low(MUX.A1, conf[2])
    high_low(MUX.A0, conf[3])
    # wpi.digitalWrite(MUX.A3, conf[0])
    # wpi.digitalWrite(MUX.A2, conf[1])
    # wpi.digitalWrite(MUX.A1, conf[2])
    # wpi.digitalWrite(MUX.A0, conf[3])

def _make_binary(num):
    """
    Just formats the number into padded 8 bit _make_binary
    """
    # return format(ord(num), 'b').zfill(8)
    return num

def read(channel):
    """
    Reads the value of a channel (on the mux) from the adc
    """
    # switch_to_channel(channel)
    val = '\n'
    value_1 = 0
    value_2 = 0
    retcode = 0
    try:
        # wpi.digitalWrite(RPI.CSB, RPI.LOW)
        bcm2835_gpio_clr(RPI.CSB)
        val = bcm2835_spi_transfer(0x23);
        val = bcm2835_spi_transfer(0x23);
        # retcode = wpi.wiringPiSPIDataRW(RPI.SPI_CHANNEL, val) #drop this it's all 0's
        # retcode = wpi.wiringPiSPIDataRW(RPI.SPI_CHANNEL, val)
        # value_1 = _make_binary(val)
        value_1 = _make_binary(val)
        # retcode = wpi.wiringPiSPIDataRW(RPI.SPI_CHANNEL, val)
        val = bcm2835_spi_transfer(0x23);
        # value_2 = _make_binary(val)
        value_2 = _make_binary(val)

        bcm2835_gpio_clr(RPI.CSB)
    except Exception as err:
        sys.stderr.write("Error: {0}".format(err))
        sys.stderr.flush()
    return '%s%s' % (value_1, value_2)

