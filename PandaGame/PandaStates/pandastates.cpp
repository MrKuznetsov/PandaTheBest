#include "pandastates.h"
#include "state.h"

#include "..\pandaTheGame.h"


using namespace std;

MoveTo* MoveTo::Instance() {
    static MoveTo instance;
    return &instance;
}

void MoveTo::Enter(PTGBot* pPanda) 
{ 
	pPanda->pBehave->doArrive = 1;
	pPanda->findEatingPoint();
	pPanda->pBehave->doTreeSeparation = 0;
	pPanda->pBehave->doSeparation = 0;
}

void MoveTo::Execute(PTGBot *pPanda) 
{
	pPanda->getStats().ChangeHunger(1 * pPanda->getScene()->getDt() * DTSCALE);
    pPanda->getStats().ChangeWeariness(1 * pPanda->getScene()->getDt() * DTSCALE);
    pPanda->getStats().ChangeAlone(1 * pPanda->getScene()->getDt() * DTSCALE);

	if (pPanda->pBehave->getFleeLength() < 10.0 * 10.0)
	{
		pPanda->getFSM()->ChangeState(Fear::Instance());
	}

	pPanda->findEatingPoint();

	if (!pPanda->move())
	{
		pPanda->getFSM()->RevertToPreviousState();
	}
}

void MoveTo::Exit(PTGBot *pPanda) 
{
	pPanda->pBehave->doArrive = 0;
	pPanda->pBehave->doTreeSeparation = 1;
	pPanda->pBehave->doSeparation = 1;
}

//=========================================

Eat* Eat::Instance() {
    static Eat instance;
    return &instance;
}

void Eat::Enter(PTGBot* pPanda) 
{ 
	pPanda->getFSM()->ChangeState(MoveTo::Instance());
	/* need to correct */
	//pPanda->pBehave->doTreeSeparation = 0;
}

void Eat::Execute(PTGBot *pPanda) 
{
	pPanda->getStats().ChangeHunger(-3 * pPanda->getScene()->getDt() * DTSCALE);
    pPanda->getStats().ChangeWeariness(0.5 * pPanda->getScene()->getDt() * DTSCALE);
    pPanda->getStats().ChangeAlone(0.5 * pPanda->getScene()->getDt() * DTSCALE);

	if (pPanda->pBehave->getFleeLength() < 10.0 * 10.0)
	{
		pPanda->getFSM()->ChangeState(Fear::Instance());
	}

	pPanda->eat();

    if (!pPanda->getStats().isHunger())
    {
        if (pPanda->getStats().Weariness())
            pPanda->getFSM()->ChangeState(Sleeping::Instance());
    //    else if (pPanda->getStats().Alone())
    //        pPanda->getFSM()->ChangeState(Talk::Instance());
        else
            pPanda->getFSM()->ChangeState(Walk::Instance());
    }
}

void Eat::Exit(PTGBot *pPanda) 
{
	//pPanda->pBehave->doTreeSeparation = 1;
}

Sleeping *Sleeping::Instance() 
{
    static Sleeping instance;
    return &instance;
}

void Sleeping::Enter(PTGBot* pPanda) 
{
	/* need to correct */
}

void Sleeping::Execute(PTGBot *pPanda) 
{
    pPanda->getStats().ChangeHunger(0.5 * pPanda->getScene()->getDt() * DTSCALE);
    pPanda->getStats().ChangeWeariness(-3 * pPanda->getScene()->getDt() * DTSCALE);
    pPanda->getStats().ChangeAlone(0.5 * pPanda->getScene()->getDt() * DTSCALE);

	pPanda->sleep();

    if (!pPanda->getStats().isWeariness())
    {
        if (pPanda->getStats().Hunger())
            pPanda->getFSM()->ChangeState(Eat::Instance());
    //    else if (pPanda->getStats().Alone())
      //      pPanda->getFSM()->ChangeState(Talk::Instance());
        else
            pPanda->getFSM()->ChangeState(Walk::Instance());
    }
}

