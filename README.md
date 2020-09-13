# RFID

This is another RFID library on Arduino with support for 125 KHz tag readers based on Melexis MLX90109 ICs.

![alt text](extras/EVB90109_V3_arduino.jpg "EVB90109_V3 with Arduino board")

## Example

```c++
#include <RFID.h>

#define CLOCK_PIN    13
#define DATA_PIN     12

RFID rfid(CLOCK_PIN, DATA_PIN);

void onTagRead(const String &tag) {
    Serial.println(tag);
}

void setup() {
    Serial.begin(115200);
    rfid.setCallback(onTagRead);
}

void loop() {
    rfid.read();
}
```

See "RFID_Callback.ino" for the complete example.

## Documentation

MLX90109 requires two pin on the MCU side configured as digital inputs: one for CLOCK signal and the other one for DATA signal. All the required initializations are done by the library.

### Definitions

`RFID_UID_LEN` is the binary length of a tag (5 bytes).

### Construction

```c++
RFID (
    uint8_t clockPin,
    uint8_t dataPin
);
```

### Functions

```c++
int read();
```
Must be called in loop() function.

Returns:
* a value < 0 if an error has occurred. It could be one of the following:
    * `RFID_MLX_ERR_TIMEOUT` when a timeout error is occurred.
    * `RFID_MLX_ERR_HPARITY` if the horizonatal parity check fails.
    * `RFID_MLX_ERR_VPARITY` if the vertical parity check fails.
    * `RFID_MLX_ERR_BITSTOP` if not bit stop found.
* 0 if no tag deteted
* a value > 0 if a tag is read

```c++
void setCallback(void (*callback)(const String &tag));
```
Set a callback function called when a tag is read. The `tag` parameter contains the UID in HEX format.

```c++
void uid(uint8_t *dst, const size_t len);
```
Put the last UID tag read (binary raw values). `dst` is a byte array pointer of `len` size to fill with UID data.
It must be length at least `RFID_UID_LEN` (default to 5).

```c++
void tag(char *dst, const size_t len);
```
Put the last UID tag read in HEX string format. `dst` is a char array pointer of `len` size to fill with UID data.
It must be length at least double `RFID_UID_LEN` plus 1 for string terminator.

```c++
uint8_t uid8(const uint8_t i);
```
Return the `i`st element of the last UID tag read.

For any further reading about MLX90109 see details at https://www.melexis.com/en/product/MLX90109/125kHz-RFID-Transceiver.
For my tests I used the evaluation board EVB90109_V3 https://www.melexis.com/en/product/evb90109/evaluation-board-mlx90109.