/************************************************************************************
 * 
 * Name    : mrRFID
 * File    : RFID.h
 * Author  : Mark Reds <marco@markreds.it>
 * Date    : September 10, 2020
 * Version : 1.0.0
 * Notes   : Arduino library with support for Melexis MLX90109 RFID tag reader ICs.
 * 
 * Copyright (C) 2020 Marco Rossi (aka Mark Reds).  All right reserved.
 * 
 * This file is part of mrRFID.
 * 
 * mrRFID is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * mrRFID is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with mrRFID. If not, see <http://www.gnu.org/licenses/>.
 * 
 ************************************************************************************/

#ifndef _RFID_h_
#define _RFID_h_

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include "pins_arduino.h"
#endif

#ifndef RFID_UID_LEN
#define RFID_UID_LEN			5
#endif

enum RFIDStatus {
	RFID_UID_OK = 1,
	RFID_NO_UID = 0,
	RFID_MLX_ERR_TIMEOUT = -1,
	RFID_MLX_ERR_HPARITY = -2,
	RFID_MLX_ERR_VPARITY = -3,
	RFID_MLX_ERR_BITSTOP = -4
};

typedef void (*OnTagRead)(const String &tag);

class RFIDClass {
public:
	RFIDClass();
	RFIDClass(const uint8_t clockPin, const uint8_t dataPin);
	virtual ~RFIDClass();
	void begin();
	void begin(const uint8_t clockPin, const uint8_t dataPin);
	void loop();
	void tag(char *dst, const size_t len);
	inline void attach(OnTagRead callback) {
		m_callback = callback;
	}
	inline void uid(uint8_t *dst, const size_t len) {
		memcpy(dst, m_uid, len < sizeof(m_uid) ? len : sizeof(m_uid));
	}
	inline uint8_t uid8(const uint8_t i) {
		return (i < RFID_UID_LEN) ? m_uid[i] : 0;
	}
	inline RFIDStatus status() {
		return m_status;
	}
private:
	uint8_t m_clockPin;
	uint8_t m_dataPin;
	uint8_t m_uid[RFID_UID_LEN];
	RFIDStatus m_status;

	OnTagRead m_callback;
	bool wait();
	RFIDStatus read();
};

#ifndef RFID_CUSTOM
extern RFIDClass RFID;
#endif

#endif
