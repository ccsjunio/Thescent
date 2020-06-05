#include "ShaderFileReader.h"
#pragma warning(push, 0)   
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#pragma warning(pop)
#if __cplusplus == 201703L
	#include <filesystem>
#endif
#include <EngineFolders.h>
#include <map>
#include <vector>
#include <sstream>

const bool ShaderFileReader::ReadShaderContainer(ParameterContainer& input, ParameterContainer& output)
{
	Parameter p;
	input.FindParameterByName("filepath", p);
	std::string path = p.value;

#if __cplusplus == 201703L
	if (!std::filesystem::exists(path))
	{
		return false;
	}
#endif

	//load file
	rapidxml::file<> xmlFile(path.c_str());
	rapidxml::xml_document<>* xml = new rapidxml::xml_document<>();
	if (xml)
	{
		char* xmlData = xmlFile.data();
		if (xmlData)
		{
			xml->parse<0>(xmlData);

			//find root
			rapidxml::xml_node<>* root = xml->first_node();
			if (root)
			{
				std::string type;
				auto xmlType = root->first_attribute("Type");
				if (xmlType)
				{
					type = xmlType->value();
				}

				rapidxml::xml_node<char>* xmlShaderPrograms = root->first_node("ShaderPrograms");
				if (xmlShaderPrograms)
				{
					std::vector<ParameterContainer>* paramsCollection = new std::vector<ParameterContainer>();

					for (rapidxml::xml_node<>* xmlShaderProgram = xmlShaderPrograms->first_node("Program");
						xmlShaderProgram;
						xmlShaderProgram = xmlShaderProgram->next_sibling())
					{
						ParameterContainer params;
						rapidxml::xml_node<char>* xmlParameters = xmlShaderProgram->first_node("Parameters");
						if (xmlParameters)
						{
							for (rapidxml::xml_node<>* xmlParameter = xmlParameters->first_node();
								xmlParameter;
								xmlParameter = xmlParameter->next_sibling())
							{
								std::string paramType;
								rapidxml::xml_attribute<char>* xml_ParamType = xmlParameter->first_attribute("Type");
								if (xml_ParamType)
								{
									paramType = xml_ParamType->value();
								}

								bool variable = false;
								rapidxml::xml_attribute<char>* xml_ParamVariable = xmlParameter->first_attribute("Variable");
								if (xml_ParamVariable)
								{
									variable = xml_ParamVariable->value() != "0";
								}

								std::string paramName = xmlParameter->name();
								std::string paramValue = xmlParameter->value();

								params.Add(paramType, paramName, paramValue, variable);
							}
						}

						paramsCollection->push_back(params);
					}

					output.Add("string", "Type", type);
					output.Add("std::vector", "parameters_collection", paramsCollection);

					delete xml;
					xml = nullptr;

					return true;
				}
				else
				{
					delete xml;
					xml = nullptr;

					return false;
				}
			}
			else
			{
				//Root not found
				delete xml;
				xml = nullptr;

				return false;
			}
		}
	}

	return false;
}

ShaderFileReader::ShaderFileReader()
{
	
}

ShaderFileReader::~ShaderFileReader()
{
	if (this->containerFactory)
	{
		delete (this->containerFactory);
		this->containerFactory = nullptr;
	}
}

const bool ShaderFileReader::ReadToParameterContainer(ParameterContainer& input, ParameterContainer& output)
{
	Parameter p;

	if (input.FindParameterByName("FileType", p))
	{
		if (p.value == "ShaderContainer")
		{
			return ReadShaderContainer(input, output);
		}
	}

	return false;
}
