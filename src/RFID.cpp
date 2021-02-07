/************************************************************************************
 * 
 * Name    : mrRFID
 * File    : RFID.cpp
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

#include "RFID.h"

#define RFID_MLX_DATA_BUFSIZE	256
#define RFID_MLX_DATA_MINSIZE	64
#define RFID_MLX_TIMEOUT		2UL

#define GPIO_PIN_SET			HIGH
#define GPIO_PIN_RESET			LOW

RFIDClass::RFIDClass() {
	
}

RFIDClass::RFIDClass(const uint8_t clockPin, const uint8_t dataPin) :
	m_clockPin(clockPin),
	m_dataPin(dataPin)
{
	
}

RFIDClass::~RFIDClass() {
	
}

void RFIDClass::begin() {
	pinMode(m_clockPin, INPUT_PULLUP);
	pinMode(m_dataPin, INPUT_PULLUP);
}

void RFIDClass::begin(const uint8_t clockPin, const uint8_t dataPin) {
	m_clockPin = clockPin;
	m_dataPin = dataPin;
	pinMode(m_clockPin, INPUT_PULLUP);
	pinMode(m_dataPin, INPUT_PULLUP);
}

void RFIDClass::loop() {
	m_status = read();
}

bool RFIDClass::wait() {
	uint32_t start = millis();
	while (digitalRead(m_clockPin) != GPIO_PIN_RESET) {
		if ((millis() - start) > RFID_MLX_TIMEOUT) {
			return false;	// time-out expired
		}
	}
	return true;
}

RFIDStatus RFIDClass::read() {
	uint8_t data[RFID_MLX_DATA_BUFSIZE], nibbles[RFID_UID_LEN * 2];

	// check if MLX is reading a tag
	// if CLOCK line is not LOW the exit
	if (digitalRead(m_clockPin) != GPIO_PIN_RESET) {
		return RFID_NO_UID;							// not active transmission
	}

	// syncing with CLOCK signal and reading DATA signal
	// save values in a temprary buffer.
	memset(data, 0, sizeof(data));
	uint32_t start = millis();
	size_t i = 0;
	while ((millis() - start) < RFID_MLX_TIMEOUT && i < sizeof(data)) {
		if (digitalRead(m_clockPin) == GPIO_PIN_SET) {
			data[i++] = digitalRead(m_dataPin);
			if (!wait()) {
				break;
			}
			start = millis();
		}
	}

	// se non ho abbastanza dati esco subito
	// check how many data collected. If not enough then exit
	if (i < RFID_MLX_DATA_MINSIZE) {
		return RFID_MLX_ERR_TIMEOUT;
	}

	// scan buffer and search for header (9 consecutive bits with 1)
	i = 0;
	uint8_t count = 9;
	while (i < sizeof(data) && count > 0) {
		if (data[i++] == GPIO_PIN_SET) {
			count--;
		} else {
			count = 9;
		}
	}

	// if not header found then exit
	if (count) {
		return RFID_NO_UID;
	}

	// analizing data after header and calculate crc
	count = 5;
	uint8_t read = 0;
	int parity = 0, j = 10;
	memset(nibbles, 0, sizeof(nibbles));
	while (i < sizeof(data)) {
		// get nibble + parity bit
		// if 0 no bit
		// if 1 set bit 1
		if (data[i++] == GPIO_PIN_SET) {
			read |= 0x10;							// set bit #4 to 1
			parity++;								// inc parity check
		}
		if (--count == 0) {							// if 0 got 5 bits
			if (j == 0) {							// got complete nibble, now check vertical parity and stop bit
				if (read & 0x10) {
					return RFID_MLX_ERR_BITSTOP;	// stop bit error
				}
				read &= 0x0F;
				for (j = 0; j < 10; j++) {
					read = read ^ nibbles[j];
				}
				if (read) {
					return RFID_MLX_ERR_VPARITY;	// vertical parity check failed
				}
				j = 0;
				memset(m_uid, 0, sizeof(m_uid));
				for (uint8_t k = 0; k < sizeof(nibbles); k += 2) {
					m_uid[j++] = ((nibbles[k + 1] << 4) & 0xF0) | nibbles[k];
				}
				if (m_callback != NULL) {
					char tag[sizeof(nibbles)+1];
					for (uint8_t k = 0; k < sizeof(nibbles); k++) {
						tag[k] = "0123456789ABCDEF"[nibbles[k]];
					}
					tag[sizeof(tag)] = '\0';
					m_callback(String(tag));
				}
				return RFID_UID_OK;					// got UID
			}
			// check horizontal bit
			if (parity & 0x01) {
				return RFID_MLX_ERR_HPARITY;		// 1 bit counter is odd, check failed
			}
			// save nibble into final buffer without parity bit
			nibbles[--j] = read & 0x0F;
			// preparing for the next nibble
			count = 5;
			read = 0;
			parity = 0;
		} else {
			read >>= 1;		// dx shifting
		}
	}

	// no valid data found in temporary buffer, exiting
	return RFID_NO_UID;
}

void RFIDClass::tag(char *dst, const size_t len) {
	uint8_t i = 0, j = 0;
	while (i < sizeof(m_uid) && j < len) {
		dst[j++] = "0123456789ABCDEF"[m_uid[i] & 0x0F];
		dst[j++] = "0123456789ABCDEF"[m_uid[i] >> 4];
		i++;
	}
	dst[j] = '\0';
}

#ifndef RFID_CUSTOM
RFIDClass RFID;
#endif