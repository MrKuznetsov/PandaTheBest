#pragma once

#include <d3dx9.h>
#include <vector>
#include <string>

#include "hwTypes.h"

using namespace std;

//-----------------------------------------------------------------------------
// Name: HwAnimFrame
// Desc: -----------
//-----------------------------------------------------------------------------

class HwAnimFrame
{
private:
	D3DXVECTOR3 *pos;  
	D3DXQUATERNION *rot;
	int boneCount;
public:
	HwAnimFrame(int bCount);
	~HwAnimFrame();

	D3DXVECTOR3 &position(int index);
	D3DXQUATERNION &rotation(int index);
};

//-----------------------------------------------------------------------------
// Name: HwAnimation
// Desc: -----------
//-----------------------------------------------------------------------------

class HwAnimation
{
private:
	string aName;
	vector<HwAnimFrame *> list; //List of frames
public:
	string &name();
	HwAnimFrame *getFrame(int index);
	int getCount();

	HwAnimFrame *addFrame(int bCount);
};

//-----------------------------------------------------------------------------
// Name: HwSkeletBone
// Desc: ------------
//-----------------------------------------------------------------------------

class HwSkeletBone
{
private:
	string bName;
	D3DXVECTOR3 pos;  
	D3DXQUATERNION rot;

	DWORD childCount;
	DWORD *childs;
	DWORD parent;
public:
	string &name();
	D3DXVECTOR3 &position();
	D3DXQUATERNION &rotation();
};

//-----------------------------------------------------------------------------
// Name: HwSkeleton
// Desc: ----------
//-----------------------------------------------------------------------------
// TO DO BONES
class HwSkeleton
{
private:
	vector<HwSkeletBone *> boneList;
	vector<HwAnimation *> animList;

	D3DXMATRIX *updates;
	DWORD boneCount;
public:
	void init(int bCount);

	HwAnimation *newAnimation(string name);
	HwSkeletBone *newBone(string name);

	int getAnimID(string name);
	HwAnimation *animBy(int id);
	HwAnimation *animBy(string name);

	HwSkeletBone *boneBy(int index);
	HwSkeletBone *boneBy(string name);

	DWORD getBoneCount();

	void update(float time, int animID, bool interp = true);

	D3DXMATRIX *getUpdates();
};
