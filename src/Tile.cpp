#include "stdafx.h"
#include "Tile.h"
#include "Scene.h"

TilePtr Tile::Create(Area area) {
	TilePtr ptr(new Tile);
	ptr->Init(area);
	return ptr;
}

void Tile::Init(Area area) {
	Sprite::Init();
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

	Scene scene = Scene::GetInstance();
	scene.DeleteNode(_effNode);

	_effNode.reset();
	_effNode = ParticleEffectNode::Create(effCont);
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