#include "stdafx.h"
#include "Tree.h"
#include "TMXTiledMap.h"


boost::intrusive_ptr<Tree> Tree::Create(TMXTiledMap* map)
{
	TreePtr ret(new Tree);
	ret->Init(map);
	return ret;
}

void Tree::Init(TMXTiledMap* map)
{
	StaticObject::Init(map);
	_sprite = Sprite::Create().get();
	Render::Texture* tex = Core::resourceManager.Get<Render::Texture>("Tree");
	_sprite->SetTexture(tex);
	_sprite->SetAnchorPoint(FPoint(0.55f, 0.05f));
	SetPosition(_position);
}

void Tree::SetPosition(const IPoint& point) {
	_position = point;
	IPoint pos = _map->GetSceneCoordinate(point);
	_sprite->SetPosition(math::Vector3(pos.x, pos.y, pos.y - 200));
}