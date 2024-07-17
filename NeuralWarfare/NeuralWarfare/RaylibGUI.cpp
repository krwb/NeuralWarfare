#include "RaylibGUI.h"

UIElement::UIElement(UIContainer* parentContainer) : parentContainer(parentContainer)
{
	parentContainer->add(this);
}

UIElement::~UIElement()
{
	parentContainer->remove(this);
}

void UIContainer::add(UIElement* element)
{
	childElements.push_back(element);
}

void UIContainer::remove(UIElement* element)
{
	childElements.remove(element);
}

bool UIButton::isPressed(MouseButton mousebutton) {
	if (isOver()) {
		if (IsMouseButtonPressed(mousebutton)) {
			return true;
		}
	}
	return false;
}

bool UIButton::isDown(MouseButton mousebutton) {
	if (IsMouseButtonDown(mousebutton)) {
		return isOver();
	}
	return false;
}

bool UIButtonRec::isOver() {
	Vector2 mPos = GetMousePosition();
	if (mPos.x > rec.x &&
		mPos.y > rec.y &&
		mPos.x < rec.x + rec.width &&
		mPos.y < rec.y + rec.height) {
		isover = true;
		return isover;
	}
	isover = false;
	return isover;
}

bool UIButtonCircle::isOver() {
	isover = (Vec2(GetMousePosition()) - pos).Length() < radius;
	return isover;
}

void UITextLine::draw(float alpha)
{
	Color color = parentContainer->textColor;
	color.a += alpha;
	DrawText(text.c_str(), pos.x - MeasureText(text.c_str(), size)/2, pos.y - size/2, size, color);
}

void UITextBox::draw(float alpha)
{
	Color color = parentContainer->secondaryColor;
	color.a += alpha;
	DrawRectangleRec(rec, color);
	DrawTextRec(GetFontDefault(), text.c_str(), {rec.x + size/2,rec.y + size/2, rec.width - size / 2,rec.height - size / 2 }, size, size / 5, true, parentContainer->textColor);
}
