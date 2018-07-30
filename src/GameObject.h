#pragma once
#include "TMXTiledMap.h"

class GameObject : public RefCounter {
public:

	virtual void SetPosition(const IPoint& position) { _position = position; }

	inline IPoint GetPosition() const { return _position; }

	virtual ~GameObject() {};

protected:

	IPoint _position = IPoint(0, 0);

	TMXTiledMapPtr _map = nullptr;

};

typedef boost::intrusive_ptr<GameObject> GameObjectPtr;