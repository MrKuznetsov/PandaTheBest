#include "hwBuffers.h"

#include <iostream>


//-----------------------------------------------------------------------------
// Name: HwBuffer
// Desc: --------
//-----------------------------------------------------------------------------

HwBuffer::HwBuffer(HwRender *rend)
{
	pRender = rend;
	bSize = 0;
	iSize = 0;
	usage = 0;
}

DWORD &HwBuffer::bufferSize()
{
	return bSize;
}

DWORD &HwBuffer::itemSize()
{
	return iSize;
}

DWORD &HwBuffer::usageType()
{
	return usage;
}
//-----------------------------------------------------------------------------
// Name: HwVerteDecl
// Desc: -------------
//-----------------------------------------------------------------------------

D3DVERTEXELEMENT9 *HwVertexDeclaration::addElement(const D3DVERTEXELEMENT9 *element)
{
	list.push_back(*element);
	return &list[list.size() - 1];
}

D3DVERTEXELEMENT9 *HwVertexDeclaration::addElement(WORD stream, WORD offset, D3DDECLTYPE type, D3DDECLUSAGE usage, BYTE usageIndex)
{
	D3DVERTEXELEMENT9 elem;
	elem.Stream = stream;
	elem.Offset = offset;
	elem.Type = type;
	elem.Usage = usage;
	elem.UsageIndex = usageIndex;
	elem.Method = D3DDECLMETHOD_DEFAULT;

	list.push_back(elem);
	return &list[list.size() - 1];
}

D3DVERTEXELEMENT9 *HwVertexDeclaration::getElement(int index)
{
	if (index < 0 || index > list.size() - 1)
		return NULL;
	return &list[index];
}


D3DVERTEXELEMENT9 *HwVertexDeclaration::getDeclArray()
{
	D3DVERTEXELEMENT9 *pOut = new D3DVERTEXELEMENT9[list.size() + 1];
	for (int i = 0; i < list.size(); i++)
		pOut[i] = list[i];

	D3DVERTEXELEMENT9 last = D3DDECL_END();
	pOut[list.size()] = last;

	return pOut;
};
//-----------------------------------------------------------------------------
// Name: HwVertexBuffer
// Desc: -------------
//-----------------------------------------------------------------------------

HwVertexBuffer::HwVertexBuffer(HwRender *rend) : HwBuffer(rend)
{
	iSize = 3 * sizeof(float);
	bSize = 1;
	usage = HWBU_STATIC;
	pBuffer = NULL;
	pD3DDecl = NULL;
}

void HwVertexBuffer::init()
{
	pRender->getDevice()->CreateVertexBuffer(bSize * iSize, usage, 0, D3DPOOL_MANAGED, &pBuffer, NULL);//D3DPOOL_MANAGED

	D3DVERTEXELEMENT9 *pDecl = decl.getDeclArray();
	pRender->getDevice()->CreateVertexDeclaration(pDecl, &pD3DDecl);
	delete[] pDecl;		
}

void *HwVertexBuffer::lock(DWORD flags, DWORD offset, DWORD size)
{
	void *data;
	pBuffer->Lock(offset, size, &data, flags);

	return data;
}

void HwVertexBuffer::unlock()
{
	pBuffer->Unlock();
}

void HwVertexBuffer::setStreamSource(DWORD streamIndex, DWORD offset)
{
	pRender->getDevice()->SetStreamSource(streamIndex, pBuffer, offset, iSize);
	pRender->getDevice()->SetVertexDeclaration(pD3DDecl);
}

HwVertexDeclaration *HwVertexBuffer::getDecl()
{
	return &decl;
}

//-----------------------------------------------------------------------------
// Name: HwIndexBuffer
// Desc: -------------
//-----------------------------------------------------------------------------

HwIndexBuffer::HwIndexBuffer(HwRender *rend) : HwBuffer(rend)
{
	iSize = sizeof(DWORD);
	bSize = 1;
	usage = HWBU_WRITEONLY;
	format = D3DFMT_INDEX16;
	pBuffer = NULL;
}

D3DFORMAT &HwIndexBuffer::iFormat()
{
	return format;
}

void HwIndexBuffer::init()
{
	pRender->getDevice()->CreateIndexBuffer(iSize * bSize, usage, format, D3DPOOL_MANAGED, &pBuffer, NULL);
}

void *HwIndexBuffer::lock(DWORD flags, DWORD offset, DWORD size)
{
	void *data;
	pBuffer->Lock(offset, size, &data, flags);

	return data;
}

void HwIndexBuffer::unlock()
{
	pBuffer->Unlock();
}


void HwIndexBuffer::setIndices()
{
	pRender->getDevice()->SetIndices(pBuffer);
}