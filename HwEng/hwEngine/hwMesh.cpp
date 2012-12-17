#include "hwMesh.h"
#include "hwEngine.h"
#include "hwShaders.h"

#include <iostream>

//-----------------------------------------------------------------------------
// Name: HwSurface
// Desc: ---------
//-----------------------------------------------------------------------------

HwSurface::HwSurface(HwMesh *m)
{
	pParent = m;
	pVB = new HwVertexBuffer(pParent->getManager()->getEngine()->getRender());
	pIB = new HwIndexBuffer(pParent->getManager()->getEngine()->getRender());
//	pVB->usageType() = HWBU_DYNAMIC_WRITE_ONLY;
	vSize = 0;
	iSize = 0;

	aType = HWAT_NONE;
}

void HwSurface::setSize(DWORD vS, DWORD iS)
{
	vSize = vS;
	iSize = iS;

	vertices = new HwMeshVertex[vS];
	indices = new DWORD[iS];
}

void HwSurface::setBoneSize(int index, DWORD size)
{
	vertices[index].boneCount = size;
	vertices[index].bones = new DWORD[size];
	vertices[index].weights = new float[size];
}

void HwSurface::init()
{
	//INIT BUFFERS
	pVB->bufferSize() = vSize;
	if (rendType == HWAR_SOFTWARE)
		pVB->itemSize() = sizeof(HwRendringVertex);
	else
		pVB->itemSize() = sizeof(HwHardRendringVertex);

	pVB->getDecl()->addElement(0, 0, D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_POSITION, 0);
	pVB->getDecl()->addElement(0, 4 * 3, D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_NORMAL, 0);
	pVB->getDecl()->addElement(0, 4 * 6, D3DDECLTYPE_FLOAT2, D3DDECLUSAGE_TEXCOORD, 0);

	if (rendType == HWAR_HARDWARE)
	{
		pVB->getDecl()->addElement(0, 4 * 8, D3DDECLTYPE_UBYTE4, D3DDECLUSAGE_BLENDINDICES, 0);
		pVB->getDecl()->addElement(0, 4 * 9, D3DDECLTYPE_FLOAT4, D3DDECLUSAGE_BLENDWEIGHT, 0);
	}

	pVB->init();

	pIB->bufferSize() = iSize;
	pIB->itemSize() = sizeof(DWORD);
	pIB->iFormat() = D3DFMT_INDEX32;

	pIB->init();

	if (rendType == HWAR_SOFTWARE)
	{
		HwRendringVertex *v = (HwRendringVertex *)pVB->lock();

		for (int i = 0; i < vSize; i++)
		{
			v[i].pos = vertices[i].pos;
			v[i].normal = vertices[i].normal;
			v[i].tex = vertices[i].tex;
		}
		pVB->unlock();
	} else
	{
		HwHardRendringVertex *v = (HwHardRendringVertex *)pVB->lock();

		for (int i = 0; i < vSize; i++)
		{
			v[i].pos = vertices[i].pos;
			v[i].normal = vertices[i].normal;
			v[i].tex = vertices[i].tex;
			for (int j = 0; j < vertices[i].boneCount; j++)
			{
				v[i].indicies[j] = vertices[i].bones[j];
				v[i].weights[j] = vertices[i].weights[j];
			}
			for (int j = vertices[i].boneCount; j < 4; j++)
			{
				v[i].indicies[j] = 0;
				v[i].weights[j] = 0;
			}
		}
		pVB->unlock();
	}

	DWORD *ind = (DWORD *)pIB->lock();
	for (int i = 0; i < iSize; i++)
		ind[i] = indices[i];
	pIB->unlock();
}

//TO DO repalce to hwMATH
void D3DXVector3Mul(D3DXVECTOR3 *pOut, const D3DXMATRIX *m, const D3DXVECTOR3 *v)
{
	pOut->x = m->m[0][0] * v->x + m->m[1][0] * v->y + m->m[2][0] * v->z + m->m[3][0];
	pOut->y = m->m[0][1] * v->x + m->m[1][1] * v->y + m->m[2][1] * v->z + m->m[3][1];
	pOut->z = m->m[0][2] * v->x + m->m[1][2] * v->y + m->m[2][2] * v->z + m->m[3][2];
}

void HwSurface::renderStatic()
{
	pVB->setStreamSource(0);
	pIB->setIndices();
	pParent->getManager()->getEngine()->getRender()->
		drawIndexedPrimitives(D3DPT_TRIANGLELIST, 0, 0, pVB->bufferSize(), 0, pIB->bufferSize() / 3); 
}

