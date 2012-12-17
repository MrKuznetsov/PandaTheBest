#pragma once

#include <d3dx9.h>

#include "hwRender.h"
#include "hwScene.h"
#include "hwMisc.h"

//-----------------------------------------------------------------------------
// Name: HwEngine
// Desc: --------
//-----------------------------------------------------------------------------
class HwEngine
{
private:
	HWND hw;
	HwRender *pRender;
	HwScene *pScene;

	FUNC onRender;
public:
	HwEngine(HWND wnd) : hw(wnd)
	{
		pRender = new HwRender;
		pScene = new HwScene(this);
		onRender = NULL;
	}

	HwRender *getRender();
	HwScene *getScene();

	void setOnRender(FUNC f);

	HRESULT init();
	void render();


	~HwEngine(void);
};

