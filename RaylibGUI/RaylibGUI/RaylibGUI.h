#pragma once
#include "raylib.h"
#include <list>
#include "Vec2.h"
#include <iostream>
#include <functional>
#include <concepts>
#include <utility>

class UIContainer;
class UIElement
{
public:
	UIElement(UIContainer* parentContainer);
	~UIElement();
	bool hidden = false;
	virtual void draw(float alpha = 255) = 0;
	virtual void update() = 0;
	
	virtual void setPos(const Vec2& pos) = 0;
	virtual Vec2 getPos() = 0;

protected:
	UIContainer* parentContainer;
};

class UIContainer
{
public:
	UIContainer(Color& primaryColor, Color& secondaryColor, Color& textColor) :
		primaryColor(primaryColor), 
		secondaryColor(secondaryColor), 
		textColor(textColor) {}

	UIContainer(UIContainer& uiContainer) : 
		primaryColor(uiContainer.primaryColor), 
		secondaryColor(uiContainer.secondaryColor), 
		textColor(uiContainer.textColor) {}

	~UIContainer()
	{
		while (childElements.size())
		{
			delete childElements.front();
		}
	}

	virtual void add(UIElement* element);
	virtual void remove(UIElement* element);

	void update()
	{
		for (auto element : childElements)
		{
			if (!element->hidden)
			{
				element->update();
			}
		}
	}
	void draw(float alpha = 255)
	{
		for (auto element : childElements)
		{
			if (!element->hidden)
			{
				element->draw(alpha);
			}
		}
	}
	
	Color& primaryColor;
	Color& secondaryColor;
	Color& textColor;
protected:
	std::list<UIElement*> childElements;
};

class UISubContainer : public UIContainer, public UIElement
{
public:
	UISubContainer(UIContainer* parentContainer) : UIElement(parentContainer), UIContainer(*parentContainer) {};
	~UISubContainer() {};

	void UIElement::draw(float alpha)
	{
		UIContainer::draw();
	}
	void UIElement::update()
	{
		UIContainer::update();
	}
private:
};

class UIButton : public UIElement
{
public:
	UIButton(UIContainer* parentContainer) : UIElement(parentContainer) {};
	~UIButton() {};

	bool isPressed(MouseButton mousebutton);
	bool isDown(MouseButton mousebutton);
	virtual bool isOver() = 0;
protected:
	bool isover = false;
};

template<typename T>
struct is_simple_button : std::false_type {};


class UIButtonRec : public UIButton
{
public:
	UIButtonRec(UIContainer* parentContainer, Rectangle buttonRec) : UIButton(parentContainer), buttonRec(buttonRec)
	{
	};
	~UIButtonRec() {};

	virtual void UIElement::draw(float alpha)
	{
		if (isOver()) {
			Color color = parentContainer->primaryColor;
			color.a = alpha;
			DrawRectangleRec(buttonRec, color);
		}
		else
		{
			Color color = parentContainer->secondaryColor;
			color.a = alpha;
			DrawRectangleRec(buttonRec, color);
		}
	}

	virtual void UIElement::update() {}

	virtual bool UIButton::isOver();

	virtual void UIElement::setPos(const Vec2& pos)
	{
		buttonRec.x = pos.x - buttonRec.width / 2;
		buttonRec.y = pos.y - buttonRec.height / 2;
	}

	virtual Vec2 UIElement::getPos()
	{
		return Vec2(buttonRec.x + buttonRec.width / 2, buttonRec.y + buttonRec.height / 2);
	}

protected:
	Rectangle buttonRec;
};

template<>
struct is_simple_button<UIButtonRec> : std::true_type {};


class UIButtonCircle : public UIButton
{
public:
	UIButtonCircle(UIContainer* parentContainer, Vec2 pos, float radius) : UIButton(parentContainer), pos(pos),radius(radius) {};
	~UIButtonCircle() {};

	virtual void UIElement::draw(float alpha)
	{
		if (isOver()) {
			DrawCircleV(pos.vector2(), radius,parentContainer->primaryColor);
		}
		else
		{
			DrawCircleV(pos.vector2(), radius,parentContainer->secondaryColor);
		}
	}
	virtual void UIElement::update() {};

	virtual bool UIButton::isOver();

	virtual void UIElement::setPos(const Vec2& pos)
	{
		UIButtonCircle::pos = pos;
	}

	virtual Vec2 UIElement::getPos()
	{
		return pos;
	}

protected:
	Vec2 pos;
	float radius;
};

template<>
struct is_simple_button<UIButtonCircle> : std::true_type {};


class UISlider : public UIButtonRec
{
public:
	UISlider(UIContainer* parentContainer, Rectangle buttonRec, float speed) : UIButtonRec(parentContainer, buttonRec), speed(speed)
	{
		if (buttonRec.height > buttonRec.width)
		{
			radius = buttonRec.width / 2;
			range = (buttonRec.height - buttonRec.width);
		}
		else
		{
			radius = buttonRec.height / 2;
			range = (buttonRec.width - buttonRec.height);
		}
	};
	~UISlider() {};

	virtual void UIElement::update()
	{
		if (isPressed(MOUSE_LEFT_BUTTON))
		{
			griped = true;
		}

		if (isover)
		{
			value -= GetMouseWheelMove() / range * speed;
		}

		if (griped)
		{
			if (buttonRec.height > buttonRec.width)
			{
				value = (GetMouseY() - buttonRec.y - radius) / range;
			}
			else
			{
				value = (GetMouseX() - buttonRec.x - radius) / range;
			}
			griped = !IsMouseButtonUp(MOUSE_LEFT_BUTTON);
		}
		if (value > 1)
		{
			value = 1;
		}
		else if (value < 0)
		{
			value = 0;
		}
	}

	virtual void UIElement::draw(float alpha)
	{
		DrawRectangleRec(buttonRec, parentContainer->secondaryColor);
		if (buttonRec.height > buttonRec.width)
		{
			DrawCircle(buttonRec.x + radius, buttonRec.y + range * value + radius, radius, parentContainer->primaryColor);
		}
		else
		{
			DrawCircle(buttonRec.x + range * value + radius, buttonRec.y + radius, radius, parentContainer->primaryColor);
		}
	}

	float value = 0;
private:
	bool griped = false;
	float radius;	
	float range;
	float speed;
};

class UISliderContainer : public UIContainer, public UISlider
{
public:
	UISliderContainer(UIContainer* parentContainer, Rectangle buttonRec, float speed) : UISlider(parentContainer, buttonRec,speed) , UIContainer(*parentContainer) {};
	~UISliderContainer() {};

	virtual void UIElement::update()
	{
		UISlider::update();
		UIContainer::update();
		findRange();
		float moveAmount = (oldValue - value) * range;
		if (buttonRec.width < buttonRec.height)
		{
			for (auto element : childElements)
			{
				Vec2 newpos = element->getPos();
				newpos.y += moveAmount;
				element->setPos(newpos);
			}
		}
		else
		{
			for (auto element : childElements)
			{
				Vec2 newpos = element->getPos();
				newpos.x += moveAmount;
				element->setPos(newpos);
			}
		}
		oldValue = value;
	}

	virtual void UIElement::draw(float alpha)
	{
		UISlider::draw(alpha);
		if (buttonRec.width < buttonRec.height)
		{
			float drawRange = buttonRec.height / 2;
			for (auto element : childElements)
			{
				element->hidden = abs(getPos().y - element->getPos().y) > drawRange;
			}
		}
		else
		{
			float drawRange = buttonRec.width / 2;
			for (auto element : childElements)
			{
				element->hidden = abs(getPos().x - element->getPos().x) > drawRange;
			}
		}
		UIContainer::draw();
	}

private:
	float oldValue = 0;
	float range = 0;

	void findRange()
	{
		float firstElementPos;
		float lastElementPos;
		if (buttonRec.width < buttonRec.height)
		{
			firstElementPos = childElements.front()->getPos().y;
			lastElementPos = firstElementPos;
			for (auto element : childElements)
			{
				float elementPos = element->getPos().y;
				if (firstElementPos > elementPos)
				{
					firstElementPos = elementPos;
				}
				if (lastElementPos < elementPos)
				{
					lastElementPos = elementPos;
				}
			}
		}
		else
		{
			firstElementPos = childElements.front()->getPos().x;
			lastElementPos = firstElementPos;
			for (auto element : childElements)
			{
				float elementPos = element->getPos().x;
				if (firstElementPos > elementPos)
				{
					firstElementPos = elementPos;
				}
				if (lastElementPos < elementPos)
				{
					lastElementPos = elementPos;
				}
			}
		}
		range = (lastElementPos - firstElementPos);
	}
};

template<typename T>
concept SimpleButton = is_simple_button<T>::value && std::derived_from<T, UIButton>;

template<SimpleButton ButtonType>
class UIFunctionButton : public UIButton, public UIContainer
{
	UIButton* button;
	std::function<void()> function;
public:
	template<typename... Args>
	UIFunctionButton(UIContainer* parentContainer, std::function<void()> func, Args&&... args) : UIButton(parentContainer), UIContainer(*parentContainer), function(func)
	{
		button = new ButtonType(this, std::forward<Args>(args)...);
	}
	~UIFunctionButton()
	{
		delete button;
	}
	
	virtual void UIElement::update()
	{
		if (button)
		{
			if (button->isPressed(MOUSE_LEFT_BUTTON))
			{
				function();
			}
		}
		else
		{
			delete this;
		}
	}

	virtual void UIElement::draw(float alpha)
	{
		button->draw(alpha);
	}

	virtual void UIElement::setPos(const Vec2& pos)
	{
		button->setPos(pos);
	}

	virtual Vec2 UIElement::getPos()
	{
		return button->getPos();
	}

	virtual bool UIButton::isOver()
	{
		return button->isOver();
	}

private:

};

