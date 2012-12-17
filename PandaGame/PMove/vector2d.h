#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <d3dx9.h>

class Vector2D
{
public:
    double x;
    double y;

    Vector2D():x(0.0),y(0.0) {}
    Vector2D(double a, double b):x(a),y(b) {}
	Vector2D(D3DXVECTOR2 v){x = v.x; y = v.y;}
     void Zero(); //sets x and y to zero
     bool isZero()const;  //returns true if both x and y are zero
     double Length()const; //returns the length of the vector
     double LengthSq()const;  //returns the squared length of the vector (thereby avoiding the sqrt)
     void Normalize();
     double Dot(const Vector2D& v2)const;  //returns the dot product of this and v2
     Vector2D Perp()const;  //returns the vector that is perpendicular to this one
     void Truncate(double max); //adjusts x and y so that the length of the vector does not exceed max
     double Distance(const Vector2D &v2)const;  //returns the distance between this vector and the one passed as a parameter
     double DistanceSq(const Vector2D &v2)const; //squared version of above
     Vector2D GetReverse()const; //returns the vector that is the reverse of this vector

    //some operators
    const Vector2D& operator+=(const Vector2D &rhs);
    const Vector2D& operator-=(const Vector2D &rhs);
    const Vector2D& operator*=(const double& rhs);
    const Vector2D& operator/=(const double& rhs);
    bool operator==(const Vector2D& rhs)const;
    bool operator!=(const Vector2D& rhs)const;
    const Vector2D& operator+(const Vector2D &rhs);
    const Vector2D& operator-(const Vector2D &rhs);
    const Vector2D& operator*(const double& rhs);
    const Vector2D& operator/(const double& rhs);
};

#endif // VECTOR2D_H
