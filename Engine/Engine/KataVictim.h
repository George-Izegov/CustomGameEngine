#ifndef _KATAVICTIM_H_
#define _KATAVICTIM_H_

#include "Gameobject.h"
#include "Katamari.h"
class KataVictim : public Gameobject {
public:
	KataVictim();
	~KataVictim();
	//base
	void Load() override;
	void  Unload() override;
	bool Init(HWND, D3DClass*) override;
	bool Init(HWND, std::string model_filename, LPCWSTR texture_filename, Vector3, D3DClass*) override;
	bool  Update() override;
	bool  Update(Katamari*);
	//custom
	void AttachTo(Gameobject*);
	void Place(Vector3);

	ModelClass* m_Model;
	Matrix m_ParentTRS;
	bool m_HasParent = false;
	const float speed = .1f;
};

#endif