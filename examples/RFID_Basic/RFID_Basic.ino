#include <RFID.h>

#define	CLOCK_PIN		13
#define DATA_PIN		12

RFID rfid(CLOCK_PIN, DATA_PIN);

void setup() {
	Serial.begin(115200);
}

void loop() {
	if (rfid.read() == RFID_UID_OK) {
		char key[RFID_UID_LEN * 2 + 1];
		rfid.tag(key, sizeof(key));
		Serial.println(key);
	}
}
