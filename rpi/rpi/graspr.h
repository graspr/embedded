//
//  graspr.h
//  rpi
//
//  Created by Anton de Winter on 4/12/15.
//  Copyright (c) 2015 Anton de Winter. All rights reserved.
//

#ifndef rpi_graspr_h
#define rpi_graspr_h

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

static uint32_t readings[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint32_t spi_read_val;

void spi_shutdown();
void graspr_shutdown();
void gpio_reset(void);


#endif