void Sleeping::Exit(PTGBot *pPanda) 
{

}


Talk* Talk::Instance() {
    static Talk instance;
    return &instance;
}

void Talk::Enter(PTGBot* pPanda) 
{
	/* need to correct */
}

void Talk::Execute(PTGBot *pPanda) 
{
    pPanda->getStats().ChangeHunger(1 * pPanda->getScene()->getDt() * DTSCALE);
    pPanda->getStats().ChangeWeariness(1 * pPanda->getScene()->getDt() * DTSCALE);
    //pPanda->getStats().ChangeAlone(-10);

    if (pPanda->getStats().Hunger())
        pPanda->getFSM()->ChangeState(Eat::Instance());
    else if (pPanda->getStats().Weariness())
        pPanda->getFSM()->ChangeState(Sleeping::Instance());
    else if(!pPanda->getStats().Alone())
        pPanda->getFSM()->ChangeState(Walk::Instance());

}

void Talk::Exit(PTGBot *pPanda) 
{

}

Walk* Walk::Instance() {
    static Walk instance;
    return &instance;
}

void Walk::Enter(PTGBot* pPanda) 
{
	pPanda->pBehave->busy = 0;
	pPanda->pBehave->doWander = 1;
	//pPanda->pBehave->doSeparation = 1;
	pPanda->pBehave->doAlignment = 1;
	pPanda->pBehave->doCohesion = 1;
}

void Walk::Execute(PTGBot *pPanda) 
{
    pPanda->getStats().ChangeHunger((rand() % 2) * pPanda->getScene()->getDt() * DTSCALE);
    pPanda->getStats().ChangeWeariness((rand() % 2) * pPanda->getScene()->getDt() * DTSCALE);
    //pPanda->getStats().ChangeAlone(rand()%4 * pPanda->getScene()->getDt());

	if (pPanda->pBehave->getFleeLength() < 10.0 * 10.0)
	{
		pPanda->getFSM()->ChangeState(Fear::Instance());
	}

	pPanda->walking();

    if (pPanda->getStats().Hunger())
        pPanda->getFSM()->ChangeState(Eat::Instance());
    else if (pPanda->getStats().Weariness())
        pPanda->getFSM()->ChangeState(Sleeping::Instance());
   // else if(pPanda->getStats().Alone())
    //    pPanda->getFSM()->ChangeState(Talk::Instance());
}

void Walk::Exit(PTGBot *pPanda) 
{
	pPanda->pBehave->busy = 1;
	pPanda->pBehave->doWander = 0;
	//pPanda->pBehave->doSeparation = 0;
	pPanda->pBehave->doAlignment = 0;
	pPanda->pBehave->doCohesion = 0;
}

Fear *Fear::Instance() 
{
    static Fear instance;
    return &instance;
}

void Fear::Enter(PTGBot* pPanda) 
{
	pPanda->pBehave->busy = 0;
	pPanda->pBehave->doFlee = 1;
	pPanda->pBehave->doSeparation = 0;
	pPanda->pBehave->doTreeSeparation = 0;
}

void Fear::Execute(PTGBot *pPanda) 
{
	pPanda->walking();
	if (pPanda->pBehave->getFleeLength() > 15.0 * 15.0)
	{
		if (pPanda->getStats().Hunger())
			pPanda->getFSM()->ChangeState(Eat::Instance());
		else if (pPanda->getStats().Weariness())
			pPanda->getFSM()->ChangeState(Sleeping::Instance());
		else
			pPanda->getFSM()->ChangeState(Walk::Instance());
	}
}

void Fear::Exit(PTGBot *pPanda) 
{
	pPanda->pBehave->busy = 1;
	pPanda->pBehave->doSeparation = 1;
	pPanda->pBehave->doTreeSeparation = 1;
}


