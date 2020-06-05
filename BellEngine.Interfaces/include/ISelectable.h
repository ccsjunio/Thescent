#pragma once

class ISelectable
{
private:
	bool isSelected;

protected:
	ISelectable() : isSelected(false) {};

public:
	virtual ~ISelectable() {};

	void Select()
	{
		this->isSelected = true;
	}
	void Deselect()
	{
		this->isSelected = false;
	}
	bool IsSelected()
	{
		return this->isSelected;
	}
};