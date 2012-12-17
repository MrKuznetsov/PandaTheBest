#pragma once

#include <iostream>
#include <vector>
#include <d3d9.h>
#include <time.h>

#include "..\HwEng\hwEngine\hwEngine.h"
#include "..\HwEng\hwEngine\hwMisc.h"
#include "..\HwEng\hwEngine\hwSkeleton.h"
#include "..\HwEng\hwEngine\hwBuffers.h"
#include "..\HwEng\hwEngine\hwMesh.h"
#include "..\HwEng\hwEngine\hwTerrain.h"
#include "..\HwEng\hwEngine\hwGrass.h"
#include "..\HwEng\hwEngine\hwEntities.h"
#include "..\HwEng\hwEngine\hwShaders.h"

#include "PMove\movingentity.h"
#include "PMove\steeringbehaviors.h"

#include "PandaStates\panda.h"
#include "PandaStates\pandastates.h"
#include "PandaStates\statemachine.h"

#define NUM_MATS 8
#define NUM_BOTS 100
#define ISSHADOW false
#define ISANIM false
//-62 51
#define BMLOL D3DXVECTOR3(-0, 0, -0)

using namespace std;

class PTGObject;
class PTGCreature;
class PTGBot;
class PTGPlayer;
class PTGStaticObj;

struct mousButtons
{
	char lb;
	char rb;
};

//-----------------------------------------------------------------------------
// Name: PTGScene
// Desc: ---------
//-----------------------------------------------------------------------------

class PTGScene
{
private:
	vector<PTGObject *> list;

	vector<HwMaterial *> mats;

	HwCamera *pCam;
	HwCamera *pLCam; //camer for the light
// FOR SHADOW MAPING
	HwTexture *depthMap;
	LPDIRECT3DTEXTURE9 pShadowMap;   
	LPDIRECT3DSURFACE9 pDSShadow; 
//-------------------

	PTGPlayer *pPlayer;

	HwTerrain *terrain;
	HwGrassField *grass;
	HwEntitie *sky;
	HwEntitie *batm;

	vector<PTGStaticObj *> trees;
	vector<PTGBot *> bots;

	void initTerrain();
	void initMaterias();
	void initCams();
	void initSky();
	void initTrees();
	void initBots();

	float dt;
	float tt;


	void INITBATMOBILE();	
public:
	char keys[255];
	mousButtons mb;
	SENDERFUNC objRender;
	SENDERFUNC skyRender;
	void onObjRender(HwSceneObject *sender);
	void onSkyRender(HwSceneObject *sender);
	HwEngine *pEng;

	float getDt();
	vector<PTGBot *> &getBots();
	vector<PTGStaticObj *> &getTrees();

	PTGScene(HWND hWnd);
	void init();
	void update();

	bool isKeyDown(char c);
	bool isKeyPressed(char c);
	bool isMouseDown(char m); //'L R M' -- left right midle
	bool isMousePressed(char m);

	friend PTGObject;
	friend PTGBot;
	friend PTGPlayer;
	friend PTGCreature;
};

//-----------------------------------------------------------------------------
// Name: PTGObject
// Desc: ---------
//-----------------------------------------------------------------------------


class PTGObject
{
protected:
	PTGScene *pScene;

	D3DXVECTOR2 pos;
	float ang;
	
	HwEntitie *mesh;
public:
	PTGObject(PTGScene *scene);

	float distTo(PTGObject *obj);
	float distTo(D3DXVECTOR2 v);
	void turnTo(PTGObject *obj);
	void turnTo(D3DXVECTOR2 v);
	
	void setPos(D3DXVECTOR2 p);
	D3DXVECTOR2 getPos();
		
	PTGScene *getScene();
	
	virtual void process();
};

//-----------------------------------------------------------------------------
// Name: PTGStaticObj
// Desc: ---------
//-----------------------------------------------------------------------------
enum PTGSO_TYPE
{
	PTGST_NONE = 0,
	PTGST_TREE = 1,
};

class PTGStaticObj : public PTGObject
{
public:
	PTGSO_TYPE type;

	PTGStaticObj(PTGScene *scene);
	void init(PTGSO_TYPE t);
	bool busy;
};
//-----------------------------------------------------------------------------
// Name: PTGCreature
// Desc: ---------
//-----------------------------------------------------------------------------

class PTGCreature : public PTGObject
{
protected:
	float moveSpeed;

	string cAnimation;
	float aTime;
	bool isDead;
public:
	PTGCreature(PTGScene *scene);
	void process();
	void moveForward(float dist);	
	void setAnimation(string anim);

	virtual void die();
};
//-----------------------------------------------------------------------------
// Name: PTGBot
// Desc: ---------
//-----------------------------------------------------------------------------

enum PTGBState
{
	PTGBS_MOVE = 0,
	PTGBS_EAT = 1,
	PTGBS_RAWAY = 2,
	PTGBS_SLEEP = 3,
	PTGBS_WALKING = 4,
	//atack
};

class PTGBot : public PTGCreature
{
private:
	D3DXVECTOR2 mPoint; // move point

	PTGBState states[2];
	char curState;

	// BOT STATS
	float fatigue;
	float hunger;

	float sleepingTime;
	float eatingTime;
public:
	bool move();
	void moving();
	void eat();
	void raway();
	void sleep();
	void walking();
	void follow();

	void setDefs();
	void getDecision();
	void reduceStats();
	void findEatingPoint();

	void checkStats();

	MovingEntity mForces;
	SteeringBehaviors *pBehave;

	Panda mStats;
	StateMachine<PTGBot> *FSM;
public:
	PTGBot(PTGScene *scene);

	void process();
	void setRandParams();

	Panda &getStats();
	StateMachine<PTGBot> *getFSM();

	void die();

	friend SteeringBehaviors;
};

//-----------------------------------------------------------------------------
// Name: PTGPlayer
// Desc: ---------
//-----------------------------------------------------------------------------

class PTGPlayer : public PTGCreature
{
private:
	bool batMOBILE;
	int cPanda;
public:
	PTGPlayer(PTGScene *scene);
	void process();
};