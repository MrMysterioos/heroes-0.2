#include "stdafx.h"

#include "ParticleEffectNode.h"
#include "Scene.h"

boost::intrusive_ptr<ParticleEffectNode> ParticleEffectNode::Create(EffectsContainer effCont)
{
	ParticleEffectNodePtr ret(new ParticleEffectNode);
	ret->Init(effCont);
	return ret;
}

void ParticleEffectNode::Init(EffectsContainer effCont)
{
	Node::Init();
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
	if (_effCont.IsFinished() && autoDelete) {
		Destroy();
	}
}

void ParticleEffectNode::Destroy()
{
	Scene::GetInstance().DeleteNode(this);
}

ParticleEffectNode::~ParticleEffectNode()
{
	if (!_effCont.IsFinished())
		_effCont.Finish();
}

