"""
Raspberry Pi B+ SPI communication code
"""
# import wiringpi2 as wpi
from constants import rpi as RPI
from constants import mux as MUX
from libbcm2835._bcm2835 import *

class SPIError(Exception):
    """
    SPI Error
    """
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return str(self.value)

def setup(SPI_SPEED=RPI.SPI_CLOCK_SPEED):
    """
    Perform initial board and SPI setup
    """
    
    # wpi.wiringPiSetup()
    # for pin in MUX.MUX_PINS:
    #     wpi.pinMode(pin, RPI.OUTPUT)

    # wpi.pinMode(MUX.EN, RPI.OUTPUT)
    # wpi.digitalWrite(MUX.EN, RPI.HIGH) #EN should always be high.
    # wpi.pinMode(RPI.CSB, RPI.OUTPUT)
    # wpi.wiringPiSPISetup(0, SPI_SPEED)


    if not bcm2835_init():
        return

    for pin in MUX.MUX_PINS:
        bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);

    bcm2835_gpio_fsel(MUX.EN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_set(MUX.EN)

    bcm2835_spi_begin()
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST)      # The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0)                   # The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536) # The default
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0)                      # The default
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW)      # the default
