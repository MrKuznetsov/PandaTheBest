#ifndef STEERINGBEHAVIORS_H
#define STEERINGBEHAVIORS_H
#include "vector2d.h"

#include <iostream>
#include <vector>
using namespace std;
class PTGBot;

class SteeringBehaviors
{
public:
    PTGBot *mPanda;
	vector<PTGBot *> mNeighbors;
	bool busy;

	bool doSeek;
    bool doFlee;
    bool doArrive;
	bool doOffsetPursuit;
    bool doWander;
	bool doSeparation;
	bool doTreeSeparation;
	bool doAlignment;
	bool doCohesion;

	//  bool doPursuit();
  //  bool doEvade();

    enum Deceleration{slow = 3, normal = 2, fast = 1};

	Vector2D m_vTarget;
	Vector2D m_vFleeTarget;

	float R;
	float BRadius;
	float TRadius;
    //for wander
    double m_dWanderRadius; // need to be tested
    double m_dWanderDistance; // and this
    double m_dWanderJitter; // this too
    Vector2D m_vWanderTarget;


    SteeringBehaviors(PTGBot* gPanda);
    Vector2D Seek();
    Vector2D Flee();
	Vector2D Evade();
    Vector2D Arrive(Deceleration deceleration);

  //  Vector2D Pursuit();
  //  Vector2D Evade();
    Vector2D Wander();

	void TagNeighbors();
	Vector2D Separation();
	Vector2D TreeSeparation();
	Vector2D Alignment();
	Vector2D Cohesion();

	Vector2D Center();

	void PandaOverlap();
	void TreeOverlap();


  //  Vector2D GetHidingPosition(Vector2D &posOb, double radiusOb);
    Vector2D Calculate();

    static double getRand(double b, double e);
	double getFleeLength();
};

#endif // STEERINGBEHAVIORS_H