void HwSurface::renderAnimHardware(float time)
{
	pVB->setStreamSource(0);
	pIB->setIndices();
	pParent->getManager()->getEngine()->getRender()->
		drawIndexedPrimitives(D3DPT_TRIANGLELIST, 0, 0, pVB->bufferSize(), 0, pIB->bufferSize() / 3); 
}

void HwSurface::renderAnimSoftware(float time)
{
	D3DXMATRIX wMatrix;
	D3DXVECTOR3 vec, n;

	HwRendringVertex *v = (HwRendringVertex *)pVB->lock();
	for (int i = 0; i < vSize; i++)
	{
		v[i].pos = D3DXVECTOR3(0, 0, 0);
		v[i].normal = D3DXVECTOR3(0, 0, 0);
		for (int j = 0; j < vertices[i].boneCount; j++)
		{
			wMatrix = pParent->pSkelet->getUpdates()[vertices[i].bones[j]] * vertices[i].weights[j];
			D3DXVector3Mul(&vec, &wMatrix, &vertices[i].pos);
			D3DXVector3Mul(&n, &wMatrix, &vertices[i].normal);
			v[i].pos += vec;
			v[i].normal += n;
		}
	//	v[i].pos = vertices[i].pos;
		v[i].tex = vertices[i].tex;
		D3DXVec3Normalize(&v[i].normal, &v[i].normal);
		//v[i].normal = vertices[i].normal; //TO DO RECALC NORMAL BUG <---
	}
	pVB->unlock();


	pVB->setStreamSource(0);
	pIB->setIndices();

	pParent->getManager()->getEngine()->getRender()->
		drawIndexedPrimitives(D3DPT_TRIANGLELIST, 0, 0, pVB->bufferSize(), 0, pIB->bufferSize() / 3); 
}

void HwSurface::render(float time)
{
	if (aType == HWAT_NONE)
		renderStatic();
	else if  (aType == HWAT_SKELET)
		if (rendType == HWAR_SOFTWARE)
			renderAnimSoftware(time);
		else
			renderAnimHardware(time);
}

//-----------------------------------------------------------------------------
// Name: HwMeshMaager
// Desc: ------------
//-----------------------------------------------------------------------------

HwResource *HwMeshManager::newRes()
{
	return new HwMesh(this);
}

//-----------------------------------------------------------------------------
// Name: HwMesh
// Desc: ------
//-----------------------------------------------------------------------------

HwMesh::HwMesh(HwResourceManager *m) : HwResource(m)
{
	isAnimated = false;
	rendType = HWAR_SOFTWARE;
}

void HwMesh::setAnimated(bool is)
{
	if (is)
		for (int i = 0; i < list.size(); i++)
			list[i]->aType = HWAT_SKELET;
	else
		for (int i = 0; i < list.size(); i++)
			list[i]->aType = HWAT_NONE;
	isAnimated = is;
}

void HwMesh::init()
{
	for (int i = 0; i < list.size(); i++)
	{
		list[i]->rendType = rendType;
		list[i]->init();
	}
}

void HwMesh::render(float time, int animID, HwShader *pShd)
{
	if (isAnimated)
		pSkelet->update(time, animID, true);

	if (rendType == HWAR_HARDWARE)
		pShd->getPass(0)->vp()->setConst("mWorldMatrixArray", pSkelet->getUpdates(), pSkelet->getBoneCount());

	for (int i = 0; i < list.size(); i++)
		list[i]->render(time);
}
HwSkeleton * HwMesh::getSkeleton()
{
	return pSkelet;
}

HwSurface *HwMesh::newSurface(string name)
{
	HwSurface *pOut = new HwSurface(this);
	list.push_back(pOut);
	return pOut;
}

//________________________________________//
//_________________LOAD()_________________//
//________________________________________//

void HwMesh::load()
{
	f->seekg(0, ios::beg);

	char s[255];
	f->read(s, 4);
	s[4] = '\0';

	if (strcmp(s, "QXRM"))
		return;

	f->seekg(1, ios::cur);
	//read version
	DWORD ver;
	f->read((char *)&ver, sizeof(DWORD));

	if (ver != 1)
		return;
	//read name
	DWORD tmp;
	f->read((char *)&tmp, sizeof(DWORD));
	f->read((char *)s, tmp);
	s[tmp] = '\0';
	name = s;
	f->seekg(1, ios::cur);

	f->read((char *)&tmp, sizeof(DWORD));
	if (tmp == 1)
		loadStatic();
	else if (tmp == 2)
		loadSkin();

	init();
}

