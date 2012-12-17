#pragma once

#include <d3dx9.h>
#include <vector>

#include "hwRender.h"
#include "hwTypes.h"

using namespace std;

//-----------------------------------------------------------------------------
// Name: HwBuffer
// Desc: --------
//-----------------------------------------------------------------------------

class HwBuffer
{
protected:
	HwRender *pRender;

	DWORD bSize; //buffer size
	DWORD iSize; //item size

	DWORD usage;
public:
	HwBuffer(HwRender *rend);
	virtual ~HwBuffer(){}

	DWORD &bufferSize();
	DWORD &itemSize();
	DWORD &usageType();

	virtual void init() = 0;
	virtual void *lock(DWORD flags = 0, DWORD offset = 0, DWORD size = 0) = 0;
	virtual void unlock() = 0;
};

//-----------------------------------------------------------------------------
// Name: HwVertexBuffer
// Desc: -------------
//-----------------------------------------------------------------------------
/*
	WORD    Stream;     // Stream index
	WORD    Offset;     // Offset in the stream in bytes
	BYTE    Type;       // Data type D3DDECLTYPE
	BYTE    Method;     // Processing method
	BYTE    Usage;      // Semantics
	BYTE    UsageIndex; // Semantic index
*/
class HwVertexDeclaration
{
private:
	vector<D3DVERTEXELEMENT9> list;
public:
	D3DVERTEXELEMENT9 *addElement(const D3DVERTEXELEMENT9 *element);
	D3DVERTEXELEMENT9 *addElement(WORD stream, WORD offset, D3DDECLTYPE type, D3DDECLUSAGE usage, BYTE usageIndex);

	D3DVERTEXELEMENT9 *getDeclArray();

	D3DVERTEXELEMENT9 *getElement(int index);
};

class HwVertexBuffer : public HwBuffer
{
public:
	HwVertexDeclaration decl;
	LPDIRECT3DVERTEXDECLARATION9 pD3DDecl;

	LPDIRECT3DVERTEXBUFFER9 pBuffer;
public:
	HwVertexBuffer(HwRender *rend);

	void init();
	void *lock(DWORD flags = 0, DWORD offset = 0, DWORD size = 0);
	void unlock();

	void setStreamSource(DWORD streamIndex, DWORD offset = 0);

	HwVertexDeclaration *getDecl();
};

//-----------------------------------------------------------------------------
// Name: HwIndexBuffer
// Desc: -------------
//-----------------------------------------------------------------------------
class HwIndexBuffer : public HwBuffer
{
private:
	LPDIRECT3DINDEXBUFFER9 pBuffer;
	D3DFORMAT format;
public:
	HwIndexBuffer(HwRender *rend);

	D3DFORMAT &iFormat();

	void init();
	void *lock(DWORD flags = 0, DWORD offset = 0, DWORD size = 0);
	void unlock();

	void setIndices();
};