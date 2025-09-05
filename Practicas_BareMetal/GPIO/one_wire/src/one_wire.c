#include "one_wire.h"
#include "hardware/gpio.h"
#include "pico/time.h"  // Reemplaza hardware/delay.h por esto

void one_wire_init(one_wire_t *one_wire, uint pin) {
    one_wire->pin = pin;
    gpio_init(pin);
}

bool one_wire_reset(one_wire_t *one_wire) {
    gpio_set_dir(one_wire->pin, GPIO_OUT);
    gpio_put(one_wire->pin, 0);
    sleep_us(480);  // Usa sleep_us en lugar de busy_wait_us

    gpio_set_dir(one_wire->pin, GPIO_IN);
    sleep_us(70);

    bool presence = !gpio_get(one_wire->pin);
    sleep_us(410);

    return presence;
}

void one_wire_write_bit(one_wire_t *one_wire, bool bit) {
    gpio_set_dir(one_wire->pin, GPIO_OUT);
    gpio_put(one_wire->pin, 0);
    sleep_us(bit ? 10 : 65);

    gpio_put(one_wire->pin, 1);
    sleep_us(bit ? 55 : 5);
}

void one_wire_write(one_wire_t *one_wire, uint8_t data) {
    for (int i = 0; i < 8; i++) {
        one_wire_write_bit(one_wire, data & 0x01);
        data >>= 1;
    }
}

bool one_wire_read_bit(one_wire_t *one_wire) {
    gpio_set_dir(one_wire->pin, GPIO_OUT);
    gpio_put(one_wire->pin, 0);
    sleep_us(6);

    gpio_set_dir(one_wire->pin, GPIO_IN);
    sleep_us(9);

    bool value = gpio_get(one_wire->pin);
    sleep_us(55);

    return value;
}

uint8_t one_wire_read(one_wire_t *one_wire) {
    uint8_t value = 0;

    for (int i = 0; i < 8; i++) {
        value >>= 1;
        if (one_wire_read_bit(one_wire)) {
            value |= 0x80;
        }
    }

    return value;
}

uint8_t one_wire_crc8(uint8_t *data, uint8_t len) {
    uint8_t crc = 0;

    for (uint8_t i = 0; i < len; i++) {
        uint8_t inbyte = data[i];
        for (uint8_t j = 0; j < 8; j++) {
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix) crc ^= 0x8C;
            inbyte >>= 1;
        }
    }

    return crc;
}
