#include "stdafx.h"

#include "Barrel.h"
#include "TMXTiledMap.h"
#include "Scene.h"
#include "ParticleEffectNode.h"

boost::intrusive_ptr<Barrel> Barrel::Create(TMXTiledMap * map, std::string element)
{
	BarrelPtr ret(new Barrel);
	ret->Init(map, element);
	return ret;
}

void Barrel::Init(TMXTiledMap* map, std::string element)
{
	_hp = _maxHp = 3;

	InterObject::Init(map);
	InitHealthBar(1.0f);
	_elem = element;

	Render::Texture* tex = Core::resourceManager.Get<Render::Texture>("Barrel");
	_sprite = Sprite::Create().get();
	_sprite->SetTexture(tex);
	auto scenePos = _map->GetSceneCoordinate(_position);
	_sprite->SetPosition(math::Vector3(scenePos.x, scenePos.y, scenePos.y));
	_sprite->SetAnchorPoint(FPoint(0.55f, 0.1f));
}

void Barrel::SetPosition(const IPoint& point)
{
	_position = point;
	IPoint pos = _map->GetSceneCoordinate(point);
	_sprite->SetPosition(math::Vector3(pos.x, pos.y, pos.y - 200));
	IPoint barPos = pos;
	barPos.x = pos.x - 50;
	barPos.y = pos.y + 150;
	_healthBar->SetPosition(math::Vector3(barPos.x, barPos.y, barPos.y - 500));
}

void Barrel::Destroy()
{
	InterObject::Destroy();
	std::vector<IPoint> points = {
		_position,
		_map->GetAdjacentAreaCoords(_position, Direction::North),
		_map->GetAdjacentAreaCoords(_position, Direction::NorthEast),
		_map->GetAdjacentAreaCoords(_position, Direction::NorthWest),
		_map->GetAdjacentAreaCoords(_position, Direction::South),
		_map->GetAdjacentAreaCoords(_position, Direction::SouthEast),
		_map->GetAdjacentAreaCoords(_position, Direction::SouthWest)
	};

	for (auto point : points) {
		_map->AddElement(_elem, point);
		auto obj = _map->GetGameObject(point);
		auto interObj = dynamic_cast<InterObject*>(obj.get());
		if (interObj && interObj != this) {
			interObj->Damage(1);
		}
	}

	EffectsContainer effCont;
	auto eff = effCont.AddEffect("Explosion_simple");
	eff->Reset();
	ParticleEffectNode* effPtr = ParticleEffectNode::Create(effCont).get();
	effPtr->SetPosition(_sprite->GetPosition());
	effPtr->SetAutoDelete(true);

	Scene::GetInstance().DeleteNode(_sprite);
	_map->EraseGameObject(this);

}