#pragma once

#include "StaticObject.h"

class Sprite;

class Tree : public StaticObject {
public:
	static boost::intrusive_ptr<Tree> Create(TMXTiledMap* map);

	void Init(TMXTiledMap* map);

	void SetPosition(const IPoint& position) override;

private:



private:

	Sprite * _sprite;
};

typedef boost::intrusive_ptr<Tree> TreePtr;
