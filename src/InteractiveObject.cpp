#include "stdafx.h"
#include "InteractiveObject.h"
#include "TMXTiledMap.h"

void InterObject::InitHealthBar()
{
	_healthBar = HealthBar::Create();
}

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