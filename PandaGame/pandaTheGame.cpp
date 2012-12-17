#include "pandaTheGame.h"

#include <iostream>

PTGScene::PTGScene(HWND hWnd)
{
	pEng = new HwEngine(hWnd);
	srand ( time(NULL) );
}

void PTGScene::init()
{
	for (int i = 0; i < 255; i++)
		keys[i] = false;
	mb.lb = 0;
	mb.rb = 0;

	pEng->init();
	initCams();
	initMaterias();
	initTerrain();
	initSky();
	initTrees();
	initBots();
	
	INITBATMOBILE(); //!!!!!!!!!!! OLOLOL
	
	pPlayer = new PTGPlayer(this);

	dt = timeGetTime();
	tt = dt;

}

void PTGScene::update()
{
	dt = (timeGetTime() - tt) / 100.0f;
	tt = timeGetTime();

	//cout << 1000.0f / dt << endl;

	pPlayer->process();
	for (int i = 0; i < bots.size(); i++)
		bots[i]->process();
	
	LPDIRECT3DDEVICE9 pd3dDevice = pEng->getRender()->getDevice();

	LPDIRECT3DSURFACE9 pOldRT = NULL;
	pd3dDevice->GetRenderTarget( 0, &pOldRT );
	LPDIRECT3DSURFACE9 pShadowSurf;

	pShadowMap->GetSurfaceLevel( 0, &pShadowSurf );
	pd3dDevice->SetRenderTarget( 0, pShadowSurf );

	LPDIRECT3DSURFACE9 pOldDS = NULL;
	pd3dDevice->GetDepthStencilSurface( &pOldDS );
	pd3dDevice->SetDepthStencilSurface( pDSShadow );
	{
		for (int i = 0; i < NUM_MATS; i++)
			mats[i]->setTechnique(0);
		pEng->getScene()->setCamera(pLCam);
		if (!ISSHADOW)
		{
			pEng->getRender()->begin();
			pEng->getRender()->end();
		}
		else
		{
			sky->setVisible(false);
			grass->setVisible(false);
			pEng->render();
		}
	}
	
	pd3dDevice->SetDepthStencilSurface( pOldDS );
	pd3dDevice->SetRenderTarget( 0, pOldRT );
	{
		for (int i = 0; i < NUM_MATS; i++)
			mats[i]->setTechnique(1);
		pEng->getScene()->setCamera(pCam);
		sky->setVisible(true);
		grass->setVisible(true);
		pEng->render();
	}

	if (mb.lb == 2)
		mb.lb = 1;
}

void PTGScene::initCams()
{
	pCam = pEng->getScene()->getCamera();
	pLCam = new HwCamera(pEng);
	pLCam->setNearDist(0.01f);
	pLCam->setFov(D3DX_PI / 2.0f);
	pEng->getRender()->setPerspectiveFovLH(&pLCam->projM(), pLCam->getFov(), 1.0f, pLCam->getNearDist(), pLCam->getFarDist());
	pLCam->setEye(D3DXVECTOR3(5, 15, 5));
}

void PTGScene::initSky()
{
	sky = new HwEntitie(pEng);
	sky->loadMesh("SKY", "data\\sky.qxr3d");
	sky->setMaterial("SKY");
	sky->setRenderFunc(skyRender);
}

void PTGScene::INITBATMOBILE()
{
	batm = new HwEntitie(pEng);
	batm->loadMesh("BATM", "data\\batm.qxr3d")->setAnimated(false);
	batm->setMaterial("BATMAN");
	batm->setRenderFunc(objRender);

	D3DXVECTOR3 v = BMLOL;

	batm->setPos(v);
	batm->getPos().y = terrain->getHeight(v.x, v.z) + 1.0f;
}

