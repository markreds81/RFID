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
 * along with RFID. If not, see <http://www.gnu.org/licenses/>.
 * 
 ************************************************************************************/

#include "RFID.h"

#define RFID_MLX_DATA_BUFSIZE	256
#define RFID_MLX_DATA_MINSIZE	64
#define RFID_MLX_TIMEOUT		2UL

#define GPIO_PIN_SET			HIGH
#define GPIO_PIN_RESET			LOW

RFID::RFID(
	const uint8_t clockPin,
	const uint8_t dataPin) :
	m_clockPin(clockPin),
	m_dataPin(dataPin)
{
	pinMode(m_clockPin, INPUT_PULLUP);
	pinMode(m_dataPin, INPUT_PULLUP);
}

RFID::~RFID() {

}

void RFID::setCallback(void (*callback)(const String &tag)) {
	m_callback = callback;
}

bool RFID::wait() {
	uint32_t start = millis();
	while (digitalRead(m_clockPin) != GPIO_PIN_RESET) {
		if ((millis() - start) > RFID_MLX_TIMEOUT) {
			return false;	// time-out expired
		}
	}
	return true;
}

int RFID::read() {
	uint8_t data[RFID_MLX_DATA_BUFSIZE], nibbles[RFID_UID_LEN * 2];

	// verifico se la trasmissione è attiva
	// se il clock non è basso esco subito
	if (digitalRead(m_clockPin) != GPIO_PIN_RESET) {
		return RFID_NO_UID;							// trasmissione non attiva
	}

	// mi sincronizzo con il clock e leggo la linea data
	// carico i valori letti in un buffer temporaneo sufficientemente grande
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
	if (i < RFID_MLX_DATA_MINSIZE) {
		return RFID_MLX_ERR_TIMEOUT;
	}

	// ricerco l'header del paccchetto dati appena letto
	i = 0;
	uint8_t count = 9;
	while (i < sizeof(data) && count > 0) {
		if (data[i++] == GPIO_PIN_SET) {
			count--;
		} else {
			count = 9;
		}
	}

	// se non ho trovato l'header esco subito
	if (count) {
		return RFID_NO_UID;
	}

	// analizzo i dati che seguono l'header e controllo la parità
	count = 5;
	uint8_t read = 0;
	int parity = 0, j = 10;
	memset(nibbles, 0, sizeof(nibbles));
	while (i < sizeof(data)) {
		// acquisisco nibble più bit parità
		// se leggo 0 non modifico bit
		// se leggo 1 metto bit a 1
		if (data[i++] == GPIO_PIN_SET) {
			read |= 0x10;							// metto bit #4 a 1
			parity++;								// incremento controllo parità
		}
		if (--count == 0) {							// se 0 ho acquisito 5 bit
			if (j == 0) {							// ho acquisito nibble parità verticale e stop bit
				if (read & 0x10) {
					return RFID_MLX_ERR_BITSTOP;	// stop bit errato
				}
				read &= 0x0F;
				for (j = 0; j < 10; j++) {
					read = read ^ nibbles[j];
				}
				if (read) {
					return RFID_MLX_ERR_VPARITY;	// controllo parità verticale fallito
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
			// verifico parità pari orizzontale nibble codice ricevuto
			if (parity & 0x01) {
				return RFID_MLX_ERR_HPARITY;		// numero di uno letti dispari ricezione errata
			}
			// salvo nibble acquisito senza bit parità
			nibbles[--j] = read & 0x0F;
			// mi preparo per il prossimo nibble codice
			count = 5;
			read = 0;
			parity = 0;
		} else {
			read >>= 1;		// shift a dx
		}
	}

	// ho scorso tutto il pacchetto senza trovare dati validi
	return RFID_NO_UID;
}

void RFID::uid(uint8_t *dst, const size_t len) {
	memcpy(dst, m_uid, len < sizeof(m_uid) ? len : sizeof(m_uid));
}

void RFID::tag(char *dst, const size_t len) {
	uint8_t i = 0, j = 0;
	while (i < sizeof(m_uid) && j < len) {
		dst[j++] = "0123456789ABCDEF"[m_uid[i] & 0x0F];
		dst[j++] = "0123456789ABCDEF"[m_uid[i] >> 4];
		i++;
	}
	dst[j] = '\0';
}

uint8_t RFID::uid8(const uint8_t i) {
	return (i < RFID_UID_LEN) ? m_uid[i] : 0;
}