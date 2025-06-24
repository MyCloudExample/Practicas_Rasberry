// one_wire.h
#ifndef ONE_WIRE_H
#define ONE_WIRE_H

#include "pico/stdlib.h"

typedef struct {
    uint pin;
} one_wire_t;

void one_wire_init(one_wire_t *one_wire, uint pin);
bool one_wire_reset(one_wire_t *one_wire);
void one_wire_write(one_wire_t *one_wire, uint8_t data);
uint8_t one_wire_read(one_wire_t *one_wire);
uint8_t one_wire_crc8(uint8_t *data, uint8_t len);

#endif
