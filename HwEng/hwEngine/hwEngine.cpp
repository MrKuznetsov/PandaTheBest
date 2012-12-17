#include "hwEngine.h"

HwEngine::~HwEngine(void)
{
	delete pRender;
}

HwRender *HwEngine::getRender()
{
	return pRender;
}

HwScene *HwEngine::getScene()
{
	return pScene;
}

void HwEngine::setOnRender(FUNC f)
{
	onRender = f;
}

HRESULT HwEngine::init()
{
	HRESULT init = pRender->initD3D(hw);
	pScene->init();
	return init;
}


void HwEngine::render()
{
	pRender->begin();
	if (onRender != NULL)
		onRender();
	pScene->render();

	pRender->end();
}

