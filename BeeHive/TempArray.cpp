#include "TempArray.h"

TempArray::TempArray(OneWire* ds)
{
	_ds = ds;
}

TempArray::~TempArray()
{
}

String TempArray::JSON()
{
	String result = "\"temperature\": [";

	for (i = 0; i < _count; i++)
	{
		address = _addr[i];
		result.concat(_temperatures[i]);
		if (i < _count - 1) result.concat(',');
	}
	result.substring(0, result.length() - 2);
	result.concat(']');
	return result;
}

void TempArray::init(byte** addr, float* temperatures, uint8 count)
{
	_addr = addr;
	_count = count;
	_temperatures = temperatures;

	for (i = 0; i < _count; i++)
	{
		address = _addr[i];

		if (OneWire::crc8(address, 7) != address[7]) {
//			Serial.println("CRC is not valid!");
		}
		else
		{
			_ds->reset();
			_ds->select(address);
			_ds->write(0x44, 1);
		}
	}
}

void TempArray::sleep()
{
	//nothing
}

int TempArray::gettype(byte id)
{
	switch (id) {
	case 0x10:
		return 1;
		break;
	case 0x28:
		return 0;
		break;
	case 0x22:
		return 0;
		break;
	default:
		return -1;
	}
}

void TempArray::readtemps()
{
	for (i = 0; i < _count; i++)
	{
		address = _addr[i];

		_ds->reset();
		_ds->select(address);
		_ds->write(0xBE);         // Read Scratchpad

		for (j = 0; j < 9; j++) {           // we need 9 bytes
//			Serial.print("iteration: "); Serial.println(j);
			data[j] = _ds->read();
		}


		int16_t raw = (data[1] << 8) | data[0];

		type_s = this->gettype(address[0]);

		if (type_s == 1) {
			raw = raw << 3; // 9 bit resolution default
			if (data[7] == 0x10) {
				// "count remain" gives full 12 bit resolution
				raw = (raw & 0xFFF0) + 12 - data[6];
			}
		}
		else {
			cfg = (data[4] & 0x60);
			// at lower res, the low bits are undefined, so let's zero them
			if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
			else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
			else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
												  //// default is 12 bit resolution, 750 ms conversion time
		}
		_temperatures[i] = (float)raw / 16.0;
		yield();
	}
}
