#ifndef PANDASTATES_H
#define PANDASTATES_H
#include "state.h"

#define DTSCALE 0.1f

class PTGBot;

class Panda;

class MoveTo : public State<PTGBot>
{
private:
    MoveTo() {}
    MoveTo(const MoveTo&);
    MoveTo& operator=(const MoveTo&);

public:
    static MoveTo* Instance();
    virtual void Enter(PTGBot* pPanda);
    virtual void Execute(PTGBot* pPanda);
    virtual void Exit(PTGBot* pPanda);
};

class Follow : public State<PTGBot>
{
private:
    Follow() {}
    Follow(const Follow&);
    Follow& operator=(const Follow&);

public:
    static Follow* Instance();
    virtual void Enter(PTGBot* pPanda);
    virtual void Execute(PTGBot* pPanda);
    virtual void Exit(PTGBot* pPanda);
};

class Eat : public State<PTGBot>
{
private:
    Eat() {}
    Eat(const Eat&);
    Eat& operator=(const Eat&);

public:
    static Eat* Instance();
    virtual void Enter(PTGBot* pPanda);
    virtual void Execute(PTGBot* pPanda);
    virtual void Exit(PTGBot* pPanda);

};

class Sleeping : public State<PTGBot>
{
private:
    Sleeping() {}
    Sleeping(const Sleeping&);
    Sleeping& operator=(const Sleeping&);

public:
    static Sleeping* Instance();
    virtual void Enter(PTGBot* pPanda);
    virtual void Execute(PTGBot* pPanda);
    virtual void Exit(PTGBot* pPanda);

};

class Talk : public State<PTGBot>
{
private:
    Talk() {}
    Talk(const Talk&);
    Talk& operator=(const Talk&);

public:
    static Talk* Instance();
    virtual void Enter(PTGBot* pPanda);
    virtual void Execute(PTGBot* pPanda);
    virtual void Exit(PTGBot* pPanda);

};

class Walk : public State<PTGBot>
{
private:
    Walk() {}
    Walk(const Walk&);
    Walk& operator=(const Walk&);

public:
    static Walk* Instance();
    virtual void Enter(PTGBot* pPanda);
    virtual void Execute(PTGBot* pPanda);
    virtual void Exit(PTGBot* pPanda);
};

class Fear : public State<PTGBot>
{
private:
    Fear() {}
    Fear(const Fear&);
    Fear& operator=(const Fear&);

public:
    static Fear* Instance();
    virtual void Enter(PTGBot* pPanda);
    virtual void Execute(PTGBot* pPanda);
    virtual void Exit(PTGBot* pPanda);
};



#endif // PANDASTATES_H
