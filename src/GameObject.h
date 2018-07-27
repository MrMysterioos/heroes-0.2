#pragma once
#include "Sprite.h"
#include "TMXTiledMap.h"

class GameObject : public RefCounter {
public:

	inline void SetSprite(SpritePtr newSprite) { _sprite = newSprite; };

	virtual ~GameObject() {};

protected:
	SpritePtr _sprite;

	IPoint _position = IPoint(0, 0);

	TMXTiledMapPtr _map = nullptr;

};

typedef boost::intrusive_ptr<GameObject> GameObjectPtr;