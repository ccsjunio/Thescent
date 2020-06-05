#pragma once
#include <glm/gtx/easing.hpp>

class Easing
{
public:
	enum class Easing_e
	{
		None,

		linearInterpolation,
		quadraticEaseIn,
		quadraticEaseOut,
		quadraticEaseInOut,
		cubicEaseIn,
		cubicEaseOut,
		cubicEaseInOut,
		quarticEaseIn,
		quarticEaseOut,
		quarticEaseInOut,
		quinticEaseIn,
		quinticEaseOut,
		quinticEaseInOut,
		sineEaseIn,
		sineEaseOut,
		sineEaseInOut,
		circularEaseIn,
		circularEaseOut,
		circularEaseInOut,
		exponentialEaseIn,
		exponentialEaseOut,
		exponentialEaseInOut,
		elasticEaseIn,
		elasticEaseOut,
		elasticEaseInOut,
		backEaseIn,
		backEaseOut,
		backEaseInOut,
		bounceEaseIn,
		bounceEaseOut,
		bounceEaseInOut
	};	

	template <typename T>
	static T GetEasingValue(T const& ratio, Easing_e easingOption = Easing_e::None);
};

template<typename T>
inline T Easing::GetEasingValue(T const& ratio, Easing_e easingOption)
{
	switch (easingOption)
	{
	case Easing::Easing_e::None:
	default:
		return ratio;
	case Easing::Easing_e::linearInterpolation:
		return glm::linearInterpolation(ratio);
	case Easing::Easing_e::quadraticEaseIn:
		return glm::quadraticEaseIn(ratio);
	case Easing::Easing_e::quadraticEaseOut:
		return glm::quadraticEaseOut(ratio);
	case Easing::Easing_e::quadraticEaseInOut:
		return glm::quadraticEaseInOut(ratio);
	case Easing::Easing_e::cubicEaseIn:
		return glm::cubicEaseIn(ratio);
	case Easing::Easing_e::cubicEaseOut:
		return glm::cubicEaseOut(ratio);
	case Easing::Easing_e::cubicEaseInOut:
		return glm::cubicEaseInOut(ratio);
	case Easing::Easing_e::quarticEaseIn:
		return glm::quarticEaseIn(ratio);
	case Easing::Easing_e::quarticEaseOut:
		return glm::quarticEaseOut(ratio);
	case Easing::Easing_e::quarticEaseInOut:
		return glm::quarticEaseInOut(ratio);
	case Easing::Easing_e::quinticEaseIn:
		return glm::quinticEaseIn(ratio);
	case Easing::Easing_e::quinticEaseOut:
		return glm::quinticEaseOut(ratio);
	case Easing::Easing_e::quinticEaseInOut:
		return glm::quinticEaseInOut(ratio);
	case Easing::Easing_e::sineEaseIn:
		return glm::sineEaseIn(ratio);
	case Easing::Easing_e::sineEaseOut:
		return glm::sineEaseOut(ratio);
	case Easing::Easing_e::sineEaseInOut:
		return glm::sineEaseInOut(ratio);
	case Easing::Easing_e::circularEaseIn:
		return glm::circularEaseIn(ratio);
	case Easing::Easing_e::circularEaseOut:
		return glm::circularEaseOut(ratio);
	case Easing::Easing_e::circularEaseInOut:
		return glm::circularEaseInOut(ratio);
	case Easing::Easing_e::exponentialEaseIn:
		return glm::exponentialEaseIn(ratio);
	case Easing::Easing_e::exponentialEaseOut:
		return glm::exponentialEaseOut(ratio);
	case Easing::Easing_e::exponentialEaseInOut:
		return glm::exponentialEaseInOut(ratio);
	case Easing::Easing_e::elasticEaseIn:
		return glm::elasticEaseIn(ratio);
	case Easing::Easing_e::elasticEaseOut:
		return glm::elasticEaseOut(ratio);
	case Easing::Easing_e::elasticEaseInOut:
		return glm::elasticEaseInOut(ratio);
	case Easing::Easing_e::backEaseIn:
		return glm::backEaseIn(ratio);
	case Easing::Easing_e::backEaseOut:
		return glm::backEaseOut(ratio);
	case Easing::Easing_e::backEaseInOut:
		return glm::backEaseInOut(ratio);
	case Easing::Easing_e::bounceEaseIn:
		return glm::bounceEaseIn(ratio);
	case Easing::Easing_e::bounceEaseOut:
		return glm::bounceEaseOut(ratio);
	case Easing::Easing_e::bounceEaseInOut:
		return glm::bounceEaseInOut(ratio);
	}
}
