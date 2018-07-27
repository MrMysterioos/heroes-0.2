#pragma once
#include "GameObject.h"

class InterObject : public GameObject {
public:
	/// Вощвращает true, если объект уничтожен
	bool Damage(int damage);
	virtual void Destroy();
protected:
	int _hp = 1;
	bool _destroy = false;
};

typedef boost::intrusive_ptr<InterObject> InterObjectPtr;