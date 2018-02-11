#pragma once

#ifndef flashconfig_h
#define flashconfig_h

#include <ESP.h>
#include <EEPROM.h>

#define _CONF_ADDR_CONF_BYTE 0x0001
#define _CONF_ADDR_API_KEY 0x0002
#define _CONF_ADDR_SAMPLE_RATE 0x0012
#define _CONF_ADDR_SAMPLE_DUR 0x0013
#define _CONF_EEPROM_SIZE 256

class FlashConfig
{
public:
	FlashConfig();
	~FlashConfig();

	bool hasApiKey = false;
	String ApiKey;
	bool hasSampling = false;
	int SampleRate() { return 0x00000001 << sampleRateByte; }				// Hz
	int SampleDuration() { return 0x00000001 << sampleDurationByte; }		// ms
	void WriteDefault(String apikey);

private:
	byte configbyte = 0;
	byte maskApiKey =		0b00000001;
	byte maskSampling =		0b00000010;
	bool getBoolValue(byte mask) { return (configbyte & mask) == mask; }
	void setBoolValue(byte mask, bool value) { configbyte = value ? configbyte | mask : configbyte & !mask; }
	void readAddress(byte* value, int start);
	void writeAddress(byte* value, int start);
	byte sampleRateByte = 0;
	byte sampleDurationByte = 0;
};
#endif
