# Serial Frame Parser

A small C library that reads a stream of bytes, detects complete serial
frames, and validates each frame using a CRC-8 checksum.

## Frame Format

Each frame has the following structure:

| Field | Size | Description |
|---|---:|---|
| START | 1 byte | `0xAA` |
| LENGTH | 1 byte | Number of payload bytes |
| PAYLOAD | 0–64 bytes | Frame data |
| CRC | 1 byte | CRC-8 checksum |

Frames with a LENGTH greater than 64 are discarded.

### Example

```text
AA 03 01 02 03 72