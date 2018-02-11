#pragma once
#include <Esp.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define BUFFER_SIZE 8*1024
#define BATCH_SIZE 2048

class DataHub
{
	// Wi-Fi Settings
	const char* ssid = "SSID"; // your wireless network name (SSID)
	const char* password = "pwd"; // your Wi-Fi network password
	const char* server = "http://www.your.domain";
	uint8_t* buffer;
	String measurement_id;
	bool has_measurement_id;
	uint32 counter;
	uint32 offset;
	uint32 postsize;
	WiFiClient client;
	HTTPClient http;

public:
	DataHub();
	~DataHub();
	void addbody(String text);
	void addbody(char c);
	void addbody(unsigned long value);
	void addbody(unsigned int value) { this->addbody((unsigned long)value); }
	void addbody(long value);
	void addbody(int value) { this->addbody((signed long)value); }
	void clear() { this->counter = 0; }
	void clear(uint32 buffersize);
	void init(uint32 buffersize);
	void setbuffer(uint32 buffersize);
	int post(String contenttype, String apikey);
};

