#include "Input.h"
#include "UIEntity.h"

Input::Input()
{
	for (int i = 0; i < 3; i++)
		MousePress[i] = false;
	MouseX = 0;
	MouseY = 0;
}


D2D1_POINT_2F Input::GetMousePos()
{
	return D2D1::Point2F(MouseX, MouseY);
}

void Input::SetMousePos(unsigned short x, unsigned short y)
{
	MouseX = x;
	MouseY = y;
	for (auto it = UI_Entity::listOfUIEnt.begin(); it != UI_Entity::listOfUIEnt.end(); ++it)
	{
		if (MouseX >= (*it)->pos.x &&
			MouseX <= (*it)->size.width + (*it)->pos.x &&
			MouseY >= (*it)->pos.y &&
			MouseY <= (*it)->size.height + (*it)->pos.y)
		{
			(*it)->hover = true;
		} 
		else (*it)->hover = false;
	}
}

void Input::MouseKeyDown(Input::MouseKeys key)
{
	bool flag = false;
	MousePress[key] = true;
	if (key == Input::LButton)
	{
		for (auto it = UI_Entity::listOfUIEnt.begin(); it != UI_Entity::listOfUIEnt.end(); ++it)
		{
			if ((*it)->hover && !flag)
			{
				(*it)->active = true;
				flag = true;
			}
			else (*it)->active = false;
		}
	}
}

void Input::MouseKeyUp(Input::MouseKeys key)
{
	MousePress[key] = false;
	if (key == Input::LButton)
	{
		for (auto it = UI_Entity::listOfUIEnt.begin(); it != UI_Entity::listOfUIEnt.end(); ++it)
			if ((*it)->active)
				(*it)->mouseUp = true;
	}
}

bool Input::IsMouseKeyDown(const Input::MouseKeys key)
{
	return MousePress[key];
}