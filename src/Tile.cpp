#include "stdafx.h"
#include "Tile.h"
#include "Scene.h"

TilePtr Tile::Create(Scene* scene, Area area) {
	TilePtr ptr(new Tile);
	ptr->Init(scene, area);
	return ptr;
}

void Tile::Init(Scene* scene, Area area) {
	Sprite::Init(scene);
	ChangeArea(area);
}

void Tile::ChangeArea(Area area)
{
	SetTexture(area.texture);
	EffectsContainer effCont;

	if (area.effect == "None") {
		return;
	}

	auto eff = effCont.AddEffect(area.effect);
	float rand = (float)(std::rand() % 100) / 10.f;
	eff->Update(rand);
	_effNode.reset();
	_effNode = ParticleEffectNode::Create(_scene, effCont);
	math::Vector3 pos = _position;
	pos.z -= 256;
	_effNode->SetPosition(pos);
}

void Tile::SetPosition(math::Vector3 pos)
{
	Sprite::SetPosition(pos);
	if (_effNode) {
		pos.z -= 250;
		_effNode->SetPosition(pos);
	}
}