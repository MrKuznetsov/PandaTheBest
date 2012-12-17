#include "panda.h"

#include <iostream>
using namespace std;


void Panda::ChangeHunger(float i) {
    m_iHunger+= i;
    if (m_iHunger > 100)
        m_iHunger = 100;
    else if (m_iHunger < 0)
        m_iHunger = 0;
}

bool Panda::Hunger() {
    if (m_iHunger < 70)
        return 0;
    return 1;
}

bool Panda::isHunger()
{
    if (m_iHunger < 40)
        return 0;
    return 1;
}

void Panda::ChangeWeariness(float i) {
    m_iWeariness+= i;
    if (m_iWeariness > 100)
        m_iWeariness = 100;
    else if (m_iWeariness < 0)
        m_iWeariness = 0;
}

bool Panda::Weariness() {
    if (m_iWeariness < 70)
        return 0;
    return 1;
}

bool Panda::isWeariness()
{
    if (m_iWeariness < 40)
        return 0;
    return 1;
}

void Panda::ChangeAlone(float i) {
    m_iAlone+= i;
    if (m_iAlone > 100)
        m_iAlone = 100;
    else if (m_iAlone < 0)
        m_iAlone = 0;
}

bool Panda::Alone() {
    if (m_iAlone < 70)
        return 0;
    return 1;
}

