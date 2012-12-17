#include "hwMisc.h"
#include "hwEngine.h"
#include "hwScene.h"
#include "hwRender.h"

#include <iostream>

//-----------------------------------------------------------------------------
// Name: HwObject
// Desc: --------
//-----------------------------------------------------------------------------
HwObject::HwObject(HwEngine *eng)
{
	pEngine = eng;
}
	
inline HwEngine *HwObject::getEngine()
{
	return pEngine;
}

HwRender *HwObject::getRender()
{
	return pEngine->getRender();
}

HwScene *HwObject::getScene()
{
	return pEngine->getScene();
}

//-----------------------------------------------------------------------------
// Name: HwFrustum
// Desc: --------
//-----------------------------------------------------------------------------

HwFrustumPlane HwFrustum::getPlane(int index)
{
	return planes[index];
}

void HwFrustum::normalize()
{
	float denom = 0;
	for (int i = 0; i < 6; i++)
	{
		denom = 1 / sqrt(planes[i].normal.x * planes[i].normal.x 
			+ planes[i].normal.y * planes[i].normal.y 
			+ planes[i].normal.z * planes[i].normal.z);
		planes[i].normal *= denom;
		planes[i].distance *= denom;
	}
}

float HwFrustum::distantionToPoint(int index, D3DXVECTOR3 point)
{
	return D3DXVec3Dot(&planes[index].normal, &point) + planes[index].distance;
}
//-----------------------------------------------------------------------------
// Name: HwCamera
// Desc: --------
//-----------------------------------------------------------------------------

HwCamera::HwCamera(HwEngine *eng) : HwObject(eng)
{
	eye = D3DXVECTOR3(0, 0, 0);
	at = D3DXVECTOR3(0, 0, 0);
	up = D3DXVECTOR3(0, 1, 0);

	fov = D3DX_PI / 4.0f;
	nearDist = 0.1f;
	farDist = 1000.0f;

	camType = HWCT_FREE;
}

void HwCamera::init()
{
	D3DVIEWPORT9 view = getEngine()->getRender()->getViewPort();
	float aspectRatio = float(view.Width) / float(view.Height);

	getEngine()->getRender()->setPerspectiveFovLH(&projMatrix, fov, aspectRatio, nearDist, farDist);	
	getEngine()->getRender()->setLookAtLH(&viewMatrix, &eye, &at, &up);
}

void HwCamera::update()
{
	getEngine()->getRender()->setLookAtLH(&viewMatrix, &eye, &at, &up);
	getEngine()->getRender()->setTransform(D3DTS_PROJECTION, &projMatrix);

	updateFrustum();
}

D3DXVECTOR3 HwCamera::getDirection()
{
	D3DXVECTOR3 dir( at.x - eye.x, at.y - eye.y, at.z - eye.z);
	D3DXVec3Normalize(&dir, &dir);
	return dir;
}

void HwCamera::setDirection(D3DXVECTOR3 dir)
{
	at.x = eye.x + dir.x;
	at.y = eye.y + dir.y;
	at.z = eye.z + dir.z;
}

D3DXVECTOR3 HwCamera::getRight()
{
	D3DXVECTOR3 dir = getDirection();
	D3DXVECTOR3 res;
	D3DXVec3Cross(&res, &dir, &up);
	return res;
}

D3DXMATRIX &HwCamera::viewM()
{
	return viewMatrix;
}

D3DXMATRIX &HwCamera::projM()
{
	return projMatrix;
}

void HwCamera::turn(float angle)
{
	D3DXVECTOR3 dir = getDirection();
	dir.x = cos(angle);
	dir.z = sin(angle);
	setDirection(dir);
}

void HwCamera::pitch(float angle)
{
	D3DXVECTOR3 dir = getDirection();
	dir.y = sin(angle);
	setDirection(dir);
}

void HwCamera::yaw(float angle)
{
	//-----
}

void HwCamera::turnAround(D3DXVECTOR3 point, float turn, float pitch, float dist, float sens)
{
	dist = abs(dist);
	at = point;
	D3DXVECTOR3 dir = getDirection();
	float tx = atan2(-dir.z, -dir.x) - turn;
	float ty = asin(-dir.y);

	if ((ty + pitch) > D3DX_PI / 2)
		ty = D3DX_PI / 2 - 0.001f;
	else if  ((ty + pitch) < -D3DX_PI / 2)
		ty = -D3DX_PI / 2 + 0.001f;
	else
		ty += pitch;

	eye.x = cos(tx) * cos(ty) * dist + point.x;
	eye.y = sin(ty) * dist + point.y;
	eye.z = sin(tx) * cos(ty) * dist + point.z;
}

void HwCamera::turnAround(float turn, float pitch, float dist, float sens)
{
	turnAround(at, turn, pitch, dist, sens);
}

void HwCamera::turnFPS(float turn, float pitch)
{
	D3DXVECTOR3 dir = getDirection();
	float tx = atan2(-dir.z, -dir.x) - turn;
	float ty = asin(-dir.y);

	dir.x = cos(tx) * cos(ty);
	dir.y = sin(ty);
	dir.z = sin(tx) * cos(ty);
	setDirection(dir);
}

void HwCamera::moveForward(float dist)
{
	D3DXVECTOR3 dir = getDirection();
	if (camType == HWCT_FREE || camType == HWCT_RECT)
	{
		eye.x += dist * dir.x;
		at.x += dist * dir.x;
		if (camType = HWCT_FREE)
		{
			eye.y += dist * dir.y;
			at.y  += dist * dir.y;
		}
		eye.z += dist * dir.z;
		at.z += dist * dir.z;
		//setDirection(dir);
	} 
	else //MODELVIEW
	{
		at.x += dist * dir.x;
		at.z += dist * dir.z;
	}
}

void HwCamera::slide(float dist)
{
	D3DXVECTOR3 right = getRight();
	if (camType == HWCT_MODELVIEW)
	{
		at.x += dist * right.x;
		at.z += dist * right.z;
	}
	else
	{
		eye.x += dist * right.x;
		eye.z += dist * right.z;
	}
}

void HwCamera::updateFrustum() //TO DO do a bit less lines
{
	D3DXMATRIX matComb = viewMatrix * projMatrix;
	//Left plane
	frustum.planes[0].normal.x = matComb._14 + matComb._11;
	frustum.planes[0].normal.y = matComb._24 + matComb._21;
	frustum.planes[0].normal.z = matComb._34 + matComb._31;
	frustum.planes[0].distance = matComb._44 + matComb._41;

	//Right plane
	frustum.planes[1].normal.x = matComb._14 - matComb._11;
	frustum.planes[1].normal.y = matComb._24 - matComb._21;
	frustum.planes[1].normal.z = matComb._34 - matComb._31;
	frustum.planes[1].distance = matComb._44 - matComb._41;

	//Top plane
	frustum.planes[2].normal.x = matComb._14 - matComb._12;
	frustum.planes[2].normal.y = matComb._24 - matComb._22;
	frustum.planes[2].normal.z = matComb._34 - matComb._32;
	frustum.planes[2].distance = matComb._44 - matComb._42;

	//Bottom plane
	frustum.planes[3].normal.x = matComb._14 + matComb._12;
	frustum.planes[3].normal.y = matComb._24 + matComb._22;
	frustum.planes[3].normal.z = matComb._34 + matComb._32;
	frustum.planes[3].distance = matComb._44 + matComb._42;

	//Near plane
	frustum.planes[4].normal.x = matComb._13;
	frustum.planes[4].normal.y = matComb._23;
	frustum.planes[4].normal.z = matComb._33;
	frustum.planes[4].distance = matComb._43;

	//Near plane
	frustum.planes[5].normal.x = matComb._14 - matComb._13;
	frustum.planes[5].normal.y = matComb._24 - matComb._23;
	frustum.planes[5].normal.z = matComb._34 - matComb._33;
	frustum.planes[5].distance = matComb._44 - matComb._43; 
}

bool HwCamera::pointInFrustum(D3DXVECTOR3 p)
{
	for (int i = 0; i < 6; i++)
		if (frustum.distantionToPoint(i, p) <= 0)
			return false;
	return true;
}

bool HwCamera::boxInFrustum(D3DXVECTOR3 min, D3DXVECTOR3 max)
{
	D3DXVECTOR3 minP;
	for (int i = 0; i < 6; i++)
	{
		minP.x = (frustum.planes[i].normal.x <= 0) ? min.x : max.x;
		minP.y = (frustum.planes[i].normal.y <= 0) ? min.y : max.y;
		minP.z = (frustum.planes[i].normal.z <= 0) ? min.z : max.z;
		if (frustum.distantionToPoint(i, minP) < 0)
			return false;
	}
	return true;
}

bool HwCamera::spherInFrustum(D3DXVECTOR3 center, float rad)
{
	for (int i = 0; i < 6; i++)
		if (frustum.planes[i].normal.x * center.x +
			frustum.planes[i].normal.y * center.y +
			frustum.planes[i].normal.z * center.z +
			frustum.planes[i].distance <= -rad)
			return false;
	return true;
}

void HwCamera::screenToRay(int x, int y, D3DXVECTOR3 *pRayOrigin, D3DXVECTOR3 *pRayDir)
{
	D3DVIEWPORT9 view = getEngine()->getRender()->getViewPort();
	int w = view.Height;
	int h = view.Height;
	D3DXVECTOR3 vec;
	vec.x = (x * 2 / w - 1) / projMatrix._11;
	vec.y = -(y * 2 / h - 1) / projMatrix._22;
	vec.z = 1;

	D3DXMATRIX matInv;
	D3DXMatrixInverse(&matInv, NULL, &projMatrix);

	pRayDir->x = vec.x * matInv._11 + vec.y * matInv._12 + vec.z * matInv._13;
	pRayDir->y = vec.x * matInv._21 + vec.y * matInv._22 + vec.z * matInv._23;
	pRayDir->z = vec.x * matInv._31 + vec.y * matInv._32 + vec.z * matInv._33;

	pRayOrigin->x = matInv._41;
	pRayOrigin->y = matInv._42;
	pRayOrigin->z = matInv._43;
}

D3DXVECTOR3 HwCamera::worldToScreen(D3DXVECTOR3 v)
{
	D3DVIEWPORT9 view = getEngine()->getRender()->getViewPort();
	D3DXMATRIX wm;
	D3DXMatrixIdentity(&wm);
	D3DXVECTOR3 out;
	D3DXVec3Project(&out, &v, &view, &projMatrix, &viewMatrix, &wm);

	return out;
}

