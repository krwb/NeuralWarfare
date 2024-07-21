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
	void draw(float alpha = 0)
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
	std::list<UIElement*> childElements; // List of child elements.

protected:
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
		UIContainer::draw(alpha);
	}

	/// <summary>
	/// Updates the UISubContainer and its elements.
	/// </summary>
	void UIElement::update()
	{
		UIContainer::update();
	}

	/// <summary>
	/// Gets the position of the UI element.
	/// </summary>
	/// <returns>Current position.</returns>
	virtual Vec2 getPos()
	{
		Vec2 pos = { 0,0 };
		for (UIElement* child : childElements)
		{
			pos += child->getPos();
		}
		pos /= childElements.size();
		return pos;
	}

	/// <summary>
	/// Sets the position of the UI element.
	/// </summary>
	/// <param name="pos">New position.</param>
	virtual void setPos(const Vec2& pos)
	{
		Vec2 move = getPos() - pos;
		for (UIElement* child : childElements)
		{
			child->setPos(child->getPos() - move);
		}
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
		Color color = parentContainer->secondaryColor;
		float ta = static_cast<float>(color.a) + alpha;
		color.a = ta > 0 ? ta < 255 ? ta : 255 : 0;
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

	virtual void update() {};

protected:
	Rectangle rec; // Rectangle dimensions.
};

/// <summary>
/// Rectangle button UI element.
/// </summary>
class UIButtonRec : virtual public UIButton, virtual public UIRectangle
{
public:
	/// <summary>
	/// Constructs a UIButtonRec.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	/// <param name="buttonRec">Button rectangle dimensions.</param>
	UIButtonRec(UIContainer* parentContainer, Rectangle buttonRec) : UIElement(parentContainer), UIButton(parentContainer), UIRectangle(parentContainer, buttonRec) {}
	virtual ~UIButtonRec() {};

	/// <summary>
	/// Draws the button.
	/// </summary>
	/// <param name="alpha">Optional alpha value for transparency.</param>
	virtual void UIElement::draw(float alpha)
	{
		if (isOver()) 
		{
			Color color = parentContainer->primaryColor;
			float ta = static_cast<float>(color.a) + alpha;
			color.a = ta > 0 ? ta < 255 ? ta : 255 : 0;
			DrawRectangleRec(rec, color);
		}
		else
		{
			Color color = parentContainer->secondaryColor;
			float ta = static_cast<float>(color.a) + alpha;
			color.a = ta > 0 ? ta < 255 ? ta : 255 : 0;
			DrawRectangleRec(rec, color);
		}
	}

	/// <summary>
	/// Updates the button state.
	/// </summary>
	virtual void UIElement::update() {}

	/// <summary>
	/// Checks if the mouse is over the button.
	/// </summary>
	/// <returns>True if the mouse is over the button, false otherwise.</returns>
	virtual bool UIButton::isOver();

protected:

};

/// <summary>
/// Circle button UI element.
/// </summary>
class UIButtonCircle : public UIButton
{
public:
	/// <summary>
	/// Constructs a UIButtonCircle.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	/// <param name="pos">Position of the button.</param>
	/// <param name="radius">Radius of the button.</param>
	UIButtonCircle(UIContainer* parentContainer, Vec2 pos, float radius) : UIElement(parentContainer), UIButton(parentContainer), pos(pos), radius(radius) {};
	~UIButtonCircle() {};

	/// <summary>
	/// Draws the button.
	/// </summary>
	/// <param name="alpha">Optional alpha value for transparency.</param>
	virtual void UIElement::draw(float alpha)
	{
		if (isOver()) 
		{
			DrawCircleV(pos.vector2(), radius, parentContainer->primaryColor);
		}
		else
		{
			DrawCircleV(pos.vector2(), radius, parentContainer->secondaryColor);
		}
	}

	/// <summary>
	/// Updates the button state.
	/// </summary>
	virtual void UIElement::update() {};

