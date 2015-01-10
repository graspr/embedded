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

/** FUNCTION SIGNATURES **/
void spi_shutdown();
void shutdown();
void gpio_reset(void);
/** END FUNCTION SIGNATURES **/

/***** SETUP STUFF ****/
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
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_128);
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default
}

void spi_shutdown() {
    bcm2835_spi_end();
    gpio_reset();
    bcm2835_close();
}

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
    shutdown();
    exit(0);
}

uint64_t getTimeStamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}

void print_array(uint32_t* arr, int len) {
    int i;
    for (i=0; i<len; i++) {
        printf("%d,", arr[i]);
    }
    printf("\n");
}

void setup_signals() {
    signal(SIGINT, intHandler);
    signal(SIGPIPE, SIG_IGN);
}

void server_setup() {
    setup_signals();
    if (!bcm2835_init()) {
        printf("ERROR: BCM2835_INIT PROBLEM.\n\n");
        exit(1);
    }
    gpio_setup();
    spi_setup();
}

/***** END SETUP STUFF *********/


/***** VALUE READING STUFF ****/
void switch_to_channel(uint8_t chan) {
    if (chan > 16 || chan < 1) {
        printf("BAD CHANNEL! DOESN'T EXIST! %d\n", chan);
        printf("EXITING...\n");
        shutdown();
        exit(1);
    }
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
   // printf("Read from SPI (chan): (%d) %d:: %d :: %d :: %d\n", \
   //        16, buffer[0], buffer[1], buffer[2], val);
    return val;
}

static uint32_t readings[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
void mega_read() {
    int i;
    for (i = 1; i <= 16; i++) {
        readings[i-1] = spi_read(i);
    }
}
/***** END VALUE READING STUFF ****/

void main_loop(int PORT) {
    struct timeval tvalBefore, tvalAfter;  // removed comma
    gettimeofday(&tvalBefore, NULL);
    int READS = 0;
    int NUM_READS = 100;
    int SOCKET_SEND_CODE = 0;
    create_socket(PORT);
    while (1) {
        accept_new_socket_connection();
        while (1) {
            if (READS == NUM_READS) {
                gettimeofday(&tvalAfter, NULL);
                float delta = ((tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000.0 \
                    + tvalAfter.tv_usec) - tvalBefore.tv_usec;
                float reads_per_sec = NUM_READS/(delta/(1000000.0));
                
                printf("SPEED: %f HZ\n", reads_per_sec);
                // print_array(readings, 16);
                READS = 0;
                gettimeofday(&tvalBefore, NULL);
            }
            mega_read();
            SOCKET_SEND_CODE = send_data(readings);
            if (SOCKET_SEND_CODE == SOCKET_SEND_ERROR) {
                close_active_socket();
                break;
            }
            READS++;
        }
    }
}

int main(int argc, char **argv) {
    if (!argv[1]) {
        printf("Specify a port (first argument)!\n\n");
        exit(1);
    }
    int PORT = atoi(argv[1]);
    server_setup();
    main_loop(PORT);
    shutdown();
    return 0;
}

void shutdown() {
    shutdown_socket();
    spi_shutdown();
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
