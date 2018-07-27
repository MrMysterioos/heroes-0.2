#include "stdafx.h"
#include "InteractiveObject.h"

bool InterObject::Damage(int damage)
{
	_hp -= damage;
	if (_hp <= 0) {
		_hp = 0;
		Destroy();
		return true;
	}
	else {
		return false;
	}
}

void InterObject::Destroy()
{
	_destroy = true;
}