#pragma once
#include "GameObject.h"
#include "HealthBar.h"

class InterObject : public GameObject {
public:
	void InitHealthBar(float value);
	/// Вощвращает true, если объект уничтожен
	virtual bool Damage(int damage);
	virtual void Destroy();
	inline bool IsDestroy() const { return _destroy; }

	inline bool IsDestroy() { return _destroy; };

	inline void ShowInfo() { 
		_showInfo = true;
		_healthBar->SetVisibility(true);
	};

	inline void HideInfo() {
		_showInfo = false;
		_healthBar->SetVisibility(false);
	};

protected:
	int _hp = 1;
	int _maxHp = 1;
	bool _destroy = false;

	bool _showInfo = false;

	HealthBar* _healthBar;
};

typedef boost::intrusive_ptr<InterObject> InterObjectPtr;