	/// <summary>
	/// Checks if the mouse is over the button.
	/// </summary>
	/// <returns>True if the mouse is over the button, false otherwise.</returns>
	virtual bool UIButton::isOver();

	/// <summary>
	/// Sets the position of the button.
	/// </summary>
	/// <param name="pos">New position.</param>
	virtual void UIElement::setPos(const Vec2& pos)
	{
		UIButtonCircle::pos = pos;
	}

	/// <summary>
	/// Gets the position of the button.
	/// </summary>
	/// <returns>Current position.</returns>
	virtual Vec2 UIElement::getPos()
	{
		return pos;
	}

protected:
	Vec2 pos; // Position of the button.
	float radius; // Radius of the button.
};

/// <summary>
/// Slider UI element.
/// </summary>
class UISlider : public UIButtonRec
{
public:
	/// <summary>
	/// Constructs a UISlider.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	/// <param name="buttonRec">Slider button rectangle dimensions.</param>
	/// <param name="speed">Speed of the slider movement.</param>
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

	/// <summary>
	/// Updates the slider state.
	/// </summary>
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

	/// <summary>
	/// Draws the slider.
	/// </summary>
	/// <param name="alpha">Optional alpha value for transparency.</param>
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

	float value = 0; // Slider value.
	float speed; // Speed of the slider movement.
	bool griped = false; // Indicates if the slider is being gripped.
private:
	float radius; // Radius of the slider button.
	float range; // Range of the slider movement.
};

/// <summary>
/// Slider container UI element.
/// </summary>
class UISliderContainer : public UIContainer, public UISlider
{
public:
	/// <summary>
	/// Constructs a UISliderContainer.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	/// <param name="buttonRec">Slider button rectangle dimensions.</param>
	/// <param name="speed">Speed of the slider movement.</param>
	UISliderContainer(UIContainer* parentContainer, Rectangle buttonRec, float speed)
		: UIElement(parentContainer), UIButton(parentContainer), UIRectangle(parentContainer, buttonRec), UISlider(parentContainer, buttonRec, speed), UIContainer(*parentContainer)
	{
		findRange();
	}
	~UISliderContainer() {};

	/// <summary>
	/// Updates the slider container state.
	/// </summary>
	virtual void UIElement::update() override
	{
		UISlider::update();
		UIContainer::update();
		findRange();
		for (auto element : childElements)
		{
			adjustChildPosition(element);
		}
		oldValue = value;
	}

	/// <summary>
	/// Draws the slider container.
	/// </summary>
	/// <param name="alpha">Optional alpha value for transparency.</param>
	virtual void UIElement::draw(float alpha) override
	{
		UISlider::draw(alpha);
		float drawRange;
		bool vertical = rec.width < rec.height;
		if (vertical)
		{
			drawRange = rec.height / 2;
		}
		else
		{
			drawRange = rec.width / 2;
		}

		for (auto element : childElements)
		{
			float distance = vertical ? abs(getPos().y - element->getPos().y) : abs(getPos().x - element->getPos().x);
			if (distance > drawRange)
			{
				element->hidden = true;
			}
			else
			{
				element->hidden = false;
				float fadeFactor = (distance - drawRange * 0.8) / (drawRange * 0.2);
				float fadeAlpha = 255 * (fadeFactor > 0 ? fadeFactor < 1 ? fadeFactor : 1 : 0);
				element->draw(-fadeAlpha);
			}
		}
	}

private:
	float oldValue = 0; // Previous slider value.
	float range = 0; // Range of the slider movement.

