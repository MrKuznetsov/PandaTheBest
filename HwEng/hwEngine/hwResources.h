#pragma once

#include <string>
#include <vector>
#include <fstream>

#include "hwMisc.h"

using namespace std;


class HwResourceManager;
class HwEngine;

//-----------------------------------------------------------------------------
// Name: HwResource
// Desc: ----------
//-----------------------------------------------------------------------------
class HwResource
{
protected:
//	int id;

	string name;
	string filename;
	fstream *f;

	HwResourceManager *pManager;
public:
	HwResource(HwResourceManager *m);
	virtual ~HwResource();

	void loadFromFile(string fn);

	virtual void load(){}
	virtual void unload(){}

	unsigned int getSize();
	HwResourceManager *getManager();
	void setName(string s);
	string getName();

	friend HwResourceManager;

};

//-----------------------------------------------------------------------------
// Name: HwResourceManager
// Desc: -----------------
//-----------------------------------------------------------------------------
class HwResourceManager : public HwObject
{
private:
	vector<HwResource *> resources;
	//HwEngine *pEngine;
public:
	HwResourceManager(HwEngine *eng);

	virtual HwResource *newRes();
	HwResource *createImpl(string name);
	HwResource *createRes(string name, string fn);
	HwResource *CreateOrRetrieveRes(string name, string fn);

	int indexOf(string name);
	int indexOf(HwResource *r);
	HwResource *getByName(string name);

	unsigned int getCount();

	friend HwResource;
};