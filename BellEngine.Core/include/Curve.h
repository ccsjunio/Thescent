#pragma once
#include <glm/gtx/spline.hpp>

class Curve
{
public:
	enum class Curve_e
	{
		catmullRom,
		cubic,
		hermite
	};

	template <typename T>
	static T GetCurvePoint
	(
		const T& start, 
		const T& end, 
		const T& controlPointA, 
		const T& controlPointB, 
		const float& ratio, 
		Curve_e curveOption = Curve_e::catmullRom);
};

template<typename T>
inline T Curve::GetCurvePoint(const T& start, const T& end, const T& controlPointA, const T& controlPointB, const float& ratio, Curve_e curveOption)
{
	switch (curveOption)
	{
	case Curve::Curve_e::catmullRom:
		return glm::catmullRom(controlPointA, start, end, controlPointB, ratio);
	case Curve::Curve_e::cubic:
		return glm::cubic(controlPointA, start, end, controlPointB, ratio);
	case Curve::Curve_e::hermite:
		return glm::hermite(controlPointA, start, end, controlPointB, ratio);
	default:
		return T();
	}
}