	/// <summary>
	/// Finds the range of the slider based on the positions of child elements.
	/// </summary>
	void findRange()
	{
		if (childElements.empty())
		{
			range = 0;
			return;
		}

		float firstElementPos = INFINITY;
		float lastElementPos = - INFINITY;
		if (rec.width < rec.height)
		{
			firstElementPos = lastElementPos = childElements.front()->getPos().y;
			for (auto element : childElements)
			{
				float elementPos = element->getPos().y;
				if (elementPos < firstElementPos)
				{
					firstElementPos = elementPos;
				}
				if (elementPos > lastElementPos)
				{
					lastElementPos = elementPos;
				}
			}
		}
		else
		{
			firstElementPos = lastElementPos = childElements.front()->getPos().x;
			for (auto element : childElements)
			{
				float elementPos = element->getPos().x;
				if (elementPos < firstElementPos)
				{
					firstElementPos = elementPos;
				}
				if (elementPos > lastElementPos)
				{
					lastElementPos = elementPos;
				}
			}
		}
		range = lastElementPos - firstElementPos;
	}

	/// <summary>
	/// Adds a child element and adjusts its position.
	/// </summary>
	/// <param name="element">Pointer to the new child element.</param>
	void add(UIElement* element) override
	{
		UIContainer::add(element); // Call base class method
	}

	/// <summary>
	/// Adjusts the position of a newly added child element based on the current slider value.
	/// </summary>
	/// <param name="element">Pointer to the new child element.</param>
	void adjustChildPosition(UIElement* element)
	{
		float moveAmount = (oldValue - value) * range;
		Vec2 newpos = element->getPos();
		if (rec.width < rec.height)
		{
			newpos.y += moveAmount;
		}
		else
		{
			newpos.x += moveAmount;
		}
		element->setPos(newpos);
	}
};

/// <summary>
/// Trait to check if a type is a simple button.
/// </summary>
template<typename T>
struct is_simple_button : std::false_type {};

/// <summary>
/// Specialization of is_simple_button for UIButtonRec.
/// </summary>
template<>
struct is_simple_button<UIButtonRec> : std::true_type {};

/// <summary>
/// Specialization of is_simple_button for UIButtonCircle.
/// </summary>
template<>
struct is_simple_button<UIButtonCircle> : std::true_type {};

/// <summary>
/// Concept to enforce a type to be a SimpleButton.
/// </summary>
template<typename T>
concept SimpleButton = is_simple_button<T>::value && std::derived_from<T, UIButton>;

/// <summary>
/// Function button UI element.
/// </summary>
template<SimpleButton ButtonType>
class UIFunctionButton : public UIButton, public UIContainer
{
	UIButton* button; // Pointer to the button.
	std::function<void()> function; // Function to be executed on button press.
public:
	/// <summary>
	/// Constructs a UIFunctionButton.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	/// <param name="func">Function to be executed on button press.</param>
	/// <param name="args">Arguments to construct the button.</param>
	template<typename... Args>
	UIFunctionButton(UIContainer* parentContainer, std::function<void()> func, Args&&... args) : UIElement(parentContainer), UIButton(parentContainer), UIContainer(*parentContainer), function(func)
	{
		button = new ButtonType(this, std::forward<Args>(args)...);
	}

	/// <summary>
	/// Destructor for UIFunctionButton.
	/// </summary>
	~UIFunctionButton()
	{
		delete button;
	}

	/// <summary>
	/// Updates the function button state.
	/// </summary>
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

	/// <summary>
	/// Draws the function button.
	/// </summary>
	/// <param name="alpha">Optional alpha value for transparency.</param>
	virtual void UIElement::draw(float alpha)
	{
		button->draw(alpha);
	}

	/// <summary>
	/// Sets the position of the function button.
	/// </summary>
	/// <param name="pos">New position.</param>
	virtual void UIElement::setPos(const Vec2& pos)
	{
		button->setPos(pos);
	}

	/// <summary>
	/// Gets the position of the function button.
	/// </summary>
	/// <returns>Current position.</returns>
	virtual Vec2 UIElement::getPos()
	{
		return button->getPos();
	}

	/// <summary>
	/// Checks if the mouse is over the button.
	/// </summary>
	/// <returns>True if the mouse is over the button, false otherwise.</returns>
	virtual bool UIButton::isOver()
	{
		return button->isOver();
	}

private:

};

