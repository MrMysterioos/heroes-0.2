#pragma once
#include "Sprite.h"
#include "ParticleEffectNode.h"

struct Area {
	std::string name;

	Render::TexturePtr texture;
	std::string effect;

};

class Tile : public Sprite {
public:
	static boost::intrusive_ptr<Tile> Create(Scene* scene, Area area);
	void Init(Scene* scene, Area area);

	void ChangeArea(Area area);

	void SetPosition(math::Vector3 pos) override;

private:
	ParticleEffectNodePtr _effNode;

};

typedef boost::intrusive_ptr<Tile> TilePtr;