#include "steeringbehaviors.h"
#include "..\pandaTheGame.h"

// need for wander
#include <time.h>
#include <iostream>
using namespace std;

SteeringBehaviors::SteeringBehaviors(PTGBot* gPanda) 
{
	mPanda = gPanda;
	busy = 0;

	R = 10.0 * 10.0; // Neighbors R
	BRadius = 1.2;
	TRadius = 0.1;
	m_vWanderTarget = Vector2D(0,0);
	m_dWanderRadius = 2;
	m_dWanderDistance = 5;
	m_dWanderJitter = 0.5;
	doSeek = 0;
    doFlee = 0;
    doArrive = 0;
    doWander = 0;
	doSeparation = 1;
	doTreeSeparation = 1;
	doAlignment = 0;
	doCohesion = 0;
}

Vector2D SteeringBehaviors::Seek() 
{
	Vector2D DesiredVelocity = m_vTarget - Vector2D(mPanda->getPos());

	DesiredVelocity.Normalize();
	DesiredVelocity*=  mPanda->mForces.m_dMaxSpeed;

	return (DesiredVelocity - mPanda->mForces.m_vVelocity);
}

Vector2D SteeringBehaviors::Flee() 
{
	//const double PanicDistanceSq = 10.0 * 10.0;

	/*
	if (Vector2D(mPanda->getPos()).DistanceSq(m_vFleeTarget) > PanicDistanceSq)
	{
		return Vector2D(0,0);
	}
	*/

	Vector2D DesiredVelocity =  (Vector2D(mPanda->getPos()) - m_vFleeTarget);
	DesiredVelocity.Normalize();
	DesiredVelocity*= mPanda->mForces.m_dMaxSpeed;

	return (DesiredVelocity - mPanda->mForces.m_vVelocity);
}
/*
Vector2D SteeringBehaviors::Evade()
{
	Vector2D ToPursuer = m_vFleeTarget - ->Pos();
	double LookAheadTime = ToPursuer.Length() / (mPanda->mForces.MaxSpeed + mPanda->mForces.MaxSpeed);
	return Flee(pursuer->Pos() + pursuer->Velocity() * LookAheadTime);
}
*/

Vector2D SteeringBehaviors::Arrive(Deceleration deceleration) 
{
	Vector2D ToTarget = m_vTarget - Vector2D(mPanda->getPos());
	double dist = ToTarget.Length();
	if (dist > 0) 
	{
		const double DecelerationTweaker = 0.3;
		double speed = dist / ((double)deceleration * DecelerationTweaker);

		if (speed > mPanda->mForces.m_dMaxSpeed)
			speed = mPanda->mForces.m_dMaxSpeed;

		Vector2D DesiredVelocity = ToTarget * (speed / dist);

		return (DesiredVelocity - mPanda->mForces.m_vVelocity);
	}
	return Vector2D(0,0);
}

double SteeringBehaviors::getRand(double b, double e) 
{
   return b + (rand() % static_cast<unsigned int>((e - b) * 1000)) / 1000.0f;
}

Vector2D SteeringBehaviors::Wander() 
{
	m_vWanderTarget += Vector2D(getRand(-1,1) * m_dWanderJitter, getRand(-1,1) * m_dWanderJitter);
	m_vWanderTarget.Normalize();
	m_vWanderTarget *= m_dWanderRadius;

	Vector2D targetLocal = m_vWanderTarget + Vector2D(m_dWanderDistance, 0);
	//Vector2D targetWorld = Vector2D(targetLocal.x * mPanda->mForces.m_vHeading.Dot(Vector2D(1, 0)) - targetLocal.y * mPanda->mForces.m_vSide.Dot(Vector2D(0, 1)),
	//	targetLocal.x * mPanda->mForces.m_vSide.Dot(Vector2D(0, 1)) + mPanda->mForces.m_vHeading.Dot(Vector2D(1, 0)) * targetLocal.y); 
	//Vector2D targetWorld; //= PointToWorldSpace(targetLocal, mPanda->m_vHeading, mPanda->m_vSide, mPanda->m_vPosition);
	Vector2D targetWorld;
	targetWorld.x =  (mPanda->mForces.m_vHeading.x) * targetLocal.x - (mPanda->mForces.m_vHeading.y) * targetLocal.y;
	targetWorld.y =  (mPanda->mForces.m_vHeading.y) * targetLocal.x + (mPanda->mForces.m_vHeading.x) * targetLocal.y;

	return targetWorld;
	//return targetWorld - Vector2D(mPanda->getPos());
}

