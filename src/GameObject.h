#pragma once
#include "AnimateSprite.h"
#include "TMXTiledMap.h"

class GameObject : public RefCounter {
public:

	//inline void SetSprite(SpritePtr newSprite) { _sprite = newSprite; };
	inline void SetAnimate(AnimateSpritePtr newAnimate) { _animate = newAnimate; }
	inline void SetPosition(const IPoint& position) { _position = position; }

	//inline SpritePtr GetSprite() const { return _sprite; }
	inline AnimateSpritePtr GetAnimate() const { return _animate; }
	inline IPoint GetPosition() const { return _position; }

	virtual ~GameObject() {};

protected:
	//SpritePtr _sprite = nullptr;
	
	AnimateSpritePtr _animate = nullptr;

	IPoint _position = IPoint(0, 0);

	TMXTiledMapPtr _map = nullptr;

};

typedef boost::intrusive_ptr<GameObject> GameObjectPtr;