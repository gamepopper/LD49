#pragma once
#include <map>
#include <string>

#define GEMSTONE_KEEPER_BUILD "v1.0.7"

class Config
{
private:
	std::map<std::string, std::string> properties;
	void OutputProperties();

public:
	Config();
	~Config();

	bool PropertiesContains(std::string name);

	bool DoesPropertyExist(std::string name);

	bool GetPropertyAsBool(std::string Name);
	int GetPropertyAsInt(std::string Name);
	std::string GetPropertyAsString(std::string Name);

	void SetPropertyAsBool(std::string Name, bool Value);
	void SetPropertyAsInt(std::string Name, int Value);
	void SetPropertyAsString(std::string Name, std::string Value);
};

