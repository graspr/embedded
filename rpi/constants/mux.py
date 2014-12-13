"""
Constants related to the MUX (ADG 706/707)
"""
from libbcm2835._bcm2835 import *

CHANNELS = {
    1: [0, 0, 0, 0],
    2: [0, 0, 0, 1],
    3: [0, 0, 1, 0],
    4: [0, 0, 1, 1],
    5: [0, 1, 0, 0],
    6: [0, 1, 0, 1],
    7: [0, 1, 1, 0],
    8: [0, 1, 1, 1],
    9: [1, 0, 0, 0],
    10: [1, 0, 0, 1],
    11: [1, 0, 1, 0],
    12: [1, 0, 1, 1],
    13: [1, 1, 0, 0],
    14: [1, 1, 0, 1],
    15: [1, 1, 1, 0],
    16: [1, 1, 1, 1],
}


# A0 = 9 #mapping from mux chan to RPI pin numbers
# A1 = 7
# A2 = 5
# A3 = 4
# EN = 8
A0 = RPI_V2_GPIO_P1_05 #mapping from mux chan to RPI pin numbers
A1 = RPI_V2_GPIO_P1_07
A2 = RPI_V2_GPIO_P1_18
A3 = RPI_V2_GPIO_P1_16
EN = RPI_V2_GPIO_P1_03
MUX_PINS = [A3, A2, A1, A0]



# 8 SDA RPI3 GPIO2 P1-03
# 9 SCLK RPI5 GPIO3 P1-05
