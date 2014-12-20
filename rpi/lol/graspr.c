#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <signal.h>
#include "server.h"


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

#define NO_PIN 40  // Some big number that's beyond the connector's pin count
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

uint8_t A0 = RPI_V2_GPIO_P1_05;  // mapping from mux chan to RPI pin numbers
uint8_t A1 = RPI_V2_GPIO_P1_07;
uint8_t A2 = RPI_V2_GPIO_P1_18;
uint8_t A3 = RPI_V2_GPIO_P1_16;
uint8_t EN = RPI_V2_GPIO_P1_03;
uint8_t MUX_PINS[4];
uint32_t BIT_MASK;
uint8_t i, len;
uint8_t data, pin, debug_mode = DEBUG_OFF;

char buffer[3];

int CHANNELS[16][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 1},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 1, 0, 0},
    {0, 1, 0, 1},
    {0, 1, 1, 0},
    {0, 1, 1, 1},
    {1, 0, 0, 0},
    {1, 0, 0, 1},
    {1, 0, 1, 0},
    {1, 0, 1, 1},
    {1, 1, 0, 0},
    {1, 1, 0, 1},
    {1, 1, 1, 0},
    {1, 1, 1, 1},
};

uint8_t CURRENT_CHANNEL = 1;

void gpio_reset(void);

