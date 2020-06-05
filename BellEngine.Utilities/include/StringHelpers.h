#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include <assert.h>
#include <MathLibrary.h>

class StringHelpers
{
public:

	inline static std::string GenerateRandomString(const int& length)
	{
		static const char alphanum[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";

		std::string str;
		for (int i = 0; i < length; ++i)
		{
			char ch = alphanum[rand() % (sizeof(alphanum) - 1)];
			str += ch;
		}

		return str;
	}

	inline static std::string GetNameWithoutExtension(const std::string& fileName)
	{
		size_t lastdot = fileName.find_last_of(".");
		if (lastdot == std::string::npos)
		{
			return fileName;
		}

		return fileName.substr(0, lastdot);
	}
	
	inline static std::string GetFileNameFromFilePath(const std::string& filePath)
	{
		return filePath.substr(filePath.find_last_of("/\\") + 1);
	}

	inline static char* StringReplace(char const* const original, char const* const pattern, char const* const replacement)
	{
		size_t const replen = strlen(replacement);
		size_t const patlen = strlen(pattern);
		size_t const orilen = strlen(original);

		size_t patcnt = 0;
		const char* oriptr;
		const char* patloc;

		// find how many times the pattern occurs in the original string
		for (oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen)
		{
			patcnt++;
		}

		{
			// allocate memory for the new string
			size_t const retlen = orilen + patcnt * (replen - patlen);
			char* const returned = (char*)malloc(sizeof(char) * (retlen + 1));

			if (returned != NULL)
			{
				// copy the original string, 
				// replacing all the instances of the pattern
				char* retptr = returned;
				for (oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen)
				{
					size_t const skplen = patloc - oriptr;
					// copy the section until the occurence of the pattern
					strncpy(retptr, oriptr, skplen);
					retptr += skplen;
					// copy the replacement 
					strncpy(retptr, replacement, replen);
					retptr += replen;
				}
				// copy the rest of the string.
				strcpy(retptr, oriptr);
			}
			return returned;
		}
	}

	inline static std::string GetAllLinesFromFile(const std::string& path)
	{
		std::ifstream file(path);
		std::string str;
		std::string file_contents;

		while (std::getline(file, str))
		{
			file_contents += str;
			file_contents.push_back('\n');
		}

		file.close();

		return file_contents;
	}

	inline static wchar_t* GetWCharBuffer(const std::string& input)
	{
		size_t length = input.length();
		const char* cInput = input.c_str();
		// error if wn == size_t(-1)
		wchar_t* buf = new wchar_t[length + 1]();  // value-initialize to 0 (see below)
		std::mbstate_t state = std::mbstate_t();
		//length = mbsrtowcs(buf, &cInput, length + 1, &state);
		size_t retVal;
		errno_t rc = mbsrtowcs_s(&retVal, buf, length + 1, &cInput, length, &state);
		// error if wn == size_t(-1)
		assert(strcmp(cInput, "") == 0); // successful conversion
		// result now in buf, return e.g. as std::wstring
		return buf;
	}

	inline static std::string Vec2ToStr(const Vector2D& vec)
	{
		return std::to_string(vec.x) + " " + std::to_string(vec.y);
	}

	inline static Vector2D StrToVec2(const std::string& vec)
	{
		std::stringstream ss(vec);

		float v[2] = {};
		unsigned int i = 0;
		std::string temp;
		while (ss >> temp)
		{
			v[i] = strtof(temp.c_str(), NULL);
			i++;

			if (i > 1) break;
		}

		return Vector2D(v[0], v[1]);
	}

	inline static std::string Vec3ToStr(const Vector3D& vec)
	{
		return std::to_string(vec.x) + " " + std::to_string(vec.y) + " " + std::to_string(vec.z);
	}

	inline static Vector3D StrToVec3(const std::string& vec)
	{
		std::stringstream ss(vec);

		float v[3] = {};
		unsigned int i = 0;
		std::string temp;
		while (ss >> temp)
		{
			v[i] = strtof(temp.c_str(), NULL);
			i++;

			if (i > 2) break;
		}

		return Vector3D(v[0], v[1], v[2]);
	}

	inline static std::string Vec4ToStr(const Vector4D& vec)
	{
		return std::to_string(vec.x) + " " + std::to_string(vec.y) + " " + std::to_string(vec.z) + " " + std::to_string(vec.w);
	}

	inline static Vector4D StrToVec4(const std::string& vec)
	{
		std::stringstream ss(vec);

		float v[4] = {};
		unsigned int i = 0;
		std::string temp;
		while (ss >> temp)
		{
			v[i] = strtof(temp.c_str(), NULL);
			i++;

			if (i > 3) break;
		}

		return Vector4D(v[0], v[1], v[2], v[3]);
	}

	/**
	 * HexToInt
	 * take a hex string and convert it to a 32bit number (max 8 hex digits)
	 */
	inline const int HexToInt(char*& hex)
	{
		//https://stackoverflow.com/questions/10156409/convert-hex-string-char-to-int
		uint32_t val = 0;
		while (*hex)
		{
			// get current character then increment
			uint8_t byte = *hex++;
			// transform hex character to the 4bit equivalent number, using the ascii table indexes
			if (byte >= '0' && byte <= '9') byte = byte - '0';
			else if (byte >= 'a' && byte <= 'f') byte = byte - 'a' + 10;
			else if (byte >= 'A' && byte <= 'F') byte = byte - 'A' + 10;
			// shift 4 to make space for new digit, and add the 4 bits of the new digit 
			val = (val << 4) | (byte & 0xF);
		}
		return val;
	}
};
