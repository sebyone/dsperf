#include "utils.h"
#include <stdint.h>

uint32_t swap_endian_32(uint32_t num)
{
    return ((num >> 24) & 0xFFu) |      // Move byte 3 to byte 0
           ((num << 8) & 0xFF0000u) |   // Move byte 1 to byte 2
           ((num >> 8) & 0xFF00u) |     // Move byte 2 to byte 1
           ((num << 24) & 0xFF000000u); // Move byte 0 to byte 3
}
// ------------------------