void spi_setup() {
    MUX_PINS[0] = A3;
    MUX_PINS[1] = A2;
    MUX_PINS[2] = A1;
    MUX_PINS[3] = A0;
    bcm2835_gpio_fsel(CSB, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(EN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default
}

void switch_to_channel(uint8_t chan) {
    BIT_MASK = 1 << A0 | 1 << A1 | 1 << A2 | 1 << A3;
    bcm2835_gpio_clr_multi(BIT_MASK);

    BIT_MASK = 0;
    int conf[4];
    memcpy(conf, CHANNELS[chan-1], sizeof conf);
    BIT_MASK = conf[0] << A0 | conf[1] << A1 | conf[2] << A2 | conf[3] << A3;
    bcm2835_gpio_set_multi(BIT_MASK);
    CURRENT_CHANNEL = chan;
}


uint32_t val;
uint32_t spi_read(uint8_t channel) {
    switch_to_channel(channel);
    bcm2835_spi_transfern(buffer, 3);
    val = (buffer[1] << 8) + buffer[2];
    // printf("Read from SPI: %d:: %d :: %d :: %d\n", \
    //        buffer[0], buffer[1], buffer[2], val);
    return val;
}

void spi_shutdown() {
    bcm2835_spi_end();
    gpio_reset();
    bcm2835_close();
}


// void gpio_read(int pin)
// {
//  bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
//     data = bcm2835_gpio_lev(pin);
//     printf("Reading pin: %d\n", data);
// }

// void gpio_write(int pin)
// {
//  bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
//  bcm2835_gpio_set(pin); //?????
// }

void gpio_setup() {
    int x;
    for (x = 0; x < 4; x++) {
        bcm2835_gpio_fsel(MUX_PINS[x], BCM2835_GPIO_FSEL_OUTP);
    }
    uint32_t zero = 0;
    bcm2835_gpio_clr_multi(zero);
}

void intHandler(int dummy) {
    printf("SHUTTING DOWN\n");
    spi_shutdown();
    exit(0);
}

uint64_t getTimeStamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}

uint32_t vals[16];
uint32_t* mega_read() {
    int i;
    for (i = 0; i < 17; i++) {
        vals[i] = spi_read(i);
    }
    return vals;
}

int main(int argc, char **argv) {
    struct timeval tvalBefore, tvalAfter;  // removed comma

    gettimeofday(&tvalBefore, NULL);
    // uint64_t startTime = getTimeStamp();
    signal(SIGINT, intHandler);
    if (!bcm2835_init()) {
        return 1;
    }
    // gpio_read(A0);
    gpio_setup();
    spi_setup();
    // create_socket(8082);
    int x = 0;
    int i = 1;
    int READS = 0;
    int NUM_READS = 10000;
    uint32_t *lol;
    // switch_to_channel(2);
    lol = mega_read();
    while (1) {
        if (i == 17) {
            i = 1;
        }

        if (READS == NUM_READS) {
            gettimeofday(&tvalAfter, NULL);
            float delta = ((tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000.0 \
                + tvalAfter.tv_usec) - tvalBefore.tv_usec;
            float reads_per_sec = NUM_READS/(delta/(1000000.0));
            printf("READS:%i SPEED: %f HZ\n", READS, reads_per_sec);
            // char* buffer = spi_read(i);
            
            
            
            printf("BUFFER!!! %d, %d, %d \n", &(lol[13]), &lol[14], &lol[15]);
            READS = 0;
            gettimeofday(&tvalBefore, NULL);
        }
        spi_read(i);
        READS++;
        x++;
        i++;
    }
    // printf("eh?");
    // gettimeofday (&tvalAfter, NULL);

    // long delta = ((tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000L + \
    // tvalAfter.tv_usec) - tvalBefore.tv_usec;
    // printf("Time in microseconds: %ld microseconds\n", delta);
    // float reads_per_sec = (float)NUM_READS/((float)delta/(1000000.0));
    // printf("... done!\n");
    // // uint64_t endTime = getTimeStamp();
    //  uint64_t timeElapsed = (endTime - startTime)/(1000000);
    // microseconds to seconds
    // printf("SPEED: %f HZ", reads_per_sec);
    // printf("startime %s, endtime %s", startTime, endTime);
    spi_shutdown();
    return 0;
}


void gpio_reset(void) {
    // bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_03, BCM2835_GPIO_PUD_OFF);
    // bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_05, BCM2835_GPIO_PUD_OFF);
    // bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_07, BCM2835_GPIO_PUD_OFF);
    // bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_26, BCM2835_GPIO_PUD_OFF);
    // bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_24, BCM2835_GPIO_PUD_OFF);
    // bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_21, BCM2835_GPIO_PUD_OFF);
    // bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_19, BCM2835_GPIO_PUD_OFF);
    // bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_23, BCM2835_GPIO_PUD_OFF);
    // bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_10, BCM2835_GPIO_PUD_OFF);
    // bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_11, BCM2835_GPIO_PUD_OFF);
    // bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_12, BCM2835_GPIO_PUD_OFF);
    // bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_13, BCM2835_GPIO_PUD_OFF);
    // bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_15, BCM2835_GPIO_PUD_OFF);
    // bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_16, BCM2835_GPIO_PUD_OFF);
    // bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_18, BCM2835_GPIO_PUD_OFF);
    // bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_22, BCM2835_GPIO_PUD_OFF);

    // bcm2835_gpio_fsel(RPI_V2_GPIO_P1_03, BCM2835_GPIO_FSEL_INPT);
    // bcm2835_gpio_fsel(RPI_V2_GPIO_P1_05, BCM2835_GPIO_FSEL_INPT);
    // bcm2835_gpio_fsel(RPI_V2_GPIO_P1_07, BCM2835_GPIO_FSEL_INPT);
    // bcm2835_gpio_fsel(RPI_V2_GPIO_P1_26, BCM2835_GPIO_FSEL_INPT);
    // bcm2835_gpio_fsel(RPI_V2_GPIO_P1_24, BCM2835_GPIO_FSEL_INPT);
    // bcm2835_gpio_fsel(RPI_V2_GPIO_P1_21, BCM2835_GPIO_FSEL_INPT);
    // bcm2835_gpio_fsel(RPI_V2_GPIO_P1_19, BCM2835_GPIO_FSEL_INPT);
    // bcm2835_gpio_fsel(RPI_V2_GPIO_P1_23, BCM2835_GPIO_FSEL_INPT);
    // bcm2835_gpio_fsel(RPI_V2_GPIO_P1_10, BCM2835_GPIO_FSEL_INPT);
    // bcm2835_gpio_fsel(RPI_V2_GPIO_P1_11, BCM2835_GPIO_FSEL_INPT);
    // bcm2835_gpio_fsel(RPI_V2_GPIO_P1_12, BCM2835_GPIO_FSEL_INPT);
    // bcm2835_gpio_fsel(RPI_V2_GPIO_P1_13, BCM2835_GPIO_FSEL_INPT);
    // bcm2835_gpio_fsel(RPI_V2_GPIO_P1_15, BCM2835_GPIO_FSEL_INPT);
    // bcm2835_gpio_fsel(RPI_V2_GPIO_P1_16, BCM2835_GPIO_FSEL_INPT);
    // bcm2835_gpio_fsel(RPI_V2_GPIO_P1_18, BCM2835_GPIO_FSEL_INPT);
    // bcm2835_gpio_fsel(RPI_V2_GPIO_P1_22, BCM2835_GPIO_FSEL_INPT);
}
