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
	HRESULT Init(HWND, ID3D11Device*) override;
	HRESULT Init(HWND, LPCWSTR model_filename, LPCWSTR texture_filename, Vector3, ID3D11Device*) override;
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