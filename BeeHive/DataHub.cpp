#include "DataHub.h"

void DataHub::init(uint32 buffersize)
{
	this->setbuffer(buffersize);
	WiFi.begin(ssid, password);
	int connectcount = 10;

	while (WiFi.status() != WL_CONNECTED)  {
		delay(500);
	}

	this->has_measurement_id = false;
	this->counter = 0;
}

void DataHub::setbuffer(uint32 buffersize)
{
	delete[] buffer;
	buffer = new uint8[buffersize];
}

DataHub::DataHub()
{}

void DataHub::clear(uint32 buffersize)
{
	this->setbuffer(buffersize);
	this->clear();
}

void DataHub::addbody(char c)
{
	this->buffer[counter] = c;
	this->counter++;
}

void DataHub::addbody(String text)
{
	for (uint16 i = 0; i < text.length(); i++)
	{
		this->addbody(text[i]);
	}
}

void DataHub::addbody(unsigned long value)
{
	unsigned long remainder = value;
	uint8 tempcount = 0;
	char tempstring[5];

	while (remainder > 9)
	{
		tempstring[tempcount] = (remainder % 10) + 48;
		remainder = (unsigned long)remainder / 10;
		tempcount++;
	}

	tempstring[tempcount] = remainder + 48;

	for (uint8 i = tempcount; i > 0; i--)
	{
		this->addbody(tempstring[i]);
	}
	this->addbody(tempstring[0]);
}

void DataHub::addbody(long value)
{
	if (value < 0)
	{
		this->addbody('-');
		this->addbody((unsigned long)(-1*value));
	}
	else
	{ 
		this->addbody((unsigned long)value);
	}
}

int DataHub::post(String contenttype, String apikey)
{
	int httpcode = -1;

	if (WiFi.status() == WL_CONNECTED) {

		offset = 0;

		while (counter != 0)
		{
			if (counter > BATCH_SIZE + offset)
			{
				postsize = BATCH_SIZE;
			}
			else
			{
				postsize = counter - offset;
				counter = 0;
			}

			http.begin(server);
			http.addHeader("Content-Type", contenttype);
			http.addHeader("x-mac-addr", WiFi.macAddress());
			http.addHeader("x-api-key", apikey);

			if (has_measurement_id) http.addHeader("x-post-id", measurement_id);

			httpcode = http.POST(buffer + offset, postsize);
			yield();
			String response = http.getString();

			if (response.length() >= 36)
			{
				measurement_id = response.substring(0, 36);
				has_measurement_id = true;
			}

			http.end();

			offset += BATCH_SIZE;
		}
		return httpcode;
	}
}

DataHub::~DataHub()
{
}
