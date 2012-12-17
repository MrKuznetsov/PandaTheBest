#ifndef PANDA_H
#define PANDA_H
#include <string>

//#include "locationtypes.h"
//include "pandastates.h"
//#include "statemachine.h"

//class State;

class Panda
{
public:
  //  StateMachine<Panda>* m_pStateMachine;
   // location_type m_Location;
    float m_iHunger;
    float m_iWeariness;
    float m_iAlone;

public:
    Panda()://m_Location(field),
        m_iHunger(20 + rand() % 80),
        m_iWeariness(20 + rand() % 80),
        m_iAlone(0)
	{
    //	m_pStateMachine = new StateMachine<Panda>(this);
	//	m_pStateMachine->SetCurrentState(Walk::Instance());
	}

   // StateMachine<Panda>* GetFSM()const{return m_pStateMachine;}

   // location_type Location();
  //  void ChangeLocation(location_type i);

    void ChangeHunger(float i);
    bool Hunger();
	bool isHunger();

    void ChangeWeariness(float i);
    bool Weariness();
	bool isWeariness();

    void ChangeAlone(float i);
    bool Alone();
};

#endif // PANDA_H
