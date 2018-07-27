#pragma once
#include "Node.h"

class ParticleEffectNode : public Node {
public:

	static boost::intrusive_ptr<ParticleEffectNode> Create(Scene* scene, EffectsContainer effCont);

	void Init(Scene* scene, EffectsContainer effCont);

	void Draw() override;

	void Update(float dt) override;

	inline EffectsContainer& GetContainer() { return _effCont; }
	inline void SetContainer(EffectsContainer newCont) { _effCont = newCont; };

	~ParticleEffectNode();

private:
	EffectsContainer _effCont;
};

typedef boost::intrusive_ptr<ParticleEffectNode> ParticleEffectNodePtr;