#ifndef _GAMEPLANE_H_
#define _GAMEPLANE_H_

#include "Gameobject.h"
#include "Katamari.h"
class Gameplane : public Gameobject {
public:
	Gameplane();
	~Gameplane();
	//base
	void Load() override;
	void  Unload() override;
	bool Init(HWND, D3DClass*) override;
	bool  Update() override;

	ModelClass* m_Model;
};

#endif