#include <Windows.h>
#include <tchar.h>
#include <mmsystem.h>
#include <d3d9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )
#include <iostream>
#include <vector>

#include "pandaTheGame.h"

#include "..\HwEng\hwEngine\hwEngine.h"
#include "..\HwEng\hwEngine\hwMisc.h"
#include "..\HwEng\hwEngine\hwSkeleton.h"
#include "..\HwEng\hwEngine\hwBuffers.h"
#include "..\HwEng\hwEngine\hwMesh.h"
#include "..\HwEng\hwEngine\hwTerrain.h"
#include "..\HwEng\hwEngine\hwEntities.h"
#include "..\HwEng\hwEngine\hwShaders.h"
HINSTANCE hInstance; HINSTANCE hPrevInst; 
LPWSTR lpszArgs; int nWinMode;

HwEngine *eng;
HwTexture *tex, *hm, *ht, *smt;
HwMaterial /**mat, *tmat,*/ *skymat;
HwShader *shd, *smshd, *tshd, *skyshd;
HwMesh *mesh;
HwTerrain *terr;
HwEntitie *en, *sky, *batm;
HwEntitie *en2;
HwCamera *lightCam, *cam;

vector<HwMaterial *> mats;

//HwIndexBuffer iB;
HwVertexBuffer *vB;



D3DLIGHT9                       g_Light;   
LPDIRECT3DTEXTURE9              g_pShadowMap = NULL;    // Texture to which the shadow map is rendered
LPDIRECT3DSURFACE9              g_pDSShadow = NULL;     // Depth-stencil buffer for rendering to shadow map
float                           g_fLightFov;            // FOV of the spot light (in radian)
D3DXMATRIXA16                   g_mShadowProj;          // Projection matrix for shadow map




float tx, ty;
bool mc = false;
bool wd = false;
bool qd = false;
bool ed = false;
float attack;
int mx, my;
int x, y;
DWORD fps;

float mspeed = 0.5;

PTGScene *ptg;

//----------------------------------------------------------------------------\\
//------------------------- SET MATERIALS -------- ----------------------------\\
//-----------------------------------------------------------------------------\\

