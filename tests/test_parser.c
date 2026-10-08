#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#include "../src/parser.h"

int main(void)
{
    FrameParser parser;
    ParsedFrame frame;

    /* =========================================================
       TEST 1: Valid frame
       Frame: AA 03 01 02 03 72
       ========================================================= */

    parser_init(&parser);

    uint8_t valid_frame[] = {
        0xAA,   /* START */
        0x03,   /* LENGTH */
        0x01,   /* PAYLOAD */
        0x02,
        0x03,
        0x72    /* CRC */
    };

    ParserResult result = PARSER_NONE;

    for (int i = 0; i < 6; i++)
    {
        result = parser_process_byte(
            &parser,
            valid_frame[i],
            &frame
        );
    }

    assert(result == FRAME_VALID);

    /* Check parsed data */
    assert(frame.length == 3);
    assert(frame.payload[0] == 0x01);
    assert(frame.payload[1] == 0x02);
    assert(frame.payload[2] == 0x03);

    printf("Valid frame test passed!\n");


    /* =========================================================
       TEST 2: Invalid CRC
       ========================================================= */

    parser_init(&parser);

    uint8_t invalid_crc_frame[] = {
        0xAA,
        0x03,
        0x01,
        0x02,
        0x03,
        0x00       /* Wrong CRC */
    };

    result = PARSER_NONE;

    for (int i = 0; i < 6; i++)
    {
        result = parser_process_byte(
            &parser,
            invalid_crc_frame[i],
            &frame
        );
    }

    assert(result == FRAME_INVALID);

    printf("Invalid CRC test passed!\n");


    /* =========================================================
       TEST 3: Unexpected bytes before START
       ========================================================= */

    parser_init(&parser);

    uint8_t unexpected_bytes[] = {
        0x10,
        0x20,
        0x55,
        0xAA,
        0x03,
        0x01,
        0x02,
        0x03,
        0x72
    };

    result = PARSER_NONE;

    for (int i = 0; i < 9; i++)
    {
        result = parser_process_byte(
            &parser,
            unexpected_bytes[i],
            &frame
        );
    }

    assert(result == FRAME_VALID);

    assert(frame.length == 3);
    assert(frame.payload[0] == 0x01);
    assert(frame.payload[1] == 0x02);
    assert(frame.payload[2] == 0x03);

    printf("Unexpected start byte test passed!\n");


    /* =========================================================
       TEST 4: Frame split across multiple calls
       ========================================================= */

    parser_init(&parser);

    /* First part */
    uint8_t part1[] = {
        0xAA,
        0x03,
        0x01
    };

    result = PARSER_NONE;

    for (int i = 0; i < 3; i++)
    {
        result = parser_process_byte(
            &parser,
            part1[i],
            &frame
        );
    }

    /* Frame is not complete yet */
    assert(result == PARSER_NONE);

    /* Second part */
    uint8_t part2[] = {
        0x02,
        0x03,
        0x72
    };

    for (int i = 0; i < 3; i++)
    {
        result = parser_process_byte(
            &parser,
            part2[i],
            &frame
        );
    }

    assert(result == FRAME_VALID);

    assert(frame.length == 3);
    assert(frame.payload[0] == 0x01);
    assert(frame.payload[1] == 0x02);
    assert(frame.payload[2] == 0x03);

    printf("Split frame test passed!\n");


    /* =========================================================
       TEST 5: Oversized payload
       LENGTH = 65
       Maximum allowed = 64
       ========================================================= */

    parser_init(&parser);

    uint8_t oversized_frame[] = {
        0xAA,
        65
    };

    result = PARSER_NONE;

    for (int i = 0; i < 2; i++)
    {
        result = parser_process_byte(
            &parser,
            oversized_frame[i],
            &frame
        );
    }

    /*
     * Parser should reject the frame
     * and return to WAIT_START.
     */
    assert(parser.state == WAIT_START);
    assert(result == PARSER_NONE);

    printf("Oversized frame test passed!\n");


    /* =========================================================
       TEST 6: Zero-length payload
       Frame: AA 00 00
       ========================================================= */

    parser_init(&parser);

    uint8_t zero_length_frame[] = {
        0xAA,
        0x00,
        0x00
    };

    result = PARSER_NONE;

    for (int i = 0; i < 3; i++)
    {
        result = parser_process_byte(
            &parser,
            zero_length_frame[i],
            &frame
        );
    }

    assert(result == FRAME_VALID);

    assert(frame.length == 0);

    printf("Zero-length frame test passed!\n");


    /* =========================================================
       ALL TESTS PASSED
       ========================================================= */

    printf("\nAll parser tests passed!\n");

    return 0;
}