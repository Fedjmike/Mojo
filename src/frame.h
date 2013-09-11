#include "stdint.h"

void frame_init ();
uint32_t frame_alloc ();
void frame_free (uint32_t frame);