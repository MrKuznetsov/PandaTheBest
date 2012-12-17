#pragma once

#include <d3dx9.h>
#include "hwTypes.h"


class HwEngine;
class HwCamera;
class HwRender;
class HwScene;
//-----------------------------------------------------------------------------
// Name: HwObject
// Desc: --------
//-----------------------------------------------------------------------------

class HwObject 
{
private:
	HwEngine *pEngine;
public:
	HwObject(HwEngine *eng);
	virtual ~HwObject(){}
	
	HwEngine *getEngine();
	HwRender *getRender();
	HwScene *getScene();
};

//-----------------------------------------------------------------------------
// Name: HwFrustum
// Desc: --------
//-----------------------------------------------------------------------------

struct HwFrustumPlane
{
	D3DXVECTOR3 normal;
	float distance;
};

class HwFrustum
{
private:
	HwFrustumPlane planes[6];
public:
	HwFrustumPlane getPlane(int index);
	void normalize();
	float distantionToPoint(int index, D3DXVECTOR3 point);

	friend HwCamera;
};

//-----------------------------------------------------------------------------
// Name: HwCamera
// Desc: --------
//-----------------------------------------------------------------------------

class HwCamera : public HwObject
{
private:
	D3DXVECTOR3 eye;
	D3DXVECTOR3 at;
	D3DXVECTOR3 up;

	D3DXMATRIX viewMatrix;
	D3DXMATRIX projMatrix;

	float fov;
	float nearDist;
	float farDist;

	HWCAMERATYPE camType;
	HwFrustum frustum;
public:
	HwCamera(HwEngine *eng);

//--------PRIVATE MEMBER SETS GETS --------//
	D3DXVECTOR3 &getEye() { return eye; }
	D3DXVECTOR3 &getAt() { return at; }
	D3DXVECTOR3 &getUp() { return up; }
	void setEye(D3DXVECTOR3 v) { eye = v; }
	void setAt(D3DXVECTOR3 v) { at = v; }
	void setUp(D3DXVECTOR3 v) { up = v; }

	float getFov() { return fov; }
	float getNearDist() { return nearDist; }
	float getFarDist()  {return farDist; }
	void setFov(float f) { fov = f; }
	void setNearDist(float f) { nearDist = f; }
	void setFarDist(float f) { farDist = f; }

	void setCameraType(HWCAMERATYPE t) { camType = t; }
//--------------------------------------------//
	D3DXVECTOR3 getDirection();
	void setDirection(D3DXVECTOR3 dir);
	D3DXVECTOR3 getRight();

	D3DXMATRIX &viewM();
	D3DXMATRIX &projM();

	void turn(float angle);
	void pitch(float angle);
	void yaw(float angle);
	void turnAround(D3DXVECTOR3 point, float turn, float pitch, float dist, float sens = 0.005f);
	void turnAround(float turn, float pitch, float dist, float sens = 0.005f);
	void turnFPS(float turn, float pitch);
	void moveForward(float dist);
	void slide(float dist);
	//lift

	void updateFrustum();
	bool pointInFrustum(D3DXVECTOR3 p);
	bool boxInFrustum(D3DXVECTOR3 min, D3DXVECTOR3 max);
	bool spherInFrustum(D3DXVECTOR3 center, float rad);

	void screenToRay(int x, int y, D3DXVECTOR3 *pRayOrigin, D3DXVECTOR3 *pRayDir);
	D3DXVECTOR3 worldToScreen(D3DXVECTOR3 v);

	void init();
	void update();
};