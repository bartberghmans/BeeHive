ADC_MODE(ADC_TOUT_3V3);

/*
 Name:		BeeHive.ino
 Created:	6/3/2017 8:23:51 PM
 Author:	bberghmans
*/
//#include <HX711.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <Esp.h>

#include "FlashConfig.h"
#include "dht11.h"
#include "TempArray.h"
#include "DataHub.h"

#define SAMPLE_RATE 2 // kHz
#define SAMPLE_DURATION 4*1024 // ms
#define MEASUREMENT_INTERVAL 5*60*1000000
#define MIC_PIN A0
#define SCALE_DATA_PIN D7
#define SCALE_SCK_PIN D6
#define DTS_PIN D2
#define DHT_TYPE DHT11
#define DHT_PIN D5

extern "C" {
#include "user_interface.h"
}

FlashConfig config;
OneWire ds(DTS_PIN);
DataHub datahub;
TempArray temperature(&ds);
//HX711 scale(SCALE_DATA_PIN, SCALE_SCK_PIN, 32);
dht11 dht;


byte addr[4][8] = {
	{ 0x28, 0xff, 0x82, 0xae, 0x31, 0x17, 0x03, 0xa9 },
	{ 0x28, 0xff, 0x4d, 0xb1, 0x31, 0x17, 0x03, 0x46 },
	{ 0x28, 0xff, 0x2d, 0x70, 0x31, 0x17, 0x04, 0x62 },
	{ 0x28, 0xff, 0x8b, 0xc2, 0x31, 0x17, 0x04, 0x5b }
};

/*
byte addr[4][8] = {
	{ 0x28, 0xff, 0x76, 0xe7, 0x02, 0x17, 0x03, 0x59 },
	{ 0x28, 0xff, 0x80, 0xe2, 0x31, 0x17, 0x04, 0x5b },
	{ 0x28, 0xff, 0xbd, 0xfe, 0x31, 0x17, 0x03, 0x9a },
	{ 0x28, 0xff, 0x2b, 0xe0, 0x31, 0x17, 0x04, 0x1f }
};
*/

byte *addrRows[4] = { addr[0],addr[1],addr[2],addr[3] };
byte *macAddr[6] = { 0, 0, 0, 0, 0, 0 };

bool tickOccured;
bool sampling;
float temperatures[4] = { 0.0, 0.0, 0.0, 0.0 };
int current_sample;
uint16 i;
unsigned long time_start;

//char hexmap[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

void setup() {
	time_start = micros();
	WiFi.mode(WIFI_STA);

	//config.WriteDefault("you API key");

	uint32_t numberSamples = config.SampleDuration() * config.SampleRate();
	unsigned long timeinterval = 1000 / config.SampleRate();			// 1000 / SR[kHz] = TI [us]
	unsigned long nextmicros = micros();
	int normalization = 0;

	for (uint32_t i = 0; i < config.SampleRate() * 100; i++)		// take a 100ms sample
	{
		while (micros() < nextmicros) yield();
		normalization += analogRead(MIC_PIN);
		nextmicros += timeinterval;
	}

	normalization /= config.SampleRate() * 100;
	normalization -= 128;

	datahub.init(numberSamples);
	datahub.clear();

	numberSamples = config.SampleDuration() * config.SampleRate();
	nextmicros = micros();

	for (uint32_t i = 0; i < numberSamples; i++)
	{
		while (micros() < nextmicros) yield(); 
		datahub.addbody((char)(analogRead(MIC_PIN)-normalization));
		nextmicros += timeinterval;
	}

	datahub.post("application/octet-stream", config.ApiKey);
	datahub.clear(4096);

	temperature.init(addrRows, temperatures, (uint8)4);

	// start JSON 
	datahub.addbody('{');

	/*
	datahub.addbody("\"weight\": ");
	datahub.addbody(scale.read());
	datahub.addbody(',');
	*/

	temperature.readtemps();
	datahub.addbody(temperature.JSON());
	datahub.addbody(',');

	dht.read(DHT_PIN);
	datahub.addbody("\"ambient\": { \"humidity\": ");
	datahub.addbody(dht.humidity);
	datahub.addbody(',');
	datahub.addbody("\"temperature\": ");
	datahub.addbody(dht.temperature);
	datahub.addbody("}");

	datahub.addbody('}');
	datahub.post("application/json", config.ApiKey);

	//	scale.power_down();
	//	temperature.sleep();
	ESP.deepSleep(MEASUREMENT_INTERVAL - (micros() - time_start));
}

void loop() {
	yield();
	delay(500);
}