/// <summary>
/// Text UI element.
/// </summary>
class UIText : virtual public UIElement
{
public:
	/// <summary>
	/// Constructs a UIText.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	UIText(UIContainer* parentContainer) : UIElement(parentContainer) {}

	/// <summary>
	/// Constructs a UIText with specified text.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	/// <param name="text">Text to display.</param>
	UIText(UIContainer* parentContainer, std::string text) : UIElement(parentContainer), text(text) {}

	/// <summary>
	/// Constructs a UIText with specified text and size.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	/// <param name="text">Text to display.</param>
	/// <param name="size">Size of the text.</param>
	UIText(UIContainer* parentContainer, std::string text, float size) : UIElement(parentContainer), text(text), size(size) {}

	virtual ~UIText() {};

	virtual void update() {}

	/// <summary>
	/// Sets the text.
	/// </summary>
	/// <param name="text">Text to display.</param>
	inline void SetText(std::string text) { UIText::text = text; }
	
	inline std::string GetText() { return text; }


	/// <summary>
	/// Sets the size of the text.
	/// </summary>
	/// <param name="size">Size of the text.</param>
	inline void SetSize(float size) { UIText::size = size; }

protected:
	float size = 20; // Size of the text.
	std::string text; // Text to display.
};

/// <summary>
/// Text line UI element.
/// </summary>
class UITextLine : virtual public UIText
{
public:
	/// <summary>
	/// Constructs a UITextLine with specified position.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	/// <param name="pos">Position of the text.</param>
	UITextLine(UIContainer* parentContainer, Vec2 pos) : UIElement(parentContainer), UIText(parentContainer), pos(pos) {}

	/// <summary>
	/// Constructs a UITextLine with specified position and text.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	/// <param name="pos">Position of the text.</param>
	/// <param name="text">Text to display.</param>
	UITextLine(UIContainer* parentContainer, Vec2 pos, std::string text) : UIElement(parentContainer), UIText(parentContainer, text), pos(pos) {}

	/// <summary>
	/// Constructs a UITextLine with specified position, text, and size.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	/// <param name="pos">Position of the text.</param>
	/// <param name="text">Text to display.</param>
	/// <param name="size">Size of the text.</param>
	UITextLine(UIContainer* parentContainer, Vec2 pos, std::string text, float size) : UIElement(parentContainer), UIText(parentContainer, text, size), pos(pos) {}

	virtual ~UITextLine() {};

	/// <summary>
	/// Draws the text line.
	/// </summary>
	/// <param name="alpha">Optional alpha value for transparency.</param>
	virtual void draw(float alpha);

	/// <summary>
	/// Sets the position of the text line.
	/// </summary>
	/// <param name="pos">New position.</param>
	virtual void setPos(const Vec2& pos) { UITextLine::pos = pos; }

	/// <summary>
	/// Gets the position of the text line.
	/// </summary>
	/// <returns>Current position.</returns>
	virtual Vec2 getPos() { return pos; }

protected:
	Vec2 pos; // Position of the text.
};

/// <summary>
/// Text box UI element.
/// </summary>
class UITextBox : virtual public UIRectangle, virtual public UIText
{
public:
	/// <summary>
	/// Constructs a UITextBox with specified rectangle dimensions.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	/// <param name="rec">Rectangle dimensions.</param>
	UITextBox(UIContainer* parentContainer, Rectangle rec) : UIElement(parentContainer), UIRectangle(parentContainer, rec), UIText(parentContainer) {}

	/// <summary>
	/// Constructs a UITextBox with specified rectangle dimensions and text.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	/// <param name="rec">Rectangle dimensions.</param>
	/// <param name="text">Text to display.</param>
	UITextBox(UIContainer* parentContainer, Rectangle rec, std::string text) : UIElement(parentContainer), UIRectangle(parentContainer, rec), UIText(parentContainer, text) {}

	/// <summary>
	/// Constructs a UITextBox with specified rectangle dimensions, text, and size.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	/// <param name="rec">Rectangle dimensions.</param>
	/// <param name="text">Text to display.</param>
	/// <param name="size">Size of the text.</param>
	UITextBox(UIContainer* parentContainer, Rectangle rec, std::string text, float size) : UIElement(parentContainer), UIRectangle(parentContainer, rec), UIText(parentContainer, text, size) {}

	virtual ~UITextBox() {};

	virtual void update() { UIRectangle::update(); UIText::update(); }

	/// <summary>
	/// Draws the text box.
	/// </summary>
	/// <param name="alpha">Optional alpha value for transparency.</param>
	virtual void draw(float alpha);
private:
};

class UIBackgroundlessTextBox : public UITextBox
{
public:
	/// <summary>
	/// Constructs a DerivedUITextBox with specified rectangle dimensions.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	/// <param name="rec">Rectangle dimensions.</param>
	UIBackgroundlessTextBox(UIContainer* parentContainer, Rectangle rec) : UIElement(parentContainer), UIRectangle(parentContainer, rec), UIText(parentContainer),  UITextBox(parentContainer, rec) {}

	UIBackgroundlessTextBox(UIContainer* parentContainer, Rectangle rec, std::string text) : UIElement(parentContainer), UIRectangle(parentContainer, rec), UIText(parentContainer, text), UITextBox(parentContainer, rec, text) {}

	UIBackgroundlessTextBox(UIContainer* parentContainer, Rectangle rec, std::string text, float size) : UIElement(parentContainer), UIRectangle(parentContainer, rec), UIText(parentContainer, text, size), UITextBox(parentContainer, rec, text, size) {}

	virtual ~UIBackgroundlessTextBox() {}

	virtual void draw(float alpha) override;
};

/// <summary>
/// Trait to check if a type is a simple text.
/// </summary>
template<typename T>
struct is_simple_text : std::false_type {};

/// <summary>
/// Specialization of is_simple_text for UITextLine.
/// </summary>
template<>
struct is_simple_text<UITextLine> : std::true_type {};

/// <summary>
/// Specialization of is_simple_text for UITextBox.
/// </summary>
template<>
struct is_simple_text<UITextBox> : std::true_type {};

/// <summary>
/// Specialization of is_simple_text for UIBackgroundlessTextBox.
/// </summary>
template<>
struct is_simple_text<UIBackgroundlessTextBox> : std::true_type {};

/// <summary>
/// Concept to enforce a type to be a SimpleText.
/// </summary>
template<typename T>
concept SimpleText = is_simple_text<T>::value && std::derived_from<T, UIText>;

/// <summary>
/// Text input UI element.
/// </summary>
template<SimpleText TextType>
class UITextInput : virtual public UIText, public TextType, public UIButtonRec
{
public:
	/// <summary>
	/// Constructs a UITextInput.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	/// <param name="t">Position or rectangle dimensions.</param>
	/// <param name="args">Additional arguments for the text element.</param>
	template<typename T, typename... Args>
	UITextInput(UIContainer* parentContainer, T t, Args&&... args) : UIElement(parentContainer), UIRectangle(parentContainer, rec), UIButton(parentContainer), UIText(parentContainer, std::forward<Args>(args)...), UIButtonRec(parentContainer, rec), TextType(parentContainer, t, std::forward<Args>(args)...)
	{
		if constexpr (std::is_same_v<UITextLine, UITextBox>)
		{
			rec = { t.x - size / 2, t.y, size, size };
		}
		else if constexpr (std::is_base_of_v<UITextBox, TextType>)
		{
			rec = t;
		}
	}

	virtual ~UITextInput() {};

	/// <summary>
	/// Draws the text input.
	/// </summary>
	/// <param name="alpha">Optional alpha value for transparency.</param>
	void draw(float alpha) override
	{
		UIButtonRec::draw(alpha);
		TextType::draw(alpha);
	}

