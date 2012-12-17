#include "hwSkeleton.h"

#include <iostream>


//-----------------------------------------------------------------------------
// Name: HwAnimFrame
// Desc: -----------
//-----------------------------------------------------------------------------

HwAnimFrame::HwAnimFrame(int bCount)
{
	boneCount = bCount;
	pos = new D3DXVECTOR3[bCount];
	rot = new D3DXQUATERNION[bCount];
}

HwAnimFrame::~HwAnimFrame()
{
	delete[] pos;
	delete[] rot;
}

D3DXVECTOR3 &HwAnimFrame::position(int index)
{
	return pos[index];
}

D3DXQUATERNION &HwAnimFrame::rotation(int index)
{
	return rot[index];
}

//-----------------------------------------------------------------------------
// Name: HwAnimation
// Desc: -----------
//-----------------------------------------------------------------------------

string &HwAnimation::name()
{
	return aName;
}

HwAnimFrame *HwAnimation::getFrame(int index)
{
	return list[index];
}

int HwAnimation::getCount()
{
	return list.size();
}

HwAnimFrame *HwAnimation::addFrame(int bCount)
{
	HwAnimFrame *frame = new HwAnimFrame(bCount);
	list.push_back(frame);

	return frame;
}

//-----------------------------------------------------------------------------
// Name: HwSkeletBone
// Desc: ------------
//-----------------------------------------------------------------------------

string &HwSkeletBone::name()
{
	return bName;
}

D3DXVECTOR3 &HwSkeletBone::position()
{
	return pos;
}

D3DXQUATERNION &HwSkeletBone::rotation()
{
	return rot;
}

//-----------------------------------------------------------------------------
// Name: HwSkeleton
// Desc: ----------
//-----------------------------------------------------------------------------

void HwSkeleton::init(int bCount)
{
	updates = new D3DXMATRIX[bCount];
	boneCount = bCount;
}

D3DXMATRIX *HwSkeleton::getUpdates()
{
	return updates;
}

HwAnimation *HwSkeleton::newAnimation(string name)
{
	HwAnimation *anim = new HwAnimation;
	anim->name() =  name;
	animList.push_back(anim);
	return anim;
}

HwSkeletBone *HwSkeleton::newBone(string name)
{
	HwSkeletBone *bone = new HwSkeletBone;
	bone->name() =  name;
	boneList.push_back(bone);
	return bone;
}

int HwSkeleton::getAnimID(string name)
{
	for (int i = 0; i < animList.size(); i++)
		if (animList[i]->name() == name)
			return i;
	return -1;
}

HwAnimation *HwSkeleton::animBy(int id)
{
	if (id < 0 || id >= animList.size())
		return NULL;
	return animList[id];
}

HwAnimation *HwSkeleton::animBy(string name)
{
	int id = getAnimID(name);
	return id != -1 ? animList[id] : NULL;
}

HwSkeletBone *HwSkeleton::boneBy(int index)
{
	if (index < 0 || index >= boneList.size())
		return NULL;
	return boneList[index];
}

HwSkeletBone *HwSkeleton::boneBy(string name)
{
	for (int i = 0; i < boneList.size(); i++)
		if (boneList[i]->name() == name)
			return boneList[i];
	return NULL;
}

DWORD HwSkeleton::getBoneCount()
{
	return boneCount;
}

/*inline void D3DXQUATERNIONLerp(D3DXQUATERNION *pOut, CONST D3DXQUATERNION *q1, CONST D3DXQUATERNION *q2, float s)
{
	float l = q1->x * q2->x + q1->y * q2->y + q1->z * q2->z + q1->w * q2->w;
	if (l < 0)
		*pOut = *q1 - (*q2 + *q1) * s;
	else
		*pOut = *q1 - (*q2 - *q1) * s;
}*/

void D3DXQuaternionToMatrix(D3DXMATRIX *pOut, const D3DXQUATERNION *q)
{
	float x = q->x;
	float y = q->y;
	float z = q->z;
	float w = q->w;

	float xx = x * (x + x), xy = x * (y + y), xz = x * (z + z);
	float yy = y * (y + y), yz = y * (z + z), zz = z * (z + z);
	float wx = w * (x + x), wy = w * (y + y), wz = w * (z + z);

	pOut->m[0][0] = 1 - (yy + zz);  pOut->m[1][0] = xy + wz;        pOut->m[2][0] = xz - wy;
	pOut->m[0][1] = xy - wz;        pOut->m[1][1] = 1 - (xx + zz);  pOut->m[2][1] = yz + wx;
	pOut->m[0][2] = xz + wy;        pOut->m[1][2] = yz - wx;        pOut->m[2][2] = 1 - (xx + yy);

	pOut->m[3][0] = 0;
	pOut->m[3][1] = 0;
	pOut->m[3][2] = 0;
	pOut->m[0][3] = 0;
	pOut->m[1][3] = 0;
	pOut->m[2][3] = 0;
	pOut->m[3][3] = 1;

}

void HwSkeleton::update(float time, int animID, bool interp)
{
	// time = curent frame
	int lf = static_cast<int>(time) % animList[animID]->getCount();
	int nf = (lf + 1) % animList[animID]->getCount();
	float t = time - static_cast<int>(time);


	D3DXVECTOR3 p;  
	D3DXQUATERNION r;

	for (int i = 0; i < boneCount; i++)
	{
		if (interp)
		{
			D3DXVec3Lerp(&p, &animList[animID]->getFrame(lf)->position(i), &animList[animID]->getFrame(nf)->position(i), t);
			D3DXQuaternionSlerp(&r, &animList[animID]->getFrame(lf)->rotation(i), &animList[animID]->getFrame(nf)->rotation(i), t);
		}
		else
		{
			p = animList[animID]->getFrame(lf)->position(i);
			r = animList[animID]->getFrame(lf)->rotation(i);
		}
		D3DXQuaternionToMatrix(&updates[i], &r);
		updates[i].m[3][0] = p.x;
		updates[i].m[3][1] = p.y;
		updates[i].m[3][2] = p.z;
	}
	
}