#include "tfa433.h"

const int TFA433::_BUFF_SIZE = 6;
const word TFA433::_DELAY = 250;
const byte TFA433::_HEADER_BITS = 12;
const byte TFA433::_DISCARD_BITS = 2;
const byte TFA433::_POLARITY = 1;

volatile byte TFA433::_buff[TFA433::_BUFF_SIZE];
volatile byte TFA433::_tempBit;
volatile boolean TFA433::_firstZero;
volatile byte TFA433::_headerHits;
volatile byte TFA433::_discardHits;
volatile byte TFA433::_dataByte;
volatile byte TFA433::_nosBits;
volatile byte TFA433::_nosBytes;

tfaResult TFA433::_result;
bool TFA433::_avail;
unsigned long TFA433::_lastResultTime;

byte TFA433::_pin = 0;

TFA433::TFA433() {
}

void TFA433::start(int pin) {
	_pin = pin;
	pinMode(_pin, INPUT);

	_avail = false;

	for (int i = 0; i < _BUFF_SIZE; i++) {
		_buff[i] = 0;
	}

	_init();

	attachInterrupt(digitalPinToInterrupt(_pin), _handler, CHANGE);
}

void TFA433::stop() {
	detachInterrupt(digitalPinToInterrupt(_pin));
}

bool TFA433::isDataAvailable() {
	return _avail;
}

void TFA433::getData(byte &type, byte &id, bool &battery, byte &channel, float &temperature, byte &humidity) {
	if (!_avail) {
		return;
	}

	type = _result.type;
	id = _result.id;
	battery = _result.battery;
	channel = _result.channel;
	temperature = _result.temperature;
	humidity = _result.humidity;

	_avail = false;
}

tfaResult TFA433::getData() {
	_avail = false;

	return _result;
}

bool operator==(const tfaResult& r1, const tfaResult& r2)
{
	return r1.type == r2.type &&
	r1.id == r2.id &&
	r1.battery == r2.battery &&
	r1.channel == r2.channel &&
	r1.temperature == r2.temperature &&
	r1.humidity == r2.humidity;
}

void TFA433::_init() {
	_tempBit = _POLARITY^1;
	_firstZero = false;
	_headerHits = 0;
	_discardHits = 0;
	_dataByte = 0;
	_nosBits = 0;
	_nosBytes = 0;
}

// Inspired by: https://github.com/robwlakes/ArduinoWeatherOS/blob/master/DebugManchester.ino
void TFA433::_handler() {
	if (_nosBytes >= _BUFF_SIZE) {
		byte expected = _buff[_BUFF_SIZE-1];
		byte calculated = _lfsr_digest8(_buff, _BUFF_SIZE-1, 0x98, 0x3e) ^ 0x64;

		if (expected == calculated) {
			tfaResult result = _parseResult();
			if (!_isRepeat(result)) {
				_result = result;
				_avail = true;
				_lastResultTime = millis();
			}
		}

		_init();
	}

	if(digitalRead(_pin) != _tempBit) {
		return;
	}

	delayMicroseconds(_DELAY);

	if (digitalRead(_pin) != _tempBit) {
		// Error, start over
		_init();
		return;
	}

	byte bitState = _tempBit ^ _POLARITY;

	delayMicroseconds(2*_DELAY);

	if(digitalRead(_pin) == _tempBit) {
		_tempBit = _tempBit^1;
	}

	if(bitState == 1) {
		if(!_firstZero) {
			_headerHits++;
			return;
		}

		_add(bitState);
		return;
	}

	if(_headerHits < _HEADER_BITS) {
		// Error, start over
		_init();
		return;
	}

	if (!_firstZero && _headerHits >= _HEADER_BITS) {
		_firstZero = true;
	}

	_add(bitState);
}

void TFA433::_add(byte bitData) {
	if (_discardHits < _DISCARD_BITS) {
		_discardHits++;
		return;
	}

	_dataByte = (_dataByte << 1) | bitData;
	_nosBits++;

	if (_nosBits == 8) {
		_nosBits = 0;
		_buff[_nosBytes] = _dataByte;
		_nosBytes++;
	}
}

// From: https://github.com/merbanan/rtl_433/blob/master/src/util.c
byte TFA433::_lfsr_digest8(byte const message[], unsigned n, byte gen, byte key) {
	byte sum = 0;

	for (unsigned k = 0; k < n; ++k) {
		byte data = message[k];

		for (int i = 7; i >= 0; --i) {
			// XOR key into sum if data bit is set
			if ((data >> i) & 1) {
				sum ^= key;
			}

			// roll the key right (actually the lsb is dropped here)
			// and apply the gen (needs to include the dropped lsb as msb)
			if (key & 1) {
				key = (key >> 1) ^ gen;
			}
			else {
				key = (key >> 1);
			}
		}
	}

	return sum;
}

tfaResult TFA433::_parseResult() {
	tfaResult result;

	result.type = _binToDec(0, 7);
	result.id = _binToDec(8, 15);
	result.battery = _binToDec(16, 16) != 1;
	result.channel = _binToDec(17, 19) + 1;
	result.temperature = _binToDec(20, 31) / 10.0 - 40.0;
	result.humidity = _binToDec(32, 39);

	return result;
}

int TFA433::_binToDec(int s, int e) {
	int result = 0;
	unsigned int mask = 1;

	for(; e > 0 && s <= e; mask <<= 1) {
		if (_getBit(e--)) {
			result |= mask;
		}
	}

	return result;
}

bool TFA433::_getBit(int k) {
	int i = k / 8;
	int pos = k % 8;

	byte mask = B10000000;

	mask >>= pos;

	return (_buff[i] & mask) != 0;
}

bool TFA433::_isRepeat(tfaResult result) {
	return result == _result && millis() - _lastResultTime < 3000;
}
