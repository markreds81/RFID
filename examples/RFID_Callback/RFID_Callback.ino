/**
 * RFID Library Callback example.
 * 
 * It demostrates how to use RFID library with callback.
 * 
 * Written by Mark Reds
 */

#include <RFID.h>

#define	CLOCK_PIN		13
#define DATA_PIN		12

void onTagRead(const String &tag) {
	Serial.println(tag);
}

void setup() {
	Serial.begin(9600);
	RFID.begin(CLOCK_PIN, DATA_PIN);
	RFID.attach(onTagRead);
}

void loop() {
	RFID.loop();
}
