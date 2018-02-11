#include "FlashConfig.h"

FlashConfig::FlashConfig()
{
	EEPROM.begin(_CONF_EEPROM_SIZE);
	configbyte = EEPROM.read(_CONF_ADDR_CONF_BYTE);

	ApiKey = "";

	hasApiKey = getBoolValue(maskApiKey);
	if (hasApiKey)
	{
		for (size_t i = 0; i < 16; i++)
		{
			ApiKey.concat((char)EEPROM.read(_CONF_ADDR_API_KEY + i));
		}
	}

	hasSampling = getBoolValue(maskSampling);
	if (hasSampling)
	{
		sampleRateByte = EEPROM.read(_CONF_ADDR_SAMPLE_RATE);
		sampleDurationByte = EEPROM.read(_CONF_ADDR_SAMPLE_DUR);
	}

	EEPROM.end();
}

FlashConfig::~FlashConfig()
{}


void FlashConfig::WriteDefault(String apikey)
{
	EEPROM.begin(_CONF_EEPROM_SIZE);

	for (size_t i = 0; i < 16; i++)
	{
		EEPROM.write(_CONF_ADDR_API_KEY + i, apikey[i]);
	}

	EEPROM.write(_CONF_ADDR_SAMPLE_DUR, 12);
	EEPROM.write(_CONF_ADDR_SAMPLE_RATE, 2);

	this->setBoolValue(maskApiKey, true);
	this->setBoolValue(maskSampling, true);
	EEPROM.write(_CONF_ADDR_CONF_BYTE, configbyte);

	EEPROM.commit();
	EEPROM.end();
}

void FlashConfig::readAddress(byte* value, int start)
{
	for (int i = 0; i < sizeof(value); i++)
	{
		value[i] = EEPROM.read(start + i);
	}
}

void FlashConfig::writeAddress(byte* value, int start)
{
	Serial.println(sizeof(value));

	for (int i = 0; i < sizeof(value); i++)
	{
		EEPROM.write(start + i, value[i]);
	}
}