void setMaterials()
{
	for (int i = 0; i < 3; i++)
	{
		HwMaterial *mat = (HwMaterial *)(eng->getScene()->materials()->createImpl("Mat01"));

		mat->newTechnique()->newPass();//For DEPTH MAP
		mat->newTechnique()->newPass();//ANOTHER RENDER
		mat->getTechnique(1)->getPass(0)->newStage();//->setTexture(tex);
		mat->getTechnique(1)->getPass(0)->newStage()->setTexture(smt);
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
	mats[0]->getTechnique(1)->getPass(0)->getStage(0)->setTexture(tex);
	mats[1]->getTechnique(1)->getPass(0)->getStage(0)->setTexture(ht);
	mats[2]->getTechnique(1)->getPass(0)->getStage(0)->setTextureFromFile("batm", "data\\batm.tga");
	mats[1]->getTechnique(1)->getPass(0)->newStage()->setTextureFromFile("grass", "data\\grass.jpg");
	mats[1]->getTechnique(1)->getPass(0)->newStage()->setTextureFromFile("rock", "data\\rock.jpg");
	mats[1]->getTechnique(1)->getPass(0)->newStage()->setTextureFromFile("sand", "data\\sand.jpg");
	/*
	tmat =  (HwMaterial *)(eng->getScene()->materials()->createImpl("Mat02"));
	tmat->newTechnique()->newPass();//For DEPTH MAP
	tmat->newTechnique()->newPass();//ANOTHER RENDER
	tmat->getTechnique(1)->getPass(0)->newStage()->setTexture(ht);
	tmat->getTechnique(1)->getPass(0)->newStage()->setTexture(smt);
	{
		HwStage *stage = tmat->getTechnique(1)->getPass(0)->getStage(0);
		stage->min = D3DTEXF_POINT;
		stage = mat->getTechnique(1)->getPass(0)->getStage(1);
		stage->mag = D3DTEXF_POINT;
		stage->min = D3DTEXF_POINT;
		stage->addressU = D3DTADDRESS_CLAMP;
		stage->addressW = D3DTADDRESS_CLAMP;
	}
	tmat->getTechnique(1)->getPass(0)->newStage()->setTextureFromFile("grass", "data\\grass.jpg");
	tmat->getTechnique(1)->getPass(0)->newStage()->setTextureFromFile("rock", "data\\rock.jpg");
	tmat->getTechnique(1)->getPass(0)->newStage()->setTextureFromFile("sand", "data\\sand.jpg");

	*/
	skymat = (HwMaterial *)(eng->getScene()->materials()->createImpl("SkyMat01"));
	skymat->newTechnique()->newPass()->newStage()->setTextureFromFile("NOISE", "data\\NoiseVolume.dds");
	{
		HwStage *stage = skymat->getPass(0)->getStage(0);
		stage->mip = D3DTEXF_LINEAR;;
		stage->mag = D3DTEXF_LINEAR;
		stage->min = D3DTEXF_LINEAR;
	}

	skyshd = (HwShader *)(eng->getScene()->shaders()->createImpl("SkySHD01"));
	skyshd->newTechnique()->newPass();
	skyshd->getPass(0)->setVPFromFile("vp3", "data\\sky_vs.fx");
	skyshd->getPass(0)->setPPFromFile("pp3", "data\\sky_ps.fx");


	shd = (HwShader *)(eng->getScene()->shaders()->createImpl("SHD01"));
	shd->newTechnique()->newPass();
	shd->getPass(0)->setVPFromFile("vp0", "data\\sm_vs.fx");
	shd->getPass(0)->setPPFromFile("pp0", "data\\sm_ps.fx");

	tshd = (HwShader *)(eng->getScene()->shaders()->createImpl("SHD03"));
	tshd->newTechnique()->newPass();
	tshd->getPass(0)->setVPFromFile("vp0", "data\\sm_vs.fx");
	tshd->getPass(0)->setPPFromFile("pp0", "data\\sm_tps.fx");

	smshd = (HwShader *)(eng->getScene()->shaders()->createImpl("SHD02"));
	smshd->newTechnique()->newPass();
	smshd->getPass(0)->setVPFromFile("vp1", "data\\sm_svs.fx");
	smshd->getPass(0)->setPPFromFile("pp1", "data\\sm_sps.fx");
	

	mats[0]->getTechnique(0)->getPass(0)->setShader(smshd);
	mats[0]->getTechnique(1)->getPass(0)->setShader(shd);
	mats[1]->getTechnique(0)->getPass(0)->setShader(smshd);
	mats[1]->getTechnique(1)->getPass(0)->setShader(tshd);
	mats[2]->getTechnique(0)->getPass(0)->setShader(smshd);
	mats[2]->getTechnique(1)->getPass(0)->setShader(shd);
	skymat->getPass(0)->setShader(skyshd);
}

//----------------------------------------------------------------------------\\
//------------------------- ------------ -------- ----------------------------\\
//-----------------------------------------------------------------------------\\

void OnSkyRender(HwSceneObject *sender)
{
	ptg->onSkyRender(sender);
	/*D3DXMATRIX m =  ptg->pEng->getScene()->getCamera()->viewM() * ptg->pEng->getScene()->getCamera()->projM();
	skyshd->getPass(0)->vp()->setConst("matViewProjection", m);

	skyshd->getPass(0)->pp()->setConst("cloudColor", D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
	skyshd->getPass(0)->pp()->setConst("skyColor", D3DXVECTOR4(0.0f, 0.496f , 1.0f, 1.0f));
	skyshd->getPass(0)->pp()->setConst("sunColor", D3DXVECTOR4(1.0f, 0.8f , 0.4f , 1.0f));
	skyshd->getPass(0)->pp()->setConst("noiseScale", 4.1f);
	skyshd->getPass(0)->pp()->setConst("noiseBias",-1.5f);
	skyshd->getPass(0)->pp()->setConst("sunFallOff", 3.5f);
	skyshd->getPass(0)->pp()->setConst("sunSharpness", 0.42f);
	skyshd->getPass(0)->pp()->setConst("cloudSpeed", 0.05f);
	skyshd->getPass(0)->pp()->setConst("noiseSpeed", 0.2f);
	skyshd->getPass(0)->pp()->setConst("scale", 3.0f);
	skyshd->getPass(0)->pp()->setConst("time_0_X",  timeGetTime() / 3000.0f);*/
}



void OnObjRender(HwSceneObject *sender)
{
	ptg->onObjRender(sender);
	/*
	D3DXMATRIX m;
	if (mats[0]->getCurrTechn() == 0)
	{
		m = sender->worldMatrix() * lightCam->viewM();
		smshd->getPass(0)->vp()->setConst("g_mWorldView", m);
		smshd->getPass(0)->vp()->setConst("g_mProj", g_mShadowProj);
	}
	else
	{

	HwShader *cshd = sender->getMaterial()->getPass(0)->getShader();
	if (cshd == NULL)
		return;

	D3DXMATRIX mLightView = lightCam->viewM();

	D3DXMATRIXA16 mViewToLightProj = cam->viewM();
	D3DXMatrixInverse( &mViewToLightProj, NULL, &mViewToLightProj );
	mViewToLightProj = mViewToLightProj *  mLightView * g_mShadowProj;

	cshd->getPass(0)->vp()->setConst("g_mViewToLightProj", mViewToLightProj);
	m = sender->worldMatrix() * cam->viewM();
	cshd->getPass(0)->vp()->setConst("g_mWorldView", m);
	cshd->getPass(0)->vp()->setConst("g_mProj", cam->projM());
	
	cshd->getPass(0)->pp()->setConst("g_fCosTheta", cosf(g_Light.Theta));
	cshd->getPass(0)->pp()->setConst( "g_vLightDiffuse", D3DXVECTOR4(1, 1, 1, 1.0));
	cshd->getPass(0)->pp()->setConst( "g_vMaterial", D3DXVECTOR4(1, 1, 1, 1.0));
	cshd->getPass(0)->pp()->setConst( "g_vLightAmbient", D3DXVECTOR4(0.3, 0.3, 0.3, 1.0));

	D3DXVECTOR3 v = lightCam->getEye();
	D3DXVECTOR4 v4;
	D3DXVec3Transform( &v4, &v, &cam->viewM());
	cshd->getPass(0)->pp()->setConst( "g_vLightPos", v4);
	v4 = D3DXVECTOR4(lightCam->getDirection(), 0);
	D3DXVec4Transform( &v4, &v4, &cam->viewM() );  // Direction in view space
	D3DXVec3Normalize( ( D3DXVECTOR3* )&v4, ( D3DXVECTOR3* )&v4 );
	cshd->getPass(0)->pp()->setConst( "g_vLightDir", v4);

	}*/
}

void OnRender()
{
	LPDIRECT3DDEVICE9 g_pd3dDevice = eng->getRender()->getDevice();

	D3DXMATRIXA16 matWorld;
	float iTime = timeGetTime() / 75.0f;

	if (!mc)
		attack = iTime;
	if (mc)
	{
		en->setAnimation("Attack");
		iTime  = iTime - attack;
		cout << en->distantioTo(en2) << "\n";
		if (en->distantioTo(en2) < 1)
		{
			en2->setAng(en2->getAng());
			en2->getAng().x = -2.3;
			en2->getPos().y =  terr->getHeight(-35, -45) + 0.7;
		}
	}

	fps = timeGetTime();

	en->animTime() = iTime;
	en2->animTime() = iTime;

}

void OnCreate()
{

	ptg->objRender = OnObjRender;
	ptg->skyRender = OnSkyRender;
	ptg->init();
	/*
	eng->init();
	eng->setOnRender(OnRender);
	mc = false;

	cam = eng->getScene()->getCamera();

	tex = (HwTexture *)(eng->getScene()->textures()->CreateOrRetrieveRes("Banan", "panda.tga"));
	hm = (HwTexture *)(eng->getScene()->textures()->CreateOrRetrieveRes("HM", "HM.bmp"));
	ht = (HwTexture *)(eng->getScene()->textures()->CreateOrRetrieveRes("TERRAIN", "data\\HM.bto"));
	smt = (HwTexture *)(eng->getScene()->textures()->createImpl("SMTEX"));

	//===================================================================
	g_fLightFov = D3DX_PI / 2.0f;
	g_Light.Range = 10.0f;
	g_Light.Theta = g_fLightFov / 2.0f;
	g_Light.Phi = g_fLightFov / 2.0f;

	lightCam = new HwCamera(eng);


	LPDIRECT3DDEVICE9 pd3dDevice = eng->getRender()->getDevice();
	pd3dDevice->CreateTexture( 1024, 1024, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &g_pShadowMap, NULL ) ;
	pd3dDevice->CreateDepthStencilSurface( 1024, 1024, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, TRUE, &g_pDSShadow, NULL ) ;
	D3DXMatrixPerspectiveFovLH( &g_mShadowProj, g_fLightFov, 1, 0.01f, 1000.0f );
	smt->p2DTexture = g_pShadowMap;
	smt->texType = HWTEX_2D;


	setMaterials();


	//tmat->getPass(0)->renderOptions.fillMode = D3DFILL_WIREFRAME;
	HwCamera *camera = eng->getScene()->getCamera();
	camera->setCameraType(HWCT_RECT);


	en = new HwEntitie(eng);
	mesh = en->loadMesh("Mesh01", "2.qxr3d");
	en2 = new HwEntitie(eng);
	en2->loadMesh("Mesh01", "1.qxr3d");
	sky = new HwEntitie(eng);
	sky->loadMesh("SKY", "data\\sky.qxr3d");
	en->setRenderFunc(OnObjRender);
	en2->setRenderFunc(OnObjRender);
	sky->setRenderFunc(OnSkyRender);

	batm = new HwEntitie(eng);
	batm->loadMesh("Batm", "data\\batm.qxr3d")->setAnimated(false);
	batm->setRenderFunc(OnObjRender);
	batm->setMaterial(mats[2]);
	//batm->setVisible(false);
	

//	for (int i = 0; i < sky->pMesh->list[0]->vSize; i++)
	//{
	//	sky->pMesh->list[0]->vertices[i].tex = D3DXVECTOR2(sky->pMesh->list[0]->vertices[i].normal.x / 2 + 0.5, sky->pMesh->list[0]->vertices[i].normal.y / 2 + 0.5);
	//}

	terr = new HwTerrain(eng);
	terr->init();
	terr->setRenderFunc(OnObjRender);
	ifstream f("data\\HM.txt", ios::in);
	for (int i = 0; i < 256; i++)
		for (int j = 0; j <256; j++)
		{
			float h = 0.0;
			f >> h;
			terr->heightMap(j, 256 - i) = h;
		}
	f.close();

	terr->reInit();

	batm->setPos(D3DXVECTOR3(30, terr->getHeight(30, 50)+1, 50));
	
	//terr->setHeightFromTexture(hm, 100);

	terr->buildTree();

	en->setMaterial(mats[0]);
	en2->setMaterial(mats[0]);
	terr->setMaterial(mats[1]);
	sky->setMaterial(skymat);

	en2->setPos(D3DXVECTOR3(-10, terr->getHeight(-10, -15) + 1, -15));

	lightCam->setAt(D3DXVECTOR3( 0.0f,  3, 0.0f ));
	lightCam->setEye(D3DXVECTOR3( 5,  20, 5 ));
	lightCam->init();
	lightCam->update();
	//eng->getScene()->setCamera(lightCam);
	*/
}

void OnUpdate()
{
	ShowCursor(FALSE);
	HwCamera *camera = ptg->pEng->getScene()->getCamera();
	ptg->update();

/*	POINT p;
	GetCursorPos(&p);
	x = p.x;
	y = p.y;
	tx = (x - 333) / 80.0f;
	ty = (y - 333) / 80.0f;

	camera->turnAround(tx, ty, 9);

	SetCursorPos(333, 333);*/

/*
	tx = 0;
	ty = 0;
	POINT p;
	GetCursorPos(&p);
	ShowCursor(FALSE);
	x = p.x;
	y = p.y;
	//if (mc)
	{
		tx = (x - 333) / 80.0f;
		ty = (y - 333) / 80.0f;
	}
	HwCamera *camera = cam;


		camera->turnAround(tx, ty, 9);

		D3DXVECTOR3 dir = camera->getDirection();
		float ax = atan2(dir.x, dir.z);

		SetCursorPos(333, 333);

		en->setAng(en->getAng());
		en->getAng().y = ax;


	if (!wd)
		en->setAnimation("Idle");
	else
		en->setAnimation("Run");

	mx = x;
	my = y;

	en->setPos(en->getPos());
	en->getPos().y = terr->getHeight(en->getPos().x, en->getPos().z) + 1;
	camera->setAt(en->getPos());
	camera->getAt().y = en->getPos().y + 2;
	lightCam->setAt(en->getPos());
	lightCam->setEye(en->getPos() + D3DXVECTOR3(5, 10, 10));
	if (qd)
	{
		en->setAng(en->getAng());
		en->getAng().y -= 0.5;
	}
	if (ed)
	{
		en->setAng(en->getAng());
		en->getAng().y += 0.5;
	}

	if (wd && !mc)
		en->moveForward(mspeed);

	en = batm;

	LPDIRECT3DDEVICE9 pd3dDevice = eng->getRender()->getDevice();

	LPDIRECT3DSURFACE9 pOldRT = NULL;
	pd3dDevice->GetRenderTarget( 0, &pOldRT );
	LPDIRECT3DSURFACE9 pShadowSurf;

	g_pShadowMap->GetSurfaceLevel( 0, &pShadowSurf );
	pd3dDevice->SetRenderTarget( 0, pShadowSurf );

	LPDIRECT3DSURFACE9 pOldDS = NULL;
	pd3dDevice->GetDepthStencilSurface( &pOldDS );
	pd3dDevice->SetDepthStencilSurface( g_pDSShadow );
	{
		for (int i = 0; i < 3; i++)
			mats[i]->setTechnique(0);
		eng->getScene()->setCamera(lightCam);
		sky->setVisible(false);
		eng->render();
	}

	pd3dDevice->SetDepthStencilSurface( pOldDS );
	pd3dDevice->SetRenderTarget( 0, pOldRT );
	{
		for (int i = 0; i < 3; i++)
			mats[i]->setTechnique(1);
		eng->getScene()->setCamera(cam);
		sky->setVisible(true);
		eng->render();
	}*/
}


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

	UINT key = ( UINT )wParam;
	switch( msg )
	{
		case WM_MOUSEMOVE:
			x = ( short )LOWORD( lParam );
			y = ( short )HIWORD( lParam );
			return 0;
		case WM_DESTROY:
		  //  Cleanup();
			PostQuitMessage( 0 );
			return 0;
		case WM_LBUTTONDOWN:
			ptg->mb.lb = 2;
			return 0;
		case WM_LBUTTONUP:
			ptg->mb.lb = 0;
			return 0;
		case WM_KEYDOWN:
			if (key < 255)
				ptg->keys[key] = 2;
			return 0;
		case WM_KEYUP:
			if (key < 255)
				ptg->keys[key] = 0;
			return 0;
	//	case WM_PAINT:
		//	OnUpdate();
		//	ValidateRect( hWnd, NULL );
	//		return 0;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	UNREFERENCED_PARAMETER( hInst );

	WNDCLASSEX wc =
	{
		sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
		L"D3D Engine Test", NULL
	};
	RegisterClassEx( &wc );

	HWND hWnd = CreateWindow( L"D3D Engine Test", L"D3D Engine Test",
							  WS_OVERLAPPEDWINDOW, 25, 25, 1000, 700,
							  NULL, NULL, wc.hInstance, NULL );

//---------------------------
//	eng = new HwEngine(hWnd);
	ptg = new PTGScene(hWnd);
//---------------------------

	OnCreate();
	//if( SUCCEEDED( eng->init() ) )
	//{
		ShowWindow( hWnd, SW_SHOWDEFAULT );
		UpdateWindow( hWnd );

		MSG msg;
		ZeroMemory( &msg, sizeof( msg ) );
		while( msg.message != WM_QUIT )
		{
			if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			OnUpdate();
		}
	//}

	UnregisterClass( L"D3D Engine Test", wc.hInstance );
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	wWinMain(hInstance,  hPrevInst, 
		 lpszArgs,  nWinMode);
	return 0;
}



