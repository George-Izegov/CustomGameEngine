#ifndef _KATAMARI_H_
#define _KATAMARI_H_

#include "Gameobject.h"
#include "modelclass.h"

class Katamari : public Gameobject {
public:
	Katamari();
	~Katamari();
	//base
	void Load() override;
	void  Unload() override;
	HRESULT Init(HWND, ID3D11Device*) override;
	bool  Update() override;
	//custom
	void Translate(Vector3);

	ModelClass* m_Model;
	const float speed = .1f;
};

#endif