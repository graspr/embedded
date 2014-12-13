"""
Constants related to the Raspberry Pi B+ board
"""
from libbcm2835._bcm2835 import *

HIGH = 1
LOW = 0
OUTPUT = 1
INPUT = 0

# CSB = 10
# MISO = 13
# MOSI = 12
# SCKL = 14

CSB = RPI_V2_GPIO_P1_24
MISO = RPI_V2_GPIO_P1_21
MOSI = RPI_V2_GPIO_P1_19
SCKL = RPI_V2_GPIO_P1_23


SPI_CHANNEL = 0  #on RPI there are two SPI channels, 0 and 1
SPI_CLOCK_SPEED = int((2E7)) #hz == ~2.62mhz
