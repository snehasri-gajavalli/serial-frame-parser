#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#include "../src/crc8.h"

int main(void)
{
    const uint8_t data[] = {
        '1', '2', '3', '4', '5',
        '6', '7', '8', '9'
    };

    uint8_t result = crc8(data, 9);

    printf("CRC = 0x%02X\n", result);

    assert(result == 0xF4);

    printf("CRC test passed!\n");

    return 0;
}