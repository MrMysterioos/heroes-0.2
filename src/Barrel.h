#pragma once

#include "InteractiveObject.h"

class Sprite;

class Barrel : public InterObject {
public:
	static boost::intrusive_ptr<Barrel> Create(TMXTiledMap* map, std::string element);

	void SetPosition(const IPoint& pos) override;

	void Destroy() override;

private:

	void Init(TMXTiledMap* map, std::string element);

private:

	/// Элемент в который окрашивается карта при разрушении бочки
	std::string _elem;

	Sprite* _sprite;

};

typedef boost::intrusive_ptr<Barrel> BarrelPtr;