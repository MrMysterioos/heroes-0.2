#include "stdafx.h"
#include "ParticleEffectNode.h"

boost::intrusive_ptr<ParticleEffectNode> ParticleEffectNode::Create(Scene* scene, EffectsContainer effCont)
{
	ParticleEffectNodePtr ret(new ParticleEffectNode);
	ret->Init(scene, effCont);
	return ret;
}

void ParticleEffectNode::Init(Scene* scene, EffectsContainer effCont)
{
	Node::Init(scene);
	_effCont = effCont;
}

void ParticleEffectNode::DrawNode() {
	Render::device.PushMatrix();

	_effCont.Draw();

	Render::device.PopMatrix();
}

void ParticleEffectNode::Update(float dt)
{
	_effCont.Update(dt);
}

ParticleEffectNode::~ParticleEffectNode()
{
	if (!_effCont.IsFinished())
		_effCont.Finish();
}

