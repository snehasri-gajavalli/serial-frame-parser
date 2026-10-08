#include <stdio.h>
#include <stdint.h>

#include "../src/parser.h"

static void process_uart_stream(
    FrameParser *parser,
    const uint8_t *data,
    int length,
    ParsedFrame *frame
)
{
    for (int i = 0; i < length; i++)
    {
        uint8_t received_byte = data[i];

        printf("UART received: 0x%02X\n", received_byte);

        ParserResult result =
            parser_process_byte(
                parser,
                received_byte,
                frame
            );

        if (result == FRAME_VALID)
        {
            printf("\nFRAME VALID!\n");
            printf("Payload length: %d\n", frame->length);

            printf("Payload: ");

            for (int j = 0; j < frame->length; j++)
            {
                printf("0x%02X ", frame->payload[j]);
            }

            printf("\n\n");
        }
        else if (result == FRAME_INVALID)
        {
            printf("\nFRAME INVALID!\n\n");
        }
    }
}

int main(void)
{
    FrameParser parser;
    ParsedFrame frame;

    parser_init(&parser);

    printf("=====================================\n");
    printf("      UART FRAME PARSER SIMULATION\n");
    printf("=====================================\n\n");


    /*
     * Simulated continuous UART stream.
     *
     * 55 12
     *       -> Garbage bytes
     *
     * AA 03 01 02 03 72
     *       -> Valid frame
     *
     * AA 02 10 20 00
     *       -> Invalid CRC
     *
     * AA 02 05 06 85
     *       -> Valid frame after error
     */

    uint8_t uart_stream[] =
    {
        /* Garbage */
        0x55,
        0x12,

        /* Valid frame */
        0xAA,
        0x03,
        0x01,
        0x02,
        0x03,
        0x72,

        /* Invalid frame */
        0xAA,
        0x02,
        0x10,
        0x20,
        0x00,

        /* Valid frame */
        0xAA,
        0x02,
        0x05,
        0x06,
        0x85
    };

    int stream_length = sizeof(uart_stream);

    printf("Receiving UART stream...\n\n");

    process_uart_stream(
        &parser,
        uart_stream,
        stream_length,
        &frame
    );


    /*
     * Simulate an incomplete frame.
     *
     * AA 03 01
     *
     * The remaining bytes never arrive.
     */
    printf("=====================================\n");
    printf("       INCOMPLETE FRAME TEST\n");
    printf("=====================================\n\n");

    uint8_t incomplete_frame[] =
    {
        0xAA,
        0x03,
        0x01
    };

    process_uart_stream(
        &parser,
        incomplete_frame,
        sizeof(incomplete_frame),
        &frame
    );

    printf("Frame incomplete.\n");

    /*
     * In a real embedded system, a UART timeout
     * would reset the parser.
     *
     * Here we simulate that timeout by calling
     * parser_init().
     */
    printf("UART timeout -> resetting parser.\n\n");

    parser_init(&parser);


    /*
     * New frame after timeout.
     */
    uint8_t frame_after_timeout[] =
    {
        0xAA,
        0x02,
        0x05,
        0x06,
        0x85
    };

    printf("Receiving new frame after timeout...\n\n");

    process_uart_stream(
        &parser,
        frame_after_timeout,
        sizeof(frame_after_timeout),
        &frame
    );

    printf("UART simulation complete.\n");

    return 0;
}