# Serial Frame Parser in C

A robust serial frame parser implemented in C using a state machine and CRC-8 error detection.

The project is designed for embedded-system communication and can be integrated with a UART peripheral on a microcontroller. The parser is currently developed and tested on a PC using GCC, with UART behavior simulated in software.

---

## 1. Frame Format

Each serial frame follows this format:

| Field | Size | Description |
|---|---:|---|
| START | 1 byte | `0xAA` |
| LENGTH | 1 byte | Payload length |
| PAYLOAD | 0–64 bytes | Actual data |
| CRC | 1 byte | CRC-8 |

Example:

```text
AA 03 01 02 03 72