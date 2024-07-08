#pragma once
#include <cmath>
#if !defined (RAYLIB_H)
struct Vector2 {
	Vector2(float x, float y) : x(x), y(y) {}
	float x, y;
};
#endif // !RAYLIB_H


class Vec2
{
public:
	float x = 0;
	float y = 0;

	Vec2() {};
	Vec2(const Vector2& vector2) : x(vector2.x), y(vector2.y) {};
	Vec2(float x, float y) : x(x), y(y) {};
	//overloaded + operator
	Vec2 operator + (const Vec2& rhs) {
		return Vec2(x + rhs.x, y + rhs.y);
	}
	//overloaded += operator
	void operator += (const Vec2& rhs) {
		x += rhs.x;
		y += rhs.y;
	}
	//overloaded - operator
	Vec2 operator - (const Vec2& rhs) {
		return Vec2(x - rhs.x, y - rhs.y);
	}
	//overloaded -= operator
	void operator -= (const Vec2& rhs) {
		x -= rhs.x;
		y -= rhs.y;
	}
	//overloaded * operator
	Vec2 operator * (float rhs) {
		return Vec2(x * rhs, y * rhs);
	}
	//overloaded *= operator
	void operator *= (float rhs) {
		x *= rhs;
		y *= rhs;
	}
	//overloaded * operator
	Vec2 operator * (Vec2 rhs) const {
		return Vec2(x * rhs.x, y * rhs.y);
	}
	//overloaded *= operator
	void operator *= (Vec2 rhs) {
		x *= rhs.x;
		y *= rhs.y;
	}
	//overloaded / operator
	Vec2 operator / (float rhs) {
		return Vec2(x / rhs, y / rhs);
	}
	// -Vec2 operator
	Vec2 operator -() const {
		return Vec2(-x, -y);
	}
	//overloaded /= operator
	void operator /= (float rhs) {
		x /= rhs;
		y /= rhs;
	}
	//overloaded == operator
	bool operator == (const Vec2& rhs) {
		return x == rhs.x && y == rhs.y;
	}
	//overloaded != operator
	bool operator != (const Vec2& rhs) {
		return x != rhs.x && y != rhs.y;	
	}

	//length function
	float Length() {
		return sqrt(x * x + y * y);
	}

	//Normalize function
	Vec2 Normalize () {
		float length = Length();
		if(length != 0)
			return Vec2(x / length, y / length);
		return Vec2(0, 0);
	}
	//Perpendicular function
	Vec2 Perpendicular() const
	{
		return Vec2(-y, x);
	}
	//dotproduct function
	float dot(const Vec2& rhs) {
		return x * rhs.x + y * rhs.y;
	}
	//crossproduct function
	float Cross(const Vec2& rhs) {
		return x * rhs.y - y * rhs.x;
	}
	//raylibVector2
	Vector2 vector2() {
		return { x,y };
	}
};