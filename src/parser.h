#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

#define START_BYTE 0xAA
#define MAX_PAYLOAD_SIZE 64

typedef enum
{
    WAIT_START,
    READ_LENGTH,
    READ_PAYLOAD,
    READ_CRC
} ParserState;

typedef enum
{
    PARSER_NONE,
    FRAME_VALID,
    FRAME_INVALID
} ParserResult;

/* A successfully parsed frame */
typedef struct
{
    uint8_t length;
    uint8_t payload[MAX_PAYLOAD_SIZE];
} ParsedFrame;

/* Internal parser state */
typedef struct
{
    ParserState state;

    uint8_t length;
    uint8_t payload[MAX_PAYLOAD_SIZE];
    uint8_t payload_index;
    uint8_t received_crc;

} FrameParser;

void parser_init(FrameParser *parser);

ParserResult parser_process_byte(
    FrameParser *parser,
    uint8_t byte,
    ParsedFrame *frame
);

#endif