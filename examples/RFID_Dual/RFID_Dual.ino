/**
 * RFID Library Dual example.
 * 
 * It demostrates how to use two readers at the same time and how to
 * initialize instances manually (instead if the default one).
 * 
 * Written by Mark Reds
 */

#define RFID_CUSTOM		// prevents creations of default instance
#include <RFID.h>

#define	ENTRANCE_CLOCK_PIN		13
#define ENTRANCE_DATA_PIN		12
#define	EXIT_CLOCK_PIN			8
#define EXIT_DATA_PIN			7

RFIDClass entranceRFID(ENTRANCE_CLOCK_PIN, ENTRANCE_DATA_PIN);
RFIDClass exitRFID(EXIT_CLOCK_PIN, EXIT_DATA_PIN);

void onEntrance(const String &tag) {
	Serial.print(tag);
	Serial.print(" entrance request.");
}

void onExit(const String &tag) {
	Serial.print(tag);
	Serial.print(" exit request.");
}

void setup() {
	Serial.begin(9600);
	entranceRFID.begin();
	entranceRFID.attach(onEntrance);
	exitRFID.begin();
	exitRFID.attach(onExit);
}

void loop() {
	entranceRFID.loop();
	exitRFID.loop();
}