double SteeringBehaviors::getFleeLength()
{
	Vector2D ToTarget = m_vFleeTarget - Vector2D(mPanda->getPos());
	return ToTarget.LengthSq();
}
/*
Vector2D SteeringBehaviors::GetHidingPosition(Vector2D& posOb, double radiusOb) {
	const double DistanceFromBoundary = 30.0;
	double DistAway = radiusOb + DistanceFromBoundary;
	Vector2D help = posOb - posTarget;
	help.Normalize();
	Vector2D ToOb = help;
	help = (ToOb * DistAway);
	return (help + posOb);
}

Vector2D SteeringBehaviors::Hide(vector<BaseGameEntity*>& obstacles) {
	double DistToClosest = 10000000000.0;
	Vector2D BestHidingSpot;
	std::vector<BaseGameEntity*>::iterator curOb = obstacles.begin();
	while(curOb != obstacles.end()) {
		Vector2D HidingSpot = GetHidingPosition((*curOb)->Pos(), (*curOb)->BRadius(), target->Pos());
		double dist = Vec2DDistanceSq(HidingSpot, m_pVehicle->Pos());
		if (dist < DistToClosest) {
			DistToClosest = dist;
			BestHidingSpot = HidingSpot;
		}
		++curOb;
	}

	if (DistToClosest == 10000000000.0) {
		return Evade(m_vTarget);
	}
return Arrive(BestHidingSpot, fast);
}


Vector2D SteeringBehaviors::OffsetPursuit(Deceleration deceleration)
{
	//SVector2D WorldOffsetPos = PointToWorldSpace(offset, leader->Heading(), leader->Side(), leader->Pos());
	Vector2D targetWorld;
	targetWorld.x =  leader->mForces.m_vHeading.x * offset.x - leader->mForces.m_vHeading.y * offset.y;
	targetWorld.y =  leader->mForces.m_vHeading.y * offset.x + leader->mForces.m_vHeading.x * offset.y;
	Vector2D ToOffset = targetWorld - mPanda->getPos();
	double LookAheadTime = ToOffset.Length() / (mPanda->mForces.m_dMaxSpeed + leader->mForces.m_dMaxSpeed);
	
	//return Arrive(targetWorld + leader->mForces.m_vVelocity * LookAheadTime, slow);

	
	Vector2D Target = targetWorld + leader->mForces.m_vVelocity * LookAheadTime;
	Vector2D ToTarget = Target - Vector2D(mPanda->getPos());
	double dist = ToTarget.Length();
	if (dist > 0) 
	{
		const double DecelerationTweaker = 0.3;
		double speed = dist / ((double)deceleration * DecelerationTweaker);

		if (speed > mPanda->mForces.m_dMaxSpeed)
			speed = mPanda->mForces.m_dMaxSpeed;

		Vector2D DesiredVelocity = ToTarget * (speed / dist);

		return (DesiredVelocity - mPanda->mForces.m_vVelocity);
	}
	return Vector2D(0,0);
}

*/
void SteeringBehaviors::TagNeighbors()
{
	mNeighbors.clear();

	for (int i = 0; i < mPanda->getScene()->getBots().size(); i++)
	{
		PTGBot *bot = mPanda->getScene()->getBots()[i];
		if (mPanda != bot && !bot->pBehave->busy && !bot->isDead) 
			if (Vector2D(mPanda->getPos()).DistanceSq(Vector2D(bot->getPos())) < R)
			{
				mNeighbors.push_back(bot);
			}
	}
}

Vector2D SteeringBehaviors::Separation()
{
	Vector2D SteeringForce;
	for (int i = 0; i < mPanda->getScene()->getBots().size(); i++)
	{
		PTGBot *bot = mPanda->getScene()->getBots()[i];
		//if (mPanda != bot && !bot->isDead) 
		if (Vector2D(mPanda->getPos()).DistanceSq(Vector2D(bot->getPos())) > 0.1f  && !bot->isDead)
		if (Vector2D(mPanda->getPos()).DistanceSq(Vector2D(bot->getPos())) < R)
		{
			Vector2D ToAgent = Vector2D(mPanda->getPos()) - Vector2D(bot->getPos());
			double Len = ToAgent.Length();
			ToAgent.Normalize();
			SteeringForce += ToAgent/Len;
		}
	}
	return SteeringForce;
}

Vector2D SteeringBehaviors::TreeSeparation()
{
	Vector2D SteeringForce;
	for (int i = 0; i < mPanda->getScene()->getTrees().size(); i++)
	{
		PTGStaticObj *obj = mPanda->getScene()->getTrees()[i];
		if (Vector2D(obj->getPos()).DistanceSq(m_vTarget) > 0.1) 
		if (Vector2D(mPanda->getPos()).DistanceSq(Vector2D(obj->getPos())) < R)
		{
			Vector2D ToAgent = Vector2D(mPanda->getPos()) - Vector2D(obj->getPos());
			double Len = ToAgent.Length();
			ToAgent.Normalize();
			SteeringForce += ToAgent/Len;
		}
	}
	return SteeringForce;
}

Vector2D SteeringBehaviors::Alignment() 
{
	Vector2D AverageHeading(0,0);
	for (int i = 0; i < mNeighbors.size(); i++)
	{
		AverageHeading += mNeighbors[i]->mForces.m_vHeading;
	}
	if (mNeighbors.size() != 0)
	{
		AverageHeading /= (double)mNeighbors.size();
		AverageHeading -= mPanda->mForces.m_vHeading;
	}
	return AverageHeading;
}

Vector2D SteeringBehaviors::Cohesion()
{
	Vector2D CenterOfMass, SteeringForce, DesiredVelocity;
	for (int i = 0; i < mNeighbors.size(); i++)
	{
		CenterOfMass += mNeighbors[i]->mForces.m_vPosition;
	}

	if (mNeighbors.size() != 0)
	{
		CenterOfMass /= (double)mNeighbors.size();
		DesiredVelocity = CenterOfMass - Vector2D(mPanda->getPos());

		DesiredVelocity.Normalize();
		DesiredVelocity*=  mPanda->mForces.m_dMaxSpeed;

	}
	return DesiredVelocity - mPanda->mForces.m_vVelocity;
}

Vector2D SteeringBehaviors::Center()
{
	float maxR = 100 * 100;
	Vector2D DesiredVelocity;
	if (Vector2D(mPanda->getPos()).LengthSq() > maxR) 
	{
		DesiredVelocity = Vector2D(mPanda->getPos()).GetReverse();
		DesiredVelocity.Normalize();
		DesiredVelocity*=  mPanda->mForces.m_dMaxSpeed;
		return DesiredVelocity - mPanda->mForces.m_vVelocity;
	}
	return Vector2D(0,0);
}

void SteeringBehaviors::PandaOverlap() 
{
	Vector2D ToEntity;
	double DistFromEachOther;
	double AmountOfOverLap;
	for (int i = 0; i < mPanda->getScene()->getBots().size(); i++)
	{
		PTGBot *bot = mPanda->getScene()->getBots()[i];
		if (Vector2D(mPanda->getPos()).DistanceSq(Vector2D(bot->getPos())) > 0.1f  && !bot->isDead)
		{
			ToEntity = Vector2D(mPanda->getPos()) - Vector2D(mPanda->getScene()->getBots()[i]->getPos());
			DistFromEachOther = ToEntity.Length();
			AmountOfOverLap = mPanda->getScene()->getBots()[i]->pBehave->BRadius + BRadius - DistFromEachOther;
			if (AmountOfOverLap >= 0)
			{
				/*
				cout << " in " << i << endl;
				cout << mPanda->getPos()).x << " " << mPanda->getPos()).y << endl;
				cout << mNeighbors[i]->mForces.m_vPosition.x << " " << mNeighbors[i]->mForces.m_vPosition.y << endl;
				cout << "out" << endl;
				*/
				ToEntity/=DistFromEachOther;
				mPanda->mForces.m_vPosition = Vector2D(mPanda->getPos()) + ToEntity * AmountOfOverLap;
				D3DXVECTOR2 pos = D3DXVECTOR2(mPanda->mForces.m_vPosition.x, mPanda->mForces.m_vPosition.y);
				mPanda->setPos(pos);
				/*
				cout << mPanda->getPos().x << " " << mPanda->getPos().y << endl;
				cout << mNeighbors[i]->mForces.m_vPosition.x << " " << mNeighbors[i]->mForces.m_vPosition.y << endl;
				system("pause");
				*/
			
			

			}
		}
	}

}

void SteeringBehaviors::TreeOverlap() 
{
	Vector2D ToEntity;
	double DistFromEachOther;
	double AmountOfOverLap;
	for (int i = 0; i < mPanda->getScene()->getTrees().size(); i++)
	{
		PTGStaticObj *obj = mPanda->getScene()->getTrees()[i];
		if (Vector2D(obj->getPos()).DistanceSq(m_vTarget) > 0.1) 
		{
			ToEntity = Vector2D(mPanda->getPos()) - Vector2D(obj->getPos());
			DistFromEachOther = ToEntity.Length();
			AmountOfOverLap = TRadius + BRadius - DistFromEachOther;
			if (AmountOfOverLap >= 0)
			{
				/*
				cout << " in " << i << endl;
				cout << mPanda->getPos()).x << " " << mPanda->getPos()).y << endl;
				cout << mNeighbors[i]->mForces.m_vPosition.x << " " << mNeighbors[i]->mForces.m_vPosition.y << endl;
				cout << "out" << endl;
				*/
				ToEntity/=DistFromEachOther;
				mPanda->mForces.m_vPosition = Vector2D(mPanda->getPos()) + ToEntity * AmountOfOverLap;
				D3DXVECTOR2 pos = D3DXVECTOR2(mPanda->mForces.m_vPosition.x, mPanda->mForces.m_vPosition.y);
				mPanda->setPos(pos);
				/*
				cout << mPanda->getPos().x << " " << mPanda->getPos().y << endl;
				cout << mNeighbors[i]->mForces.m_vPosition.x << " " << mNeighbors[i]->mForces.m_vPosition.y << endl;
				system("pause");
				*/
			
			

			}
		}
	}

}

Vector2D SteeringBehaviors::Calculate() 
{
	Vector2D SteeringForce(0,0);
	SteeringForce += Center() * 3;

	if (!busy)
		TagNeighbors();
	if (doSeek == 1)
		SteeringForce += Seek();
	if (doFlee == 1)
		SteeringForce += Flee() * 3;
	if (doArrive == 1)
		SteeringForce += Arrive(fast) * 2;
	if (doWander == 1)
		SteeringForce += Wander() * 0.5;
	if (doSeparation == 1)
		SteeringForce += Separation();
	if (doTreeSeparation == 1)
		SteeringForce += TreeSeparation();
	if (doAlignment == 1)
		SteeringForce += Alignment();
	if (doCohesion == 1)
		SteeringForce += Cohesion();
		
	/*
	if (doSmoothing == 1)
		mPanda->mForces.m_vHeading = m_pHeadingSmoother->Update(Heading());
		*/


	SteeringForce.Truncate(mPanda->mForces.m_dMaxForce);
	return SteeringForce;
}