void PTGScene::initTerrain()
{
	terrain = new HwTerrain(pEng);
	terrain->init();
	ifstream f("data\\HM.txt", ios::in);
	for (int i = 0; i < 256; i++)
		for (int j = 0; j <256; j++)
		{
			float h = 0.0;
			f >> h;
			terrain->heightMap(j, 256 - i) = h;
		}
	f.close();
	terrain->reInit();
	terrain->buildTree();

	terrain->setRenderFunc(objRender);

	terrain->setMaterial("TERRAIN");

	grass = new HwGrassField(pEng);
	terrain->setGrassField(grass);
	
	HwTexture *pBlend = (HwTexture *)pEng->getScene()->textures()->getByName("TerrainTex");

	grass->init();
	for (int i = 0; i < 256; i++)
		for (int j = 0; j < 256; j++)
			grass->Map(i, 255 - j) = pBlend->getPixelColor(int(i / 255.0f * 1023.0f), int(j / 255.0f * 1023.0f)).r > 0.8 ? rand() % 3 : 0;
	grass->reInit();

	grass->setMaterial("GRASS");
	//grass->setRenderFunc(objRender);
	//grass->setVisible(false);

}

void PTGScene::initTrees()
{
	for (int i = 0; i < 100; i++)
	{
		PTGStaticObj *tree = new PTGStaticObj(this);
		float x = rand() % 200;
		float y = rand() % 200;

		tree->setPos(D3DXVECTOR2(x - 100, y - 100));
		tree->init(PTGST_TREE);
		tree->process();
		trees.push_back(tree);
	}
}

void PTGScene::initBots()
{
	HwMesh *m = (HwMesh *)pEng->getScene()->meshes()->createImpl("PANDA01");
	m->rendType = HWAR_HARDWARE;
	m->loadFromFile("2.qxr3d");

	for (int i = 0; i < NUM_BOTS; i++)
	{
		PTGBot *bot = new PTGBot(this);
		float x = rand() % 200;
		float y = rand() % 200;
		/*
		//================================
		if (i == 0)
			bot->FSM->SetCurrentState(Walk::Instance());
		//================================
		*/
		bot->setPos(D3DXVECTOR2(x - 100, y - 100));
		bot->setRandParams();
		bots.push_back(bot);
	}
}


