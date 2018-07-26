#pragma once
#include "Sprite.h"

struct Area {
	std::string name;

	Render::TexturePtr texture;
	std::string effect;

};

class Tile : public Sprite {
public:
	static boost::intrusive_ptr<Tile> Create(Area area);
	void Init(Area area);

	void Draw() override;
	void Update(float dt) override;

private:
	EffectsContainer _effCont;
	ParticleEffectPtr _eff;

};

typedef boost::intrusive_ptr<Tile> TilePtr;