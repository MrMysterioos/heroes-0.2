#pragma once
#include "Node.h"

class ParticleEffectNode : public Node {
public:

	static boost::intrusive_ptr<ParticleEffectNode> Create(EffectsContainer effCont);

	void Init(EffectsContainer effCont);

	void DrawNode() override;

	void Update(float dt) override;

	void Destroy();

	inline EffectsContainer& GetContainer() { return _effCont; }
	inline void SetContainer(EffectsContainer newCont) { _effCont = newCont; };
	inline void SetAutoDelete(bool val) { autoDelete = val; };

	~ParticleEffectNode();

private:
	EffectsContainer _effCont;

	bool autoDelete = false;
};

typedef boost::intrusive_ptr<ParticleEffectNode> ParticleEffectNodePtr;