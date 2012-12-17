#ifndef STATE_H
#define STATE_H

#include <iostream>

class Panda;

template <class entity_type>
class State
{
public:
    virtual ~State() {}

    virtual void Enter (entity_type*) = 0;
    virtual void Execute (entity_type*) = 0;
    virtual void Exit (entity_type*) = 0;
};

#endif // STATE_H
