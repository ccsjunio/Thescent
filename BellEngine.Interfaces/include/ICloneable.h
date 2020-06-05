#pragma once
class ICloneable
{
protected:
	ICloneable() {};

public:
	virtual ~ICloneable() {};
	virtual ICloneable* Clone() = 0;
};