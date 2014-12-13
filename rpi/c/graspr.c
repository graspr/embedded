#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MODE_READ 0
#define MODE_SET 1
#define MODE_CLR 2
#define MODE_INPUT_READ 3

#define PULL_UP 0
#define PULL_DOWN 1
#define NO_PULL 2
#define GPIO_BEGIN 0
#define GPIO_END 1
#define NO_ACTION 2

#define NO_PIN 40 // Some big number that's beyond the connector's pin count
#define DEBUG_OFF 0
#define DEBUG_ON 1

uint8_t  init = NO_ACTION;
uint8_t  pull = NO_PULL;
uint8_t  mode = MODE_READ;
uint8_t  pin_number = NO_PIN;

uint8_t CSB = RPI_V2_GPIO_P1_24;
uint8_t MISO = RPI_V2_GPIO_P1_21;
uint8_t MOSI = RPI_V2_GPIO_P1_19;
uint8_t SCKL = RPI_V2_GPIO_P1_23;

uint8_t A0 = RPI_V2_GPIO_P1_05; //mapping from mux chan to RPI pin numbers
uint8_t A1 = RPI_V2_GPIO_P1_07;
uint8_t A2 = RPI_V2_GPIO_P1_18;
uint8_t A3 = RPI_V2_GPIO_P1_16;
uint8_t EN = RPI_V2_GPIO_P1_03;
uint8_t MUX_PINS[4];

uint8_t i, len;
uint8_t data, pin, debug_mode = DEBUG_OFF;

void gpio_reset(void);

void spi_setup()
{
	MUX_PINS[0] = A3;
	MUX_PINS[1] = A2;
	MUX_PINS[2] = A1;
	MUX_PINS[3] = A0;
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536); // The default
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default

}

void spi_read()
{
    uint8_t data = bcm2835_spi_transfer(0x23);
    printf("Read from SPI: %02X\n", data);
    data = bcm2835_spi_transfer(0x23);
    printf("Read from SPI: %02X\n", data);
    data = bcm2835_spi_transfer(0x23);
    printf("Read from SPI: %02X\n", data);

}

void spi_shutdown()
{
    bcm2835_spi_end();
    bcm2835_close();
}

void gpio_read(int pin)
{
	bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
    data = bcm2835_gpio_lev(pin);
    printf("Reading pin: %d\n", data);
}

void gpio_write(int pin)
{
	bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_set(pin); //?????
}

int main(int argc, char **argv)
{
	if (!bcm2835_init())
	{
		return 1;
	}
    
	gpio_read(A0);
	spi_setup();
	spi_read();
	spi_shutdown();
	printf("... done!\n");

	return 0;
}


void gpio_reset(void) {
	bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_03, BCM2835_GPIO_PUD_OFF);
	bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_05, BCM2835_GPIO_PUD_OFF);
	bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_07, BCM2835_GPIO_PUD_OFF);
	bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_26, BCM2835_GPIO_PUD_OFF);
	bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_24, BCM2835_GPIO_PUD_OFF);
	bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_21, BCM2835_GPIO_PUD_OFF);
	bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_19, BCM2835_GPIO_PUD_OFF);
	bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_23, BCM2835_GPIO_PUD_OFF);
	bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_10, BCM2835_GPIO_PUD_OFF);
	bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_11, BCM2835_GPIO_PUD_OFF);
	bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_12, BCM2835_GPIO_PUD_OFF);
	bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_13, BCM2835_GPIO_PUD_OFF);
	bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_15, BCM2835_GPIO_PUD_OFF);
	bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_16, BCM2835_GPIO_PUD_OFF);
	bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_18, BCM2835_GPIO_PUD_OFF);
	bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_22, BCM2835_GPIO_PUD_OFF);

	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_03, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_05, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_07, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_26, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_24, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_21, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_19, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_23, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_10, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_11, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_12, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_13, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_15, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_16, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_18, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_22, BCM2835_GPIO_FSEL_INPT);
}