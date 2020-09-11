/************************************************************************************
 * 
 * Name    : RFID.h
 * Author  : Mark Reds <marco@markreds.it>
 * Date    : September 10, 2020
 * Version : 0.1
 * Notes   : RFID library for Arduino with support for Melexis MLX90109 tag reader ICs.
 * 
 * Copyright (c) 2020 Mark Reds.  All right reserved.
 * 
 * 
 * RFID is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * RFID is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with RFID.  If not, see <http://www.gnu.org/licenses/>.
 * 
 ************************************************************************************/

#ifndef RFID_h
#define RFID_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include "pins_arduino.h"
#endif

#define RFID_UID_LEN			5

// generic result codes
#define RFID_NO_UID				0
#define RFID_UID_OK				1

// specific result codes
#define RFID_MLX_ERR_TIMEOUT	-1
#define RFID_MLX_ERR_HPARITY	-2
#define RFID_MLX_ERR_VPARITY	-3
#define RFID_MLX_ERR_BITSTOP	-4

class RFID {
	private:
		uint8_t m_clockPin;
		uint8_t m_dataPin;
		uint8_t m_uid[RFID_UID_LEN];
		void (*m_callback)(const String &tag);
		bool wait();
	public:
		RFID(const uint8_t clockPin, const uint8_t dataPin);
		virtual ~RFID();
		void setCallback(void (*callback)(const String &tag));
		int read();
		void uid(uint8_t *dst, const size_t len);
		void tag(char *dst, const size_t len);
		uint8_t uid8(const uint8_t i);
};

#endif
