#include "Config.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>

Config::Config()
{
	properties["Audio_SoundVolume"] = "100";
	properties["Audio_MusicVolume"] = "100";
	properties["Visual_Fullscreen"] = "false";

	std::ifstream iFile("config.txt");

	if (iFile.is_open())
	{
		std::string line;
		while (getline(iFile, line))
		{
			std::vector<std::string> tokens;
			std::istringstream iss(line);
			copy(std::istream_iterator<std::string>(iss),
				std::istream_iterator<std::string>(),
				std::back_inserter(tokens));

			if (tokens.size() == 3 && tokens[0][0] != '/')
			{
				properties[tokens[0]] = tokens[2];
			}
			else if (tokens.size() == 1)
			{
				properties[tokens[0]] = "";
			}
		}
	}
	else
	{
		OutputProperties();
	}
}


Config::~Config()
{
	OutputProperties();
	properties.clear();
}

void Config::OutputProperties()
{
	std::ofstream oFile("config.txt");

	if (oFile.is_open())
	{
		oFile << "/                                                        /"	<< std::endl;
		oFile << "/ Template                                /"	<< std::endl;
		oFile << "/ Build Date: " << __DATE__ << " - " << __TIME__ << "			 /"	<< std::endl;
		oFile << "/ Game Developed by Gamepopper. As well as this config.  /"	<< std::endl;
		oFile << "/                                                        /"	<< std::endl;
		oFile << "/ Make sure you have a ' = ' between property and value. /"	<< std::endl;
		oFile << "/                                                        /"	<< std::endl << std::endl;

		for (std::pair<std::string, std::string> pair : properties)
		{
			if (pair.second != "")
			{
				oFile << pair.first << " = " << pair.second << std::endl;
			}
			else
			{
				oFile << pair.first << std::endl;
			}
		}
	}
}

bool Config::PropertiesContains(std::string name)
{
	for (std::pair<std::string, std::string> pair : properties)
	{
		if (pair.first.find(name) != std::string::npos)
		{
			return true;
		}
	}

	return false;
}

bool Config::DoesPropertyExist(std::string Name)
{
	return properties.find(Name) != properties.end();
}

bool Config::GetPropertyAsBool(std::string Name)
{
	return properties[Name] == "true" ? true : false;
}

int Config::GetPropertyAsInt(std::string Name)
{
	return std::stoul(properties[Name]);
}

std::string Config::GetPropertyAsString(std::string Name)
{
	return properties[Name];
}

void Config::SetPropertyAsBool(std::string Name, bool Value)
{
	properties[Name] = Value ? "true" : "false";
}

void Config::SetPropertyAsInt(std::string Name, int Value)
{
	properties[Name] = std::to_string(Value);
}

void Config::SetPropertyAsString(std::string Name, std::string Value)
{
	properties[Name] = Value;
}