void  PTGScene::initMaterias()
{
	depthMap = (HwTexture *)(pEng->getScene()->textures()->createImpl("SMTEX"));
	LPDIRECT3DDEVICE9 pd3dDevice = pEng->getRender()->getDevice();
	pd3dDevice->CreateTexture( 1024, 1024, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &pShadowMap, NULL ) ;
	pd3dDevice->CreateDepthStencilSurface( 1024, 1024, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, TRUE, &pDSShadow, NULL ) ;
	depthMap->p2DTexture = pShadowMap;


	for (int i = 0; i < NUM_MATS; i++)
	{
		HwMaterial *mat = (HwMaterial *)(pEng->getScene()->materials()->createImpl("MAT"));
		mat->newTechnique()->newPass();//For DEPTH MAP
		mat->newTechnique()->newPass();//ANOTHER RENDER
		mat->getTechnique(1)->getPass(0)->newStage();//->setTexture(tex);
		mat->getTechnique(1)->getPass(0)->newStage()->setTexture(depthMap);
		{
			HwStage *stage = mat->getTechnique(1)->getPass(0)->getStage(0);
			stage->min = D3DTEXF_POINT;
			stage = mat->getTechnique(1)->getPass(0)->getStage(1);
			stage->mag = D3DTEXF_POINT;
			stage->min = D3DTEXF_POINT;
			stage->addressU = D3DTADDRESS_CLAMP;
			stage->addressW = D3DTADDRESS_CLAMP;
		}
		mats.push_back(mat);
	}

	HwMaterial *mat = (HwMaterial *)(pEng->getScene()->materials()->createImpl("GRASS"));
	mat->newTechnique()->newPass()->newStage()->setTextureFromFile("GRASSTEX", "data\\grass2.tga");
	mat->getPass(0)->blend.alphaRef = 90;//(DWORD)0x000000FF;
	mat->getPass(0)->blend.src = D3DBLEND_SRCALPHA;
	mat->getPass(0)->blend.dest = D3DBLEND_INVSRCALPHA;
	mat->getPass(0)->blend.func = D3DCMP_GREATEREQUAL;
	mat->getPass(0)->getStage(0)->addressU = D3DTADDRESS_CLAMP;
	mat->getPass(0)->getStage(0)->addressV = D3DTADDRESS_CLAMP;

	HwShader *shd = (HwShader *)(pEng->getScene()->shaders()->createImpl("CREATURESHD"));
	shd->newTechnique()->newPass();
	shd->getPass(0)->setVPFromFile("vp0", "data\\sm_vs.fx");
	shd->getPass(0)->setPPFromFile("pp0", "data\\sm_ps.fx");

	shd = (HwShader *)(pEng->getScene()->shaders()->createImpl("TERRAINSHD"));
	shd->newTechnique()->newPass();
	shd->getPass(0)->setVPFromFile("vp1", "data\\sm_vs.fx");
	shd->getPass(0)->setPPFromFile("pp1", "data\\sm_tps.fx");

	shd = (HwShader *)(pEng->getScene()->shaders()->createImpl("DEPTHSHD"));
	shd->newTechnique()->newPass();
	shd->getPass(0)->setVPFromFile("vp2", "data\\sm_svs.fx");
	shd->getPass(0)->setPPFromFile("pp2", "data\\sm_sps.fx");

	shd = (HwShader *)(pEng->getScene()->shaders()->createImpl("SkySHD01"));
	shd->newTechnique()->newPass();
	shd->getPass(0)->setVPFromFile("vp3", "data\\sky_vs.fx");
	shd->getPass(0)->setPPFromFile("pp3", "data\\sky_ps.fx");

	shd = (HwShader *)(pEng->getScene()->shaders()->createImpl("ANIMSHD"));
	shd->newTechnique()->newPass();
	shd->getPass(0)->setVPFromFile("vp4", "data\\sm_avs.fx");
	shd->getPass(0)->setPPFromFile("pp4", "data\\sm_ps.fx");

	shd = (HwShader *)(pEng->getScene()->shaders()->createImpl("ADEPTHSHD"));
	shd->newTechnique()->newPass();
	shd->getPass(0)->setVPFromFile("vp5", "data\\sm_asvs.fx");
	shd->getPass(0)->setPPFromFile("pp5", "data\\sm_sps.fx");


	mat = mats[0]; // PANDA MATERIAL
	mat->setName("PANDA");
	mat->getTechnique(1)->getPass(0)->getStage(0)->setTextureFromFile("PandaTex", "data\\panda.tga");
	mat->getTechnique(0)->getPass(0)->setShader("ADEPTHSHD");
	mat->getTechnique(1)->getPass(0)->setShader("ANIMSHD");

	mat = mats[1];
	mat->setName("TERRAIN");
	mat->getTechnique(1)->getPass(0)->getStage(0)->setTextureFromFile("TerrainTex", "data\\HH.tga");
	mat->getTechnique(1)->getPass(0)->newStage()->setTextureFromFile("grass", "data\\Grass_Diffuse.tga");
	mat->getTechnique(1)->getPass(0)->newStage()->setTextureFromFile("rock", "data\\rock.jpg");
	mat->getTechnique(1)->getPass(0)->newStage()->setTextureFromFile("sand", "data\\Sand_Diffuse.tga");
	mat->getTechnique(1)->getPass(0)->getStage(0)->mip = D3DTEXF_NONE;
	for (int i = 1; i < 4; i++)
	{
		mat->getTechnique(1)->getPass(0)->getStage(i)->min = D3DTEXF_ANISOTROPIC;
		mat->getTechnique(1)->getPass(0)->getStage(i)->mag = D3DTEXF_ANISOTROPIC;
		mat->getTechnique(1)->getPass(0)->getStage(i)->mip = D3DTEXF_NONE;
	}
	mat->getTechnique(0)->getPass(0)->setShader("DEPTHSHD");
	mat->getTechnique(1)->getPass(0)->setShader("TERRAINSHD");

	mat = mats[2];
	mat->setName("BATMAN");
	mat->getTechnique(1)->getPass(0)->getStage(0)->setTextureFromFile("BatmanTex", "data\\batm.tga");
	mat->getTechnique(0)->getPass(0)->setShader("DEPTHSHD");
	mat->getTechnique(1)->getPass(0)->setShader("CREATURESHD");

	mat = mats[3];
	mat->setName("TREE1"); // TREE MATERIAl
	mat->getTechnique(1)->getPass(0)->getStage(0)->setTextureFromFile("TreeTex", "data\\tree01.tga");
	mat->getTechnique(0)->getPass(0)->setShader("DEPTHSHD");
	mat->getTechnique(1)->getPass(0)->setShader("CREATURESHD");

	mat = mats[4];
	mat->setName("TREE2"); // TREE2 MATERIAl
	mat->getTechnique(1)->getPass(0)->getStage(0)->setTextureFromFile("TreeTex2", "data\\tree02.tga");
	mat->getTechnique(0)->getPass(0)->setShader("DEPTHSHD");
	mat->getTechnique(1)->getPass(0)->setShader("CREATURESHD");

	mat = mats[5];
	mat->setName("TREE3"); // TREE3 MATERIAl
	mat->getTechnique(1)->getPass(0)->getStage(0)->setTextureFromFile("TreeTex2", "data\\tree03.tga");
	mat->getTechnique(0)->getPass(0)->setShader("DEPTHSHD");
	mat->getTechnique(1)->getPass(0)->setShader("CREATURESHD");

	mat = mats[6]; // PANDA MATERIAL
	mat->setName("PANDADEAD");
	mat->getTechnique(1)->getPass(0)->getStage(0)->setTextureFromFile("PandaTex", "data\\pandadead.tga");
	mat->getTechnique(0)->getPass(0)->setShader("ADEPTHSHD");
	mat->getTechnique(1)->getPass(0)->setShader("ANIMSHD");

	mat = mats[7]; // PANDA MATERIAL
	mat->setName("BAT");
	mat->getTechnique(1)->getPass(0)->getStage(0)->setTextureFromFile("PandaTex", "data\\BatmanArmoured_Body_D.tga");
	mat->getTechnique(0)->getPass(0)->setShader("ADEPTHSHD");
	mat->getTechnique(1)->getPass(0)->setShader("ANIMSHD");

	mat = (HwMaterial *)(pEng->getScene()->materials()->createImpl("SKY"));
	mat->newTechnique()->newPass()->newStage()->setTextureFromFile("Noise", "data\\NoiseVolume.dds");
	mat->getPass(0)->setShader("SkySHD01");


}

void PTGScene::onSkyRender(HwSceneObject *sender)
{
	HwShader *shd = sender->getMaterial()->getPass(0)->getShader();
	D3DXMATRIX m =  pCam->viewM() * pCam->projM();
	shd->getPass(0)->vp()->setConst("matViewProjection", m);

	shd->getPass(0)->pp()->setConst("cloudColor", D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
	shd->getPass(0)->pp()->setConst("skyColor", D3DXVECTOR4(0.0f, 0.496f , 1.0f, 1.0f));
	shd->getPass(0)->pp()->setConst("sunColor", D3DXVECTOR4(1.0f, 0.8f , 0.4f , 1.0f));
	shd->getPass(0)->pp()->setConst("noiseScale", 4.1f);
	shd->getPass(0)->pp()->setConst("noiseBias",-1.5f);
	shd->getPass(0)->pp()->setConst("sunFallOff", 3.5f);
	shd->getPass(0)->pp()->setConst("sunSharpness", 0.42f);
	shd->getPass(0)->pp()->setConst("cloudSpeed", 0.05f);
	shd->getPass(0)->pp()->setConst("noiseSpeed", 0.2f);
	shd->getPass(0)->pp()->setConst("scale", 3.0f);
	shd->getPass(0)->pp()->setConst("time_0_X",  timeGetTime() / 3000.0f);
}

void PTGScene::onObjRender(HwSceneObject *sender)
{
	HwMaterial *mat = sender->getMaterial();
	HwShader *shd = mat->getPass(0)->getShader();
	HwCamera *cam = pEng->getScene()->getCamera();
	if (shd == NULL)
		return;

	if (mat->getCurrTechn() == 0)
	{
		D3DXMATRIX wvm = sender->worldMatrix() * cam->viewM();
		shd->getPass(0)->vp()->setConst("g_mWorldView", wvm);
		shd->getPass(0)->vp()->setConst("g_mProj", cam->projM());
	}
	else
	{
		D3DXMATRIX mLightView = pLCam->viewM();

		D3DXMATRIXA16 mViewToLightProj = cam->viewM();
		D3DXMatrixInverse( &mViewToLightProj, NULL, &mViewToLightProj );
		mViewToLightProj = mViewToLightProj *  mLightView * pLCam->projM();

		shd->getPass(0)->vp()->setConst("g_mViewToLightProj", mViewToLightProj);
		D3DXMATRIX wvm = sender->worldMatrix() * cam->viewM();
		shd->getPass(0)->vp()->setConst("g_mWorldView", wvm);
		shd->getPass(0)->vp()->setConst("g_mProj", cam->projM());
	
		shd->getPass(0)->pp()->setConst("g_fCosTheta", cosf(D3DX_PI / 4));
		shd->getPass(0)->pp()->setConst( "g_vLightDiffuse", D3DXVECTOR4(1, 1, 1, 1.0));
		shd->getPass(0)->pp()->setConst( "g_vMaterial", D3DXVECTOR4(1, 1, 1, 1.0));
		shd->getPass(0)->pp()->setConst( "g_vLightAmbient", D3DXVECTOR4(0.3, 0.3, 0.3, 1.0));

		D3DXVECTOR3 v = pLCam->getEye();
		D3DXVECTOR4 v4;
		D3DXVec3Transform( &v4, &v, &cam->viewM());
		shd->getPass(0)->pp()->setConst( "g_vLightPos", v4);
		v4 = D3DXVECTOR4(pLCam->getDirection(), 0);
		D3DXVec4Transform( &v4, &v4, &cam->viewM() );  // Direction in view space
		D3DXVec3Normalize( ( D3DXVECTOR3* )&v4, ( D3DXVECTOR3* )&v4 );
		shd->getPass(0)->pp()->setConst( "g_vLightDir", v4);	
	}
}

float PTGScene::getDt()
{
	return dt;
}

vector<PTGBot *> &PTGScene::getBots() 
{
	return bots;
}

vector<PTGStaticObj *> &PTGScene::getTrees() 
{
	return trees;
}

bool PTGScene::isKeyDown(char c)
{
	return keys[c];
}
bool PTGScene::isKeyPressed(char c)
{
	return keys[c] == 2;
}

bool PTGScene::isMouseDown(char m)
{
	if (m == 'L')
		return mb.lb;
	if (m == 'R')
		return mb.rb;
}
bool PTGScene::isMousePressed(char m)
{
	if (m == 'L')
		return mb.lb == 2;
	if (m == 'R')
		return mb.rb == 2;
}

//-----------------------------------------------------------------------------
// Name: PTGObject
// Desc: ---------
//-----------------------------------------------------------------------------

PTGObject::PTGObject(PTGScene *scene)
{
	pScene = scene;
	mesh = new HwEntitie(scene->pEng);
	//mesh->setVisible(false);
	pos.x = 0;
	pos.y = 0;
	ang = 0;

	//
}

inline float sqr(float a)
{
	return a * a;
}


float PTGObject::distTo(PTGObject *obj)
{
	return sqrt(sqr(pos.x - obj->pos.x) + sqr(pos.y - obj->pos.y));
}

float PTGObject::distTo(D3DXVECTOR2 v)
{
	return sqrt(sqr(pos.x - v.x) + sqr(pos.y - v.y));
}

void PTGObject::turnTo(PTGObject *obj)
{
	turnTo(obj->pos);
}

void PTGObject::turnTo(D3DXVECTOR2 v)
{
	ang = atan2(v.x - pos.x, v.y - pos.y);
}

void PTGObject::setPos(D3DXVECTOR2 p)
{
	pos = p;
}

PTGScene *PTGObject::getScene()
{
	return pScene;
}

D3DXVECTOR2 PTGObject::getPos()
{
	return pos;
}

void PTGObject::process()
{
	mesh->setPos(D3DXVECTOR3(pos.x, pScene->terrain->getHeight(pos.x, pos.y) + 1.0f, pos.y));
	mesh->setAng(D3DXVECTOR3(0, ang, 0));
}

//-----------------------------------------------------------------------------
// Name: PTGStaticObj
// Desc: ---------
//-----------------------------------------------------------------------------

PTGStaticObj::PTGStaticObj(PTGScene *scene) : PTGObject(scene)
{
	type = PTGST_NONE;
}

void PTGStaticObj::init(PTGSO_TYPE t)
{
	type = t;
	if (t == PTGST_TREE)
	{
		string s = to_string(long long(rand() % 3 + 1));
		mesh->loadMesh("TREE", "data\\tree0"+s+".qxr3d")->setAnimated(false);
		mesh->setMaterial("TREE" + s);
		mesh->setRenderFunc(pScene->objRender);
	}
}

//-----------------------------------------------------------------------------
// Name: PTGCreature
// Desc: ---------
//-----------------------------------------------------------------------------
PTGCreature::PTGCreature(PTGScene *scene) : PTGObject(scene)
{
	moveSpeed = 0.3f;
	aTime = 0.0f;
	isDead = false;
}

void PTGCreature::moveForward(float dist)
{
	float dx = dist * sin(ang);
	float dz = dist * cos(ang);
	pos.x += dx;
	pos.y += dz;
}

void PTGCreature::die()
{
	isDead = true;
}

void PTGCreature::process()
{
	if (isDead)
	{
		aTime = 0;
		setAnimation("Sleep");
	}

	mesh->animTime() = aTime ;
	aTime += pScene->dt;

	PTGObject::process();
}

void PTGCreature::setAnimation(string anim)
{
	cAnimation = anim;
	mesh->setAnimation(anim);
}

//-----------------------------------------------------------------------------
// Name: PTGBot
// Desc: ---------
//-----------------------------------------------------------------------------

PTGBot::PTGBot(PTGScene *scene) : PTGCreature(scene)
{
	if (ISANIM)
		mesh->loadMesh("PANDA01", "2.qxr3d")->setAnimated(false);
	else
	{
		mesh->loadMesh("PANDA01", "2.qxr3d");

	}
	mesh->setRenderFunc(pScene->objRender);
	mesh->setMaterial("PANDA");
	//mesh->setVisible(false);

	pBehave = new SteeringBehaviors(this);
	FSM = new StateMachine<PTGBot>(this);
	FSM->SetCurrentState(Sleeping::Instance());
	FSM->ChangeState(Walk::Instance());

	curState = 0;
	setDefs();
	getDecision();
}

void PTGBot::setDefs()
{
	eatingTime = 0;
	sleepingTime = 0;
}

void PTGBot::moving()
{	
	static int  i = -1;
	i++;
		mForces.m_vVelocity.Zero();
		pBehave->m_vTarget = Vector2D(mPoint);
		pBehave->m_vFleeTarget = Vector2D(pScene->pPlayer->getPos());
		
		Vector2D m_vSteeringForce = pBehave->Calculate();
		Vector2D acceleration = m_vSteeringForce / mForces.m_dMass;

		mForces.m_vVelocity += acceleration * pScene->dt;
		mForces.m_vVelocity.Truncate(mForces.m_dMaxSpeed);

		mForces.m_vVelocity /= mForces.m_dMass;
		
		//mForces.m_vPosition += mForces.m_vVelocity * pScene->dt;
		
		if (mForces.m_vVelocity.LengthSq() > 0.00000001) 
		{
			mForces.m_vHeading = mForces.m_vVelocity;
			mForces.m_vHeading.Normalize();
			mForces.m_vSide = mForces.m_vHeading.Perp();
		}

		pos.x += mForces.m_vVelocity.x * pScene->dt;
		pos.y += mForces.m_vVelocity.y * pScene->dt;
		/*
		pos.x += (mForces.m_vVelocity.x + mForces.m_vOldVelocity.x * 10) / 11 * pScene->dt;
		pos.y += (mForces.m_vVelocity.y + mForces.m_vOldVelocity.y * 10) / 11 * pScene->dt;
		*/
		pBehave->PandaOverlap();
		pBehave->TreeOverlap();
}

bool PTGBot::move()
{
	setAnimation("Run");
	if (distTo(mPoint) > 1.5f)
	{	
		moving();
		//turnTo(mPoint);
		setAnimation("Run");
		//moveForward(moveSpeed * pScene->dt);
		return true;
	} 
	else
	{
		return false;
		curState++;
		setDefs();
	}
}

void PTGBot::checkStats()
{
	if (hunger > 100)
		hunger = 100;
	if (fatigue > 100)
		fatigue = 100;
}

void PTGBot::setRandParams()
{
	hunger = 20 + rand() % 80;
	fatigue = 20 + rand() & 80;
}

Panda &PTGBot::getStats()
{
	return mStats;
}

StateMachine<PTGBot> *PTGBot::getFSM()
{
	return FSM;
}

void PTGBot::reduceStats()
{
	hunger -= pScene->dt * 0.2;
	fatigue -= pScene->dt * 0.1;
}

void PTGBot::getDecision()
{
	curState = 0;
	setDefs();
	if (hunger < 40 && fatigue > 30)
	{
		states[0] = PTGBS_MOVE;
		states[1] = PTGBS_EAT;
		findEatingPoint();
	}
	else if (fatigue < 30)
	{
		states[0] = PTGBS_SLEEP;
	} else
	{
		states[0] = PTGBS_MOVE;
		states[1] = PTGBS_WALKING;
		mPoint = D3DXVECTOR2(pos.x + rand() % 50 - 25, pos.y + rand() % 50 - 25);
	}
}

void PTGBot::findEatingPoint()
{
	int mini = 0;
	for (int i = 1; i < pScene->trees.size(); i++)
	{
		PTGStaticObj *tree = pScene->trees[i];
		Vector2D v =  Vector2D(tree->getPos()) - Vector2D(getPos());
		v.Normalize();
		float dist = v.DistanceSq(mForces.m_vHeading);
		//double RelativeHeading = mForces.m_vHeading.Dot(v)
	//	if ( v.Dot(mForces.m_vHeading) > 0 && RelativeHeading < -0.95) 
		if (dist < 1)
			if (distTo(tree) < distTo(pScene->trees[mini]))
				mini = i;
	}
	mPoint =  pScene->trees[mini]->getPos();
}

void PTGBot::eat()
{
//	if (eatingTime < 100) //in seconds
//	{
//		hunger += pScene->dt * 0.6;
		setAnimation("Eat");
//	}
//	else
	//	getDecision();

//	eatingTime += pScene->dt;
}

void PTGBot::raway()
{
	getDecision();
}

void PTGBot::sleep()
{
//	if (sleepingTime < 150) //in seconds
//	{
//		fatigue += pScene->dt * 0.6;
		setAnimation("Sleep");
//	}
//	else
//		getDecision();

//	sleepingTime += pScene->dt;
}

void PTGBot::walking()
{
	setAnimation("Run");
	//cout << "walking" << pos.x << " " <<pos.y <<"\n";
	moving();
//	mPoint = D3DXVECTOR2(pos.x + rand() % 50 - 25, pos.y + rand() % 50 - 25);
//	move();
//	getDecision();
}

void PTGBot::follow()
{
	setAnimation("Run");
	//cout << "walking" << pos.x << " " <<pos.y <<"\n";
	moving();
//	mPoint = D3DXVECTOR2(pos.x + rand() % 50 - 25, pos.y + rand() % 50 - 25);
//	move();
//	getDecision();
}

void PTGBot::die()
{
	PTGCreature::die();
	mesh->setMaterial("PANDADEAD");
}

void PTGBot::process()
{
	PTGCreature::process(); 

	if (isDead)
		return;

	ang = atan2(mForces.m_vHeading.x, mForces.m_vHeading.y);
	//mForces.m_dMass = (100 - mStats.m_iHunger) / 100.0 + 1.0f; 

	FSM->Update();
/*
	checkStats();
	reduceStats();

	switch(states[curState])
	{
	case PTGBS_MOVE:
		move();
		break;
	case PTGBS_EAT:
		eat();
		break;
	case PTGBS_RAWAY:
		raway();
		break;
	case PTGBS_SLEEP:
		sleep();
		break;
	case PTGBS_WALKING:
		walking();
		break;
	}*/
}

//-----------------------------------------------------------------------------
// Name: PTGPlayer
// Desc: ---------
//-----------------------------------------------------------------------------

PTGPlayer::PTGPlayer(PTGScene *scene) : PTGCreature(scene)
{
	HwMesh *m = (HwMesh *)pScene->pEng->getScene()->meshes()->createImpl("PLAYER01");
	m->rendType = HWAR_HARDWARE;
	m->loadFromFile("1.qxr3d");
	mesh->setMesh(m);
	mesh->setRenderFunc(pScene->objRender);
	mesh->setMaterial("BAT");

	batMOBILE = false;
	cPanda = 0;
}

void PTGPlayer::process()
{
	setAnimation("Idle");
	if (pScene->isKeyDown('W'))
	{
		setAnimation("Run");
		moveForward(moveSpeed * pScene->dt);
	}
	if (pScene->isKeyDown('S'))
	{
		setAnimation("Run");
		moveForward(-moveSpeed * pScene->dt);
	}

	if (pScene->isMouseDown('L'))
	{
		if (pScene->isMousePressed('L'))
			aTime = 0;
		setAnimation("Attack");

		for (int i = 0; i < pScene->bots.size(); i++)
			if (distTo(pScene->bots[i]) <1.0)
				pScene->bots[i]->die();
	}

	if (pScene->isKeyPressed('N'))
	{
		cPanda++;
		D3DXVECTOR2 pjjs = pScene->bots[cPanda]->getPos();
		cout << pjjs.x << " " << pjjs.y << endl;

	}
	int index = cPanda;


	D3DXVECTOR2 ps = pScene->bots[index]->getPos();
	//ps.y = pScene->terrain->getHeight(ps.x, ps.z) + 3.0f;
	
	pScene->pCam->setAt(D3DXVECTOR3(pos.x, pScene->terrain->getHeight(pos.x, ps.y) + 3.0f, pos.y));
	
	if (distTo(D3DXVECTOR2(pScene->batm->getPos().x, pScene->batm->getPos().z)) < 1.0f && pScene->isKeyDown('E') && !batMOBILE)
	{
		mesh->setVisible(false);
		mesh = pScene->batm;
		moveSpeed = 1;
		batMOBILE = true;
	}

	if (batMOBILE)
	{
		for (int i = 0; i < pScene->bots.size(); i++)
			if (distTo(pScene->bots[i]) <1.0)
			{
				pScene->bots[i]->die();
				//задавили бедолажку

			}
	}


	POINT p;
	GetCursorPos(&p);
	int x = p.x;
	int y = p.y;
	float tx = (x - 333) / 80.0f;
	float ty = (y - 333) / 80.0f;
	pScene->pCam->turnAround(tx, ty, 9);

	if (ISSHADOW)
	{
		pScene->pLCam->setAt(pScene->pCam->getAt());
		pScene->pLCam->setEye(pScene->pCam->getAt() + D3DXVECTOR3(10, 10, 10));
	}
	else
	{
		pScene->pLCam->setEye(D3DXVECTOR3(1000, 1000, 1000));
		pScene->pLCam->setAt(D3DXVECTOR3(10000, 10000, 10000));
	}

	SetCursorPos(333, 333);

	D3DXVECTOR3 dir = pScene->pCam->getDirection();
	ang = atan2(dir.x, dir.z);

	aTime += pScene->dt * 2;

	PTGCreature::process(); 
}