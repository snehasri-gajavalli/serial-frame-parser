#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#include "../src/parser.h"

int main(void)
{
    FrameParser parser;

    parser_init(&parser);

    uint8_t frame[] = {
        0xAA,
        0x03,
        0x01,
        0x02,
        0x03,
        0x72
    };

    ParserResult result = PARSER_NONE;

    for (int i = 0; i < 6; i++)
    {
        result = parser_process_byte(&parser, frame[i]);
    }
    printf("Result = %d\n", result);
    printf("FRAME_VALID = %d\n", FRAME_VALID);
    assert(result == FRAME_VALID);

    printf("Valid frame test passed!\n");
        parser_init(&parser);

    uint8_t bad_frame[] = {
        0xAA,
        0x03,
        0x01,
        0x02,
        0x03,
        0x73
    };

    result = PARSER_NONE;

    for (int i = 0; i < 6; i++)
    {
        result = parser_process_byte(&parser, bad_frame[i]);
    }

    assert(result == FRAME_INVALID);

    printf("Invalid CRC test passed!\n");
        parser_init(&parser);

    uint8_t unexpected_start_frame[] = {
        0x55,
        0xAA,
        0x03,
        0x01,
        0x02,
        0x03,
        0x72
    };

    result = PARSER_NONE;

    for (int i = 0; i < 7; i++)
    {
        result = parser_process_byte(&parser, unexpected_start_frame[i]);
    }

    assert(result == FRAME_VALID);

    printf("Unexpected start byte test passed!\n");
        parser_init(&parser);

    uint8_t part1[] = {
        0xAA,
        0x03
    };

    uint8_t part2[] = {
        0x01,
        0x02
    };

    uint8_t part3[] = {
        0x03,
        0x72
    };

    result = PARSER_NONE;

    for (int i = 0; i < 2; i++)
    {
        result = parser_process_byte(&parser, part1[i]);
    }

    assert(result == PARSER_NONE);

    for (int i = 0; i < 2; i++)
    {
        result = parser_process_byte(&parser, part2[i]);
    }

    assert(result == PARSER_NONE);

    for (int i = 0; i < 2; i++)
    {
        result = parser_process_byte(&parser, part3[i]);
    }

    assert(result == FRAME_VALID);

    printf("Split frame test passed!\n");
        parser_init(&parser);

    result = parser_process_byte(&parser, 0xAA);

    assert(result == PARSER_NONE);

    result = parser_process_byte(&parser, 65);

    assert(result == PARSER_NONE);
    assert(parser.state == WAIT_START);

    printf("Oversized frame test passed!\n");

    return 0;
}