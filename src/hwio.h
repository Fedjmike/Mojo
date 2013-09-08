#include "stdint.h"

void hwio_out8 (uint16_t port, uint8_t value);
uint8_t hwio_in8 (uint16_t port);
uint16_t hwio_in16 (uint16_t port);