/*
433 MHz weather station receiver library for TFA Dostmann 30.3208.02.
Created by d10i on the 11th of February 2023.
Inspired by: https://github.com/denxhun/TFA433.
With contributions taken from:
 - https://github.com/robwlakes/ArduinoWeatherOS
 - https://github.com/merbanan/rtl_433

Released into the public domain.
*/
#ifndef tfa433dostmann30320802_h
#define tfa433dostmann30320802_h

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

typedef struct{
	byte type;
	byte id;
	bool battery;
	byte channel;
	float temperature; // In Fahrenheit
	byte humidity;
} tfaResult;

class TFA433{
	public:
		TFA433();
		void start(int pin);
		void stop();
		bool isDataAvailable();
		void getData(byte &type, byte &id, bool &battery, byte &channel, float &temperature, byte &humidity);
		tfaResult getData();

	private:
		const static int _BUFF_SIZE;
		const static word _DELAY;
		const static byte _HEADER_BITS;
		const static byte _DISCARD_BITS;
		const static byte _POLARITY;

		volatile static byte _buff[];
		volatile static byte _tempBit;
		volatile static boolean _firstZero;
		volatile static byte _headerHits;
		volatile static byte _discardHits;
		volatile static byte _dataByte;
		volatile static byte _nosBits;
		volatile static byte _nosBytes;

		static tfaResult _result;
		static bool _avail;
		static unsigned long _lastResultTime;

		static byte _pin;

		static void _init();
		static void _handler();
		static void _add(byte bitData);
		static byte _lfsr_digest8(byte const message[], unsigned n, byte gen, byte key);
		static tfaResult _parseResult();
		static int _binToDec(int s, int e);
		static bool _getBit(int k);
		static bool _isRepeat(tfaResult result);
};

#endif
