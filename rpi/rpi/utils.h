//
//  utils.h
//  rpi
//
//  Created by Anton de Winter on 4/12/15.
//  Copyright (c) 2015 Anton de Winter. All rights reserved.
//

#ifndef rpi_utils_h
#define rpi_utils_h

uint64_t getTimeStamp();
void print_array(uint32_t* arr, int len);
void printBits(size_t const size, void const * const ptr);

#endif
