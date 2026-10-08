#include "parser.h"
#include "crc8.h"

void parser_init(FrameParser *parser)
{
    parser->state = WAIT_START;
    parser->length = 0;
    parser->payload_index = 0;
    parser->received_crc = 0;
}

ParserResult parser_process_byte(
    FrameParser *parser,
    uint8_t byte,
    ParsedFrame *frame
)
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
        /*
         * Invalid length.
         *
         * If the invalid byte is also START_BYTE,
         * treat it as the beginning of a new frame.
         */
            if (byte == START_BYTE)
            {
                parser->state = READ_LENGTH;
            }
            else
            {
                parser->state = WAIT_START;
            }
        }
        else
        {
            parser->length = byte;
            parser->payload_index = 0;

            if (parser->length == 0)
            {
            /* No payload, so the next byte is CRC */
                parser->state = READ_CRC;
            }
            else
            {
                parser->state = READ_PAYLOAD;
            }
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

            /*
             * CRC is calculated over:
             * LENGTH + PAYLOAD
             */
            uint8_t crc_data[MAX_PAYLOAD_SIZE + 1];

            crc_data[0] = parser->length;

            for (uint8_t i = 0; i < parser->length; i++)
            {
                crc_data[i + 1] = parser->payload[i];
            }

            uint8_t calculated_crc =
                crc8(crc_data, parser->length + 1);

            if (calculated_crc == parser->received_crc)
            {
                result = FRAME_VALID;

                /*
                 * Copy the successfully parsed frame
                 * to the output structure.
                 */
                frame->length = parser->length;

                for (uint8_t i = 0; i < parser->length; i++)
                {
                    frame->payload[i] = parser->payload[i];
                }
            }
            else
            {
                result = FRAME_INVALID;
            }

            /* Ready for the next frame */
            parser->state = WAIT_START;

            break;
        }
    }

    return result;
}