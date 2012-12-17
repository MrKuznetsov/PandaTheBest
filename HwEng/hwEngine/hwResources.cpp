
#include "hwResources.h"
#include "hwEngine.h"

//-----------------------------------------------------------------------------
// Name: HwResource
// Desc: ----------
//-----------------------------------------------------------------------------

HwResource::HwResource(HwResourceManager *m)
{
	pManager = m;
	name = "";

	m->resources.push_back(this);
	//id = m->resources.size() - 1;

	f = new fstream;
}

HwResource::~HwResource()
{
	delete f;
}

void HwResource::loadFromFile(string fn)
{
	f->open(fn, ios::in | ios::binary);
	filename = fn;
	load();
	f->close();
}

unsigned int HwResource::getSize() //not safe!!!!
{
	f->seekg (0, ios::end);
	unsigned int size = f->tellg();
	f->seekg (0, ios::beg);

	return size;
}

HwResourceManager *HwResource::getManager()
{
	return pManager;
}

void HwResource::setName(string s)
{
	name = s;
}

string HwResource::getName()
{
	return name;
}

//-----------------------------------------------------------------------------
// Name: HwResourceManager
// Desc: -----------------
//-----------------------------------------------------------------------------

HwResourceManager::HwResourceManager(HwEngine *eng) : HwObject(eng)
{

}

int HwResourceManager::indexOf(string name)
{
	vector<HwResource *>::iterator it = resources.begin();
	int i = 0;
	while (it != resources.end())
	{
		 if ((*it)->name == name)
			 return i;
		++it;
		++i;
	}
	return -1;
}

int HwResourceManager::indexOf(HwResource *r)
{
	vector<HwResource *>::iterator it = resources.begin();
	int i = 0;
	while (it != resources.end())
	{
		 if ((*it) == r)
			 return i;
		++it;
		++i;
	}
	return -1;
}

HwResource *HwResourceManager::getByName(string name)
{
	vector<HwResource *>::iterator it = resources.begin();
	while (it != resources.end())
	{
		if ((*it)->name == name)
			 return (*it);
		++it;
	}
	return 0;
}

inline unsigned int HwResourceManager::getCount()
{
	return resources.size();
}

HwResource *HwResourceManager::newRes()
{
	return new HwResource(this);
}

HwResource *HwResourceManager::createImpl(string name)
{
	HwResource *res = newRes();
	res->name = name;
	return res;
}

HwResource *HwResourceManager::createRes(string name, string fn)
{
	HwResource *res = createImpl(name);
	res->loadFromFile(fn);
	return res;
}

HwResource *HwResourceManager::CreateOrRetrieveRes(string name, string fn)
{
	for (int i = 0; i < resources.size(); i++)
		if (resources[i]->filename == fn)
			return resources[i];
	return createRes(name, fn);
}

