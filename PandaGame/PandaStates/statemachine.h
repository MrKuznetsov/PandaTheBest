#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <string>
#include "state.h"

template <class entity_type>
class StateMachine
{
private:
  entity_type*          m_pOwner;
  State<entity_type>*   m_pCurrentState;
  State<entity_type>*   m_pPreviousState;

public:
  StateMachine(entity_type* owner):m_pOwner(owner),
                                   m_pCurrentState(NULL),
                                   m_pPreviousState(NULL)
  {}

  virtual ~StateMachine(){}

  void SetCurrentState(State<entity_type>* s){m_pCurrentState = s;}
  void SetPreviousState(State<entity_type>* s){m_pPreviousState = s;}

  void  Update()const 
  {
    if (m_pCurrentState) m_pCurrentState->Execute(m_pOwner);
  }

  void  ChangeState(State<entity_type>* pNewState) {
    m_pPreviousState = m_pCurrentState;
    m_pCurrentState->Exit(m_pOwner);
    m_pCurrentState = pNewState;
    m_pCurrentState->Enter(m_pOwner);
  }

  void ChangeBackState() {
    m_pCurrentState->Exit(m_pOwner);
    m_pCurrentState = m_pPreviousState;
    m_pCurrentState->Enter(m_pOwner);
  }

  void  RevertToPreviousState() {
//ChangeState(m_pPreviousState);
	State<entity_type>* tmp = m_pPreviousState;
    m_pPreviousState = m_pCurrentState;
    m_pCurrentState->Exit(m_pOwner);
    m_pCurrentState = tmp;
  }

  bool  isInState(const State<entity_type>& st)const {
    return typeid(*m_pCurrentState) == typeid(st);
  }

  State<entity_type>*  CurrentState()  const{return m_pCurrentState;}
  State<entity_type>*  PreviousState() const{return m_pPreviousState;}
};

#endif // STATEMACHINE_H
