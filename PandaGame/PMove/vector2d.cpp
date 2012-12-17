#include "vector2d.h"
#include <math.h>
#include <iostream>

 void Vector2D::Zero() {
    x = 0.0;
    y = 0.0;
}

 bool Vector2D::isZero()const {
    if (x == 0.0 && y == 0.0)
        return true;
    return false;
}

 double Vector2D::Length()const {
    return (sqrt( x*x + y*y ));
}

 double Vector2D::LengthSq()const {
    return ( x*x + y*y );
}

 void Vector2D::Normalize() {
	if (isZero())
		return;
	if (x > 1000000.0f || y > 1000000.0f)
	{
	//	std::cout << "error" << std::endl;
	}
    double len = Length();
    x = x / len;
    y = y / len;
}

 double Vector2D::Dot(const Vector2D& v2)const {
    return (x * v2.x + y * v2.y);
}

 Vector2D Vector2D::Perp()const {
    return Vector2D(-y, x);
}

 void Vector2D::Truncate(double max) {
    if (Length() > max) {
        Normalize();
        x = x * max;
        y = y * max;
    }
}

 double Vector2D::Distance(const Vector2D &v2)const {
    return (sqrt((x - v2.x) * (x - v2.x) + (y - v2.y) * (y - v2.y)));
}

 double Vector2D::DistanceSq(const Vector2D &v2)const {
    return ((x - v2.x) * (x - v2.x) + (y - v2.y) * (y - v2.y));
}

 Vector2D Vector2D::GetReverse()const {
    return Vector2D(-x, -y);
}

const Vector2D& Vector2D::operator+=(const Vector2D &rhs) {
    x = x + rhs.x;
    y = y + rhs.y;
    return *this;
}

const Vector2D& Vector2D::operator-=(const Vector2D &rhs) {
    x = x - rhs.x;
    y = y - rhs.y;
    return *this;
}

const Vector2D& Vector2D::operator*=(const double& rhs) {
    x = x * rhs;
    y = y * rhs;
    return *this;
}


const Vector2D& Vector2D::operator/=(const double& rhs) {
	if (rhs == 0)
	{
		//std::cout << "error" << std::endl;
	}

    x = x / rhs;
    y = y / rhs;
    return *this;
}


bool Vector2D::operator==(const Vector2D& rhs)const {
    if (x == rhs.x && y == rhs.y)
        return true;
    return false;
}


bool Vector2D::operator!=(const Vector2D& rhs)const {
    if (x != rhs.x || y != rhs.y)
        return true;
    return false;
}

const Vector2D& Vector2D::operator+(const Vector2D &rhs) {
    x = x + rhs.x;
    y = y + rhs.y;
    return *this;
}

const Vector2D& Vector2D::operator-(const Vector2D &rhs) {
    x = x - rhs.x;
    y = y - rhs.y;
    return *this;
}

const Vector2D& Vector2D::operator*(const double& rhs) {
    x = x * rhs;
    y = y * rhs;
    return *this;
}

const Vector2D& Vector2D::operator/(const double& rhs) {
    x = x / rhs;
    y = y / rhs;
    return *this;
}

