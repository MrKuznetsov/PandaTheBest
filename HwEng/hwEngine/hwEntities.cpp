#include "hwEntities.h"
#include "hwEngine.h"

HwEntitie::HwEntitie(HwEngine *eng) : HwSceneObject(eng)
{
	pMesh = NULL;
	curentFrame = 0;
	updateTime = 0.0f;
	curentAnimation = 0;
	aMode = HWAM_NONE;
	updates = NULL;
}

HwMesh *HwEntitie::loadMesh(string name, string fn)
{
	pMesh = (HwMesh *)getScene()->meshes()->CreateOrRetrieveRes(name, fn);
	return pMesh;
}

void HwEntitie::setMesh(string name)
{
	pMesh = (HwMesh *)getScene()->meshes()->getByName(name);
}

void HwEntitie::setMesh(HwMesh *mesh)
{
	pMesh = mesh;
}

void HwEntitie::draw()
{
	if (pMesh == NULL)
		return;
	pMesh->render(updateTime, curentAnimation, getMaterial()->getPass(0)->getShader());
}

void HwEntitie::setAnimation(string name)
{
	if (!pMesh->isAnimated)
		return;
	int id = pMesh->getSkeleton()->getAnimID(name);
	if (id != -1)
		setAnimation(id);
}

void HwEntitie::setAnimation(int id)
{
	curentAnimation = id;
}

float &HwEntitie::animTime()
{
	return updateTime;
}