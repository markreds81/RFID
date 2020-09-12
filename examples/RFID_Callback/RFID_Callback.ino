#include <RFID.h>

#define	CLOCK_PIN		13
#define DATA_PIN		12

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
