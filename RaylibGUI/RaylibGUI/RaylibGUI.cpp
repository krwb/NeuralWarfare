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
	if (mPos.x > buttonRec.x &&
		mPos.y > buttonRec.y &&
		mPos.x < buttonRec.x + buttonRec.width &&
		mPos.y < buttonRec.y + buttonRec.height) {
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
