#pragma once
#include <Esp.h>
#include <OneWire.h>

class TempArray
{
private:
	OneWire* _ds;
	byte** _addr;
	float* _temperatures;
	uint8 _count;
	uint8 i, j;
	byte* address;
	byte data[12];
	byte type_s;
	byte cfg;
	int gettype(byte id);

public:
	TempArray(OneWire* ds);
	~TempArray();

	void init(byte** addr, float* temperatures, uint8 count);
	void sleep();
	void readtemps();
	String JSON();
};