	/// <summary>
	/// Updates the text input state.
	/// </summary>
	void update()
	{
		UIButtonRec::update();
		isSubmited = false;
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
			else if (IsKeyPressed(KEY_ENTER) && !text.empty())
			{
				isSubmited = true;
				selected = false;
			}
			else if (KeyboardKey inputkey = static_cast<KeyboardKey>(GetKeyPressed()))
			{
				text.push_back(inputkey);
			}

		}
	}

	bool IsSubmited()
	{
		return isSubmited;
	}

	/// <summary>
	/// Sets the position of the text input.
	/// </summary>
	/// <param name="pos">New position.</param>
	void setPos(const Vec2& pos) override
	{
		TextType::setPos(pos);
		UIButtonRec::setPos(pos);
	}

	/// <summary>
	/// Gets the position of the text input.
	/// </summary>
	/// <returns>Current position.</returns>
	Vec2 getPos() override
	{
		return UIButtonRec::getPos();
	}
	bool selected = false; // Indicates if the text input is selected.

private:
	bool isSubmited = false;
};

/// <summary>
/// Live updating text UI element.
/// </summary>
template<SimpleText TextType>
class UILiveText : public TextType
{
public:
	template<typename T, typename... Args>
	UILiveText(UIContainer* parentContainer, std::function<std::string()> function , T t, Args&&... args) : UIElement(parentContainer), UIText(parentContainer, std::forward<Args>(args)...), TextType(parentContainer, t, std::forward<Args>(args)...), function(function)
	{

	}
	virtual ~UILiveText() {}

	virtual void update() override
	{
		TextType::update();
		TextType::SetText(function());
	}
private:
	std::function<std::string()> function; // Function to get new text
};

template<typename T>
concept Button = std::derived_from<T, UIButton>;

template<Button ButtonType>
class UILabeledButton : public UIButton, public UIContainer, public UITextLine
{
	UIButton* button; // Pointer to the button.
public:
	/// <summary>
	/// Constructs a UIFunctionButton.
	/// </summary>
	/// <param name="parentContainer">Pointer to the parent container.</param>
	/// <param name="text">Label text to be displayed.</param>
	/// <param name="size">Size of the displayed text.</param>
	/// <param name="args">Arguments to construct the button.</param>
	template<typename... Args>
	UILabeledButton(UIContainer* parentContainer, std::string text, float size, Args&&... args) : UIElement(parentContainer), UIButton(parentContainer), UIContainer(*parentContainer), UIText(parentContainer, text, size), UITextLine(parentContainer, {})
	{
		button = new ButtonType(this, std::forward<Args>(args)...);
		UITextLine::setPos(button->getPos());
	}

	/// <summary>
	/// Destructor for UIFunctionButton.
	/// </summary>
	~UILabeledButton()
	{
		delete button;
	}

	/// <summary>
	/// Updates the function button state.
	/// </summary>
	virtual void UIElement::update()
	{
		if (button)
		{
			button->update();
		}
		else
		{
			delete this;
		}
		UITextLine::update();
	}

	/// <summary>
	/// Draws the function button.
	/// </summary>
	/// <param name="alpha">Optional alpha value for transparency.</param>
	virtual void UIElement::draw(float alpha)
	{
		button->draw(alpha);
		UITextLine::draw(alpha);
	}

	/// <summary>
	/// Sets the position of the function button.
	/// </summary>
	/// <param name="pos">New position.</param>
	virtual void UIElement::setPos(const Vec2& pos)
	{
		button->setPos(pos);
		UITextLine::setPos(pos);
	}

	/// <summary>
	/// Gets the position of the function button.
	/// </summary>
	/// <returns>Current position.</returns>
	virtual Vec2 UIElement::getPos()
	{
		return button->getPos();
	}

	/// <summary>
	/// Checks if the mouse is over the button.
	/// </summary>
	/// <returns>True if the mouse is over the button, false otherwise.</returns>
	virtual bool UIButton::isOver()
	{
		return button->isOver();
	}

private:

};
