#include "parser.h"
#include "crc8.h"
void parser_init(FrameParser *parser)
{
    parser->state = WAIT_START;
    parser->length = 0;
    parser->payload_index = 0;
    parser->received_crc = 0;
}

ParserResult parser_process_byte(FrameParser *parser, uint8_t byte)
{
    ParserResult result = PARSER_NONE;
    switch (parser->state)
    {
        case WAIT_START:

            if (byte == START_BYTE)
            {
                parser->state = READ_LENGTH;
            }

            break;

        case READ_LENGTH:

            if (byte > MAX_PAYLOAD_SIZE)
            {
                parser->state = WAIT_START;
            }
            else
            {
                parser->length = byte;
                parser->payload_index = 0;
                parser->state = READ_PAYLOAD;
            }

            break;

        case READ_PAYLOAD:

            parser->payload[parser->payload_index] = byte;
            parser->payload_index++;

            if (parser->payload_index >= parser->length)
            {
                parser->state = READ_CRC;
            }

            break;

        case READ_CRC:
        {
            parser->received_crc = byte;

            uint8_t crc_data[65];

            crc_data[0] = parser->length;

            for (uint8_t i = 0; i < parser->length; i++)
            {
                crc_data[i + 1] = parser->payload[i];
            }

            uint8_t calculated_crc = crc8(crc_data, parser->length + 1);


            if (calculated_crc == parser->received_crc)
            {
                result = FRAME_VALID;
            }
            else
            {
                result = FRAME_INVALID;
            }
            parser->state = WAIT_START;

            break;
        }
    }
    return result;
}