#pragma once
#include "AnimateSprite.h"

class TMXTiledMap;

class GameObject : public RefCounter {
public:

	virtual void SetPosition(const IPoint& position) { _position = position; }

	inline IPoint GetPosition() const { return _position; }

	virtual void Update(float dt) {}

	virtual ~GameObject() {};

protected:

	IPoint _position = IPoint(0, 0);

	TMXTiledMap * _map = nullptr;
};

typedef boost::intrusive_ptr<GameObject> GameObjectPtr;