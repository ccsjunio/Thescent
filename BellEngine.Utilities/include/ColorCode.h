#pragma once

class ColorCode
{
public:
	enum class ColorCode_e
	{		
		UNKNOWN = -1,

		BLACK,
		WHITE,
		RED,
		BLUE,
		GREEN,
		YELLOW
	};

	inline static ColorCode::ColorCode_e GetColourCode(unsigned char r, unsigned char g, unsigned char b)
	{
		if (r == 255 && g == 0 && b == 0)		return ColorCode::ColorCode_e::RED;
		if (r == 0 && g == 255 && b == 0)		return ColorCode::ColorCode_e::GREEN;
		if (r == 0 && g == 0 && b == 255)		return ColorCode::ColorCode_e::BLUE;
		if (r == 255 && g == 255 && b == 255)	return ColorCode::ColorCode_e::WHITE;
		if (r == 0 && g == 0 && b == 0)			return ColorCode::ColorCode_e::BLACK;
		if (r == 255 && g == 255 && b == 0)		return ColorCode::ColorCode_e::YELLOW;
		
		return ColorCode::ColorCode_e::UNKNOWN;
	}
};