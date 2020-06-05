/***********************************************************************
* Project           : Game
* Author            : Felipe Bellini
* Date created      : Oct. 12 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#pragma warning(push, 0)   
#include <rapidxml.hpp>
#pragma warning(pop)
#include <Level.h>

class StateSaver
{
private:
	std::string GetTimeStamp(bool time = true, bool date = true);
	std::string SaveXML(rapidxml::xml_document<>* xml, std::string directory, std::string filename);
	void WriteXMLHeader(std::string version, std::string encoding, rapidxml::xml_document<>* xml);
	void Serialize(Level* gameData, rapidxml::xml_document<>* xml);
public:
	std::string SaveState(Level* level);
};

