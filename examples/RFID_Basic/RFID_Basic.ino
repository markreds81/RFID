/**
 * RFID Library Basic example.
 * 
 * It demostrates the basic usage of RFID library.
 * 
 * Written by Mark Reds
 */

#include <RFID.h>

#define	CLOCK_PIN		13
#define DATA_PIN		12

void setup() {
	Serial.begin(9600);
	RFID.begin(CLOCK_PIN, DATA_PIN);
}

void loop() {
	RFID.loop();
	
	if (RFID.status() == RFID_UID_OK) {
		char key[RFID_UID_LEN * 2 + 1];
		RFID.tag(key, sizeof(key));
		Serial.println(key);
	}
}