void HwMesh::readSub()
{
	DWORD tmp;
	char s[255];
	//name
	f->read((char *)&tmp, sizeof(DWORD));
	f->read((char *)s, tmp);
	f->seekg(1, ios::cur);
	s[tmp] = '\0';

	DWORD vCount, iCount;

	f->read((char *)&vCount, sizeof(DWORD)); //vertex Count
	f->read((char *)&iCount, sizeof(DWORD)); //index Count

	HwSurface *sub = newSurface(s);
	sub->aType = HWAT_NONE;
	sub->setSize(vCount, iCount);

	for (int i = 0; i < vCount; i++)
	{
		f->read((char *)&sub->vertices[i].pos, sizeof(D3DXVECTOR3));
		f->read((char *)&sub->vertices[i].normal, sizeof(D3DXVECTOR3));
		f->read((char *)&sub->vertices[i].tex, sizeof(D3DXVECTOR2));
	}

	for (int i = 0; i < iCount; i++)
		f->read((char *)&sub->indices[i], sizeof(DWORD));
}

void HwMesh::loadStatic()
{
	isAnimated = false;
	DWORD tmp;

	while (true)
	{
		f->read((char *)&tmp, sizeof(DWORD));
		if (tmp == HWMT_SUB)
			readSub();
		else 
			break;
	}
}

void HwMesh::loadSkin()
{
	isAnimated = true;

	DWORD tmp;
	char s[255];
	pSkelet = new HwSkeleton();
	HwSkeletBone *bone;

	DWORD bCount;
	f->read((char *)&bCount, sizeof(DWORD));
	pSkelet->init(bCount);

//BONES -------------------------------
	for (int i = 0; i < bCount; i++)
	{
		//name
		f->read((char *)&tmp, sizeof(DWORD));
		f->read((char *)s, tmp);
		f->seekg(1, ios::cur);
		s[tmp] = '\0';
		bone = pSkelet->newBone(s);
		
		// pos & rot
		D3DXVECTOR3 v;
		D3DXQUATERNION r;
		f->read((char *)&v, sizeof(D3DXVECTOR3));
		f->read((char *)&r, sizeof(D3DXQUATERNION));

		bone->position() = v;
		bone->rotation() = r;
	}
//END BONES ---------------------------

	while (true)
	{
		f->read((char *)&tmp, sizeof(DWORD));
		if (tmp == HWMT_SUB)
			readAnimSub();
		else if (tmp == HWMT_ANIM)
			readAnim();
		else 
			break;
	}
}

void HwMesh::readAnimSub()
{
	DWORD tmp;
	char s[255];
	//name
	f->read((char *)&tmp, sizeof(DWORD));
	f->read((char *)s, tmp);
	f->seekg(1, ios::cur);
	s[tmp] = '\0';

	DWORD vCount, iCount;

	f->read((char *)&vCount, sizeof(DWORD)); //vertex Count
	f->read((char *)&iCount, sizeof(DWORD)); //index Count

	HwSurface *sub = newSurface(s);
	sub->aType = HWAT_SKELET;
	sub->setSize(vCount, iCount);

	for (int i = 0; i < vCount; i++)
	{
		f->read((char *)&sub->vertices[i].pos, sizeof(D3DXVECTOR3));
		f->read((char *)&sub->vertices[i].normal, sizeof(D3DXVECTOR3));
		f->read((char *)&sub->vertices[i].tex, sizeof(D3DXVECTOR2));

		f->read((char *)&tmp, sizeof(DWORD));
		sub->setBoneSize(i, tmp);
		for (int j = 0; j < tmp; j++)
		{
			DWORD tmp2;
			f->read((char *)&tmp2, sizeof(DWORD));
			sub->vertices[i].bones[j] = tmp2;
			float w;
			f->read((char *)&w, sizeof(float));
			sub->vertices[i].weights[j] = w;
		}
	}

	for (int i = 0; i < iCount; i++)
		f->read((char *)&sub->indices[i], sizeof(DWORD));
	
}

void HwMesh::readAnim()
{
	DWORD tmp, fCount;
	char s[255];
	f->read((char *)&fCount, sizeof(DWORD)); //frame count
	//name
	f->read((char *)&tmp, sizeof(DWORD));
	f->read((char *)s, tmp);
	f->seekg(1, ios::cur);
	s[tmp] = '\0';

	HwAnimation *anim = pSkelet->newAnimation(s);
	DWORD bCount = pSkelet->getBoneCount();
	for (int i = 0; i < fCount; i++)
	{
		HwAnimFrame *frame = anim->addFrame(bCount);
		for (int j = 0; j < bCount; j++)
		{
			D3DXVECTOR3 v;
			D3DXQUATERNION r;
			f->read((char *)&v, sizeof(D3DXVECTOR3));
			f->read((char *)&r, sizeof(D3DXQUATERNION));
			frame->position(j) = v;
			frame->rotation(j) = r;
		}

	}
	cout << s << '\n';

}