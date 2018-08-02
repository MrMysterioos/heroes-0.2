#include "stdafx.h"
#include "InteractiveObject.h"
#include "TMXTiledMap.h"
#include "Scene.h"

void InterObject::InitHealthBar(float value)
{
	_healthBar = HealthBar::Create().get();
	_healthBar->SetValue(value);
	_healthBar->SetVisibility(_showInfo);
}

bool InterObject::Damage(int damage)
{
	if (_healthBar) {
		float val = (float)damage / _maxHp;
		_healthBar->Damage(val);
	}
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
	Scene::GetInstance().DeleteNode(_healthBar);
}