#ifndef MOVINGENTITY_H
#define MOVINGENTITY_H

#include "vector2d.h"

class MovingEntity
{
public:
	MovingEntity()
	{
		m_dMass = 1.0f;
		//m_dMaxSpeed = 0.7f;
		m_dMaxSpeed = 0.3f;
		m_dMaxForce = 1.0f;
		m_vHeading = Vector2D((50 - rand() % 100) / 100.0f, (50 - rand() % 100) / 100.0f);
		m_vHeading.Normalize();
		m_vVelocity = m_vHeading;
		m_vSide = m_vHeading.Perp();
	}

    Vector2D m_vVelocity;
    Vector2D m_vHeading;
    Vector2D m_vSide;
    Vector2D m_vSteeringForce;
	Vector2D m_vPosition;

    double m_dMass;
    double m_dMaxSpeed;
    double m_dMaxForce;
    double m_dMaxTurnRate;

    virtual void Update() {}

};

#endif // MOVINGENTITY_H
