#pragma once
#include "GameObject.h"

class InterObject : public GameObject {
public:
	/// ���������� true, ���� ������ ���������
	bool Damage(int damage);
	virtual void Destroy();
protected:
	int _hp = 1;
	bool _destroy = false;
};

typedef boost::intrusive_ptr<InterObject> InterObjectPtr;