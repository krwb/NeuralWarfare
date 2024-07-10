#pragma once
#include "raylib.h"

#include <list>
#include "Vec2.h"
#include <iostream>
#include <functional>
#include <concepts>
#include <utility>
#include <type_traits>

class UIContainer;


/// <summary>
/// Base class for UI elements.
/// </summary>
class UIElement
{
public:
	/// <summary>
	/// Constructor for UIElement.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	UIElement(UIContainer* parentContainer);

	/// <summary>
	/// Virtual destructor for UIElement.
	/// </summary>
	virtual ~UIElement();

	bool hidden = false; // Visibility flag for the UI element.

	/// <summary>
	/// Draws the UI element.
	/// </summary>
	/// <param name="alpha">Optional alpha value for transparency.</param>
	virtual void draw(float alpha = 0) = 0;

	/// <summary>
	/// Updates the UI element.
	/// </summary>
	virtual void update() = 0;

	/// <summary>
	/// Sets the position of the UI element.
	/// </summary>
	/// <param name="pos">New position.</param>
	virtual void setPos(const Vec2& pos) = 0;

	/// <summary>
	/// Gets the position of the UI element.
	/// </summary>
	/// <returns>Current position.</returns>
	virtual Vec2 getPos() = 0;

protected:
	UIContainer* parentContainer; // Pointer to the parent container.
};


/// <summary>
/// Container for managing multiple UI elements.
/// </summary>
class UIContainer
{
public:
	/// <summary>
	/// Constructs a UIContainer with specified colors.
	/// </summary>
	/// <param name="primaryColor">Primary color.</param>
	/// <param name="secondaryColor">Secondary color.</param>
	/// <param name="textColor">Text color.</param>
	UIContainer(Color& primaryColor, Color& secondaryColor, Color& textColor) :
		primaryColor(primaryColor),
		secondaryColor(secondaryColor),
		textColor(textColor) {}

	/// <summary>
	/// Copy constructor for UIContainer.
	/// </summary>
	/// <param name="uiContainer">UIContainer to copy from.</param>
	UIContainer(UIContainer& uiContainer) :
		primaryColor(uiContainer.primaryColor),
		secondaryColor(uiContainer.secondaryColor),
		textColor(uiContainer.textColor) {}

	virtual ~UIContainer()
	{
		while (childElements.size())
		{
			delete childElements.front();
		}
	}

	/// <summary>
	/// Adds a UI element to the container.
	/// </summary>
	/// <param name="element">Element to add.</param>
	virtual void add(UIElement* element);

	/// <summary>
	/// Removes a UI element from the container.
	/// </summary>
	/// <param name="element">Element to remove.</param>
	virtual void remove(UIElement* element);

	/// <summary>
	/// Updates all visible child elements.
	/// </summary>
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

	/// <summary>
	/// Draws all visible child elements.
	/// </summary>
	/// <param name="alpha">Optional alpha value for transparency.</param>
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

	Color& primaryColor; // Primary color for the container.
	Color& secondaryColor; // Secondary color for the container.
	Color& textColor; // Text color for the container.
protected:
	std::list<UIElement*> childElements; // List of child elements.
};

/// <summary>
/// A UIContainer that also functions as a UIElement.
/// </summary>
class UISubContainer : public UIContainer, virtual public UIElement
{
public:
	/// <summary>
	/// Constructs a UISubContainer.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	UISubContainer(UIContainer* parentContainer) : UIElement(parentContainer), UIContainer(*parentContainer) {};

	virtual ~UISubContainer() {};

	/// <summary>
	/// Draws the UISubContainer and its elements.
	/// </summary>
	void UIElement::draw(float alpha)
	{
		UIContainer::draw();
	}

	/// <summary>
	/// Updates the UISubContainer and its elements.
	/// </summary>
	void UIElement::update()
	{
		UIContainer::update();
	}
private:
};

/// <summary>
/// Button UI element.
/// </summary>
class UIButton : virtual public UIElement
{
public:
	/// <summary>
	/// Constructs a UIButton.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	UIButton(UIContainer* parentContainer) : UIElement(parentContainer) {};

	/// <summary>
	/// Virtual destructor for UIButton.
	/// </summary>
	virtual ~UIButton() {};

	/// <summary>
	/// Checks if the button is pressed.
	/// </summary>
	/// <param name="mousebutton">Mouse button to check.</param>
	/// <returns>True if pressed, false otherwise.</returns>
	bool isPressed(MouseButton mousebutton);

	/// <summary>
	/// Checks if the button is held down.
	/// </summary>
	/// <param name="mousebutton">Mouse button to check.</param>
	/// <returns>True if held down, false otherwise.</returns>
	bool isDown(MouseButton mousebutton);

	/// <summary>
	/// Checks if the mouse is over the button.
	/// </summary>
	/// <returns>True if over, false otherwise.</returns>
	virtual bool isOver() = 0;
protected:
	bool isover = false; // Flag indicating if the mouse is over the button.
};

/// <summary>
/// Rectangle UI element.
/// </summary>
class UIRectangle : virtual public UIElement
{
public:
	/// <summary>
	/// Constructs a UIRectangle.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	/// <param name="rec">Rectangle dimensions.</param>
	UIRectangle(UIContainer* parentContainer, Rectangle rec) : UIElement(parentContainer), rec(rec) {};

	virtual ~UIRectangle() {};

	/// <summary>
	/// Draws the rectangle.
	/// </summary>
	/// <param name="alpha">Optional alpha value for transparency.</param>
	virtual void UIElement::draw(float alpha)
	{
		Color color = parentContainer->primaryColor;
		color.a += alpha;
		DrawRectangleRec(rec, color);
	}

	/// <summary>
	/// Sets the position of the rectangle.
	/// </summary>
	/// <param name="pos">New position.</param>
	virtual void UIElement::setPos(const Vec2& pos) override
	{
		rec.x = pos.x - rec.width / 2;
		rec.y = pos.y - rec.height / 2;
	}

	/// <summary>
	/// Gets the position of the rectangle.
	/// </summary>
	/// <returns>Current position.</returns>
	virtual Vec2 UIElement::getPos()
	{
		return Vec2(rec.x + rec.width / 2, rec.y + rec.height / 2);
	}

protected:
	Rectangle rec; // Rectangle dimensions.
};

class UIButtonRec : virtual public UIButton, virtual public UIRectangle
{
public:
	UIButtonRec(UIContainer* parentContainer, Rectangle buttonRec) : UIElement(parentContainer), UIButton(parentContainer), UIRectangle(parentContainer, buttonRec) {}
	virtual ~UIButtonRec() {};

	virtual void UIElement::draw(float alpha)
	{
		if (isOver()) {
			Color color = parentContainer->primaryColor;
			color.a += alpha;
			DrawRectangleRec(rec, color);
		}
		else
		{
			Color color = parentContainer->secondaryColor;
			color.a += alpha;
			DrawRectangleRec(rec, color);
		}
	}

	virtual void UIElement::update() {}

	virtual bool UIButton::isOver();

protected:

};

class UIButtonCircle : public UIButton
{
public:
	UIButtonCircle(UIContainer* parentContainer, Vec2 pos, float radius) : UIElement(parentContainer), UIButton(parentContainer), pos(pos), radius(radius) {};
	~UIButtonCircle() {};

	virtual void UIElement::draw(float alpha)
	{
		if (isOver()) {
			DrawCircleV(pos.vector2(), radius, parentContainer->primaryColor);
		}
		else
		{
			DrawCircleV(pos.vector2(), radius, parentContainer->secondaryColor);
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

class UISlider : public UIButtonRec
{
public:
	UISlider(UIContainer* parentContainer, Rectangle buttonRec, float speed) : UIElement(parentContainer), UIRectangle(parentContainer, buttonRec), UIButton(parentContainer), UIButtonRec(parentContainer, buttonRec), speed(speed)
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
			if (rec.height > rec.width)
			{
				value = (GetMouseY() - rec.y - radius) / range;
			}
			else
			{
				value = (GetMouseX() - rec.x - radius) / range;
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
		UIRectangle::draw(alpha);
		if (rec.height > rec.width)
		{
			DrawCircle(rec.x + radius, rec.y + range * value + radius, radius, parentContainer->primaryColor);
		}
		else
		{
			DrawCircle(rec.x + range * value + radius, rec.y + radius, radius, parentContainer->primaryColor);
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
	UISliderContainer(UIContainer* parentContainer, Rectangle buttonRec, float speed) : UIElement(parentContainer), UIButton(parentContainer), UIRectangle(parentContainer,buttonRec), UISlider(parentContainer, buttonRec, speed), UIContainer(*parentContainer) {};
	~UISliderContainer() {};

	virtual void UIElement::update()
	{
		UISlider::update();
		UIContainer::update();
		findRange();
		float moveAmount = (oldValue - value) * range;
		if (rec.width < rec.height)
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
		if (rec.width < rec.height)
		{
			float drawRange = rec.height / 2;
			for (auto element : childElements)
			{
				element->hidden = abs(getPos().y - element->getPos().y) > drawRange;
			}
		}
		else
		{
			float drawRange = rec.width / 2;
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
		if (rec.width < rec.height)
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
struct is_simple_button : std::false_type {};

template<>
struct is_simple_button<UIButtonRec> : std::true_type {};

template<>
struct is_simple_button<UIButtonCircle> : std::true_type {};

template<typename T>
concept SimpleButton = is_simple_button<T>::value && std::derived_from<T, UIButton>;

template<SimpleButton ButtonType>
class UIFunctionButton : public UIButton, public UIContainer
{
	UIButton* button;
	std::function<void()> function;
public:
	template<typename... Args>
	UIFunctionButton(UIContainer* parentContainer, std::function<void()> func, Args&&... args) : UIElement(parentContainer), UIButton(parentContainer), UIContainer(*parentContainer), function(func)
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

class UIText : virtual public UIElement
{
public:
	UIText(UIContainer* parentContainer) : UIElement(parentContainer) {}
	UIText(UIContainer* parentContainer, std::string text) : UIElement(parentContainer), text(text) {}
	UIText(UIContainer* parentContainer, std::string text, float size) : UIElement(parentContainer), text(text), size(size) {}
	virtual ~UIText() {};

	virtual void update() {}

	inline void SetText(std::string text) { UIText::text = text; }
	inline void SetSize(float size) { UIText::size = size; }

protected:
	float size = 20;
	std::string text;
};

class UITextLine : virtual public UIText
{
public:
	UITextLine(UIContainer* parentContainer, Vec2 pos) : UIElement(parentContainer), UIText(parentContainer), pos(pos) {}
	UITextLine(UIContainer* parentContainer, Vec2 pos, std::string text) : UIElement(parentContainer), UIText(parentContainer, text), pos(pos) {}
	UITextLine(UIContainer* parentContainer, Vec2 pos, std::string text, float size) : UIElement(parentContainer), UIText(parentContainer, text, size), pos(pos) {}

	virtual ~UITextLine() {};

	virtual void draw(float alpha);

	virtual void setPos(const Vec2& pos) { UITextLine::pos = pos; }
	virtual Vec2 getPos() { return pos; }

protected:
	Vec2 pos;

};

class UITextBox : virtual public UIRectangle, virtual public UIText
{
public:
	UITextBox(UIContainer* parentContainer, Rectangle rec) : UIElement(parentContainer), UIRectangle(parentContainer,rec), UIText(parentContainer) {}
	UITextBox(UIContainer* parentContainer, Rectangle rec, std::string text) : UIElement(parentContainer), UIRectangle(parentContainer,rec), UIText(parentContainer, text) {}
	UITextBox(UIContainer* parentContainer, Rectangle rec, std::string text, float size) : UIElement(parentContainer), UIRectangle(parentContainer,rec), UIText(parentContainer, text, size) {}
	virtual ~UITextBox() {};

	virtual void draw(float alpha);
private:
};

template<typename T>
struct is_simple_text : std::false_type {};

template<>
struct is_simple_text<UITextLine> : std::true_type {};

template<>
struct is_simple_text<UITextBox> : std::true_type {};

template<typename T>
concept SimpleText = is_simple_text<T>::value && std::derived_from<T, UIText>;

template<SimpleText TextType>
class UITextInput : virtual public UIText, public TextType, public UIButtonRec
{
public:
	template<typename T, typename... Args>
	UITextInput(UIContainer* parentContainer, T t, Args&&... args) : UIElement(parentContainer), UIRectangle(parentContainer, rec), UIButton(parentContainer), UIText(parentContainer, std::forward<Args>(args)...), UIButtonRec(parentContainer, rec), TextType(parentContainer, t,std::forward<Args>(args)...)
	{
		if constexpr (std::is_same_v<TextType, UITextLine>)
		{
			rec = { t.x - size / 2,t.y,size,size};
		}
		else if constexpr (std::is_same_v<TextType, UITextBox>)
		{
			rec = t;
		}
	}

	virtual ~UITextInput() {};

	void draw(float alpha) override
	{
		UIButtonRec::draw(alpha);
		TextType::draw(alpha);
	}
	void update()
	{
		if (isPressed(MOUSE_LEFT_BUTTON))
		{
			selected = true;
		}
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !isover)
		{
			selected = false;
		}
		if (selected)
		{
			if (IsKeyPressed(KEY_BACKSPACE) && !text.empty())
			{
				text.pop_back();
			}
			else if (KeyboardKey inputkey = static_cast<KeyboardKey>(GetKeyPressed()))
			{
				text.push_back(inputkey);
			}

		}
	}
	void setPos(const Vec2& pos) override
	{
		TextType::setPos(pos);
		UIButtonRec::setPos(pos);
	}
	Vec2 getPos() override
	{
		return UIButtonRec::getPos();
	}
private:
	bool selected = false;
};
