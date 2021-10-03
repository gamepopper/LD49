#pragma once

#ifdef WIN32
#define strncpy strncpy_s
#define strncat(x,y,z) strncat_s(x,z,y,z)
#endif

struct Item
{
	char name[255];
	int number;

	Item()
	{
		strncpy(name, "", 255);
		number = 0;
	}

	Item(const char* n)
	{
		strncpy(name, n, 255);
		number = 0;
	}

	Item(const char* n, int num)
	{
		strncpy(name, n, 255);
		number = num;
	}
};