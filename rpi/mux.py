"""
Raspberry Pi B+ Code for muxer ADG_707 or ADG_706
"""
import wiringpi2 as wpi
from constants import mux as MUX
from constants import rpi as RPI
import spi

def switch_to_channel(channel):
    """
    Switches on the correct channel on the mux
    """
    conf = MUX.CHANNELS[channel]
    for i, pin_value in enumerate(conf):
        wpi.digitalWrite(MUX.MUX_PINS[i], pin_value)

def _make_binary(num):
    """
    TODO: Investigate this. Can we just use 
    
    return format(num, '08b') 
    
    instead?
    """
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
        wpi.digitalWrite(RPI.CSB, RPI.LOW)
        retcode = wpi.wiringPiSPIDataRW(RPI.SPI_CHANNEL, val) #drop this it's all 0's
        retcode = wpi.wiringPiSPIDataRW(RPI.SPI_CHANNEL, val)
        value_1 = _make_binary(val)
        retcode = wpi.wiringPiSPIDataRW(RPI.SPI_CHANNEL, val)
        value_2 = _make_binary(val)

        wpi.digitalWrite(RPI.CSB, RPI.HIGH)
    except Exception as err:
        print("Error: {0}".format(err))
    # if retcode == -1:
    #     print((type(retcode)))
    #     print((type(SPI_CHANNEL)))
    #     err = 'ERROR PERFORMING SPI DATA RW on channel: {:s}, \
    #            retcode: {:s}'.format(str(SPI_CHANNEL),str(retcode))
    #     e = SPIError(err)
    #     print(err)
    #     # raise e
    return '{}{}'.format(value_1, value_2)
