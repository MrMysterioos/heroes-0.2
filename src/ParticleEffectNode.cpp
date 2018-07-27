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

void ParticleEffectNode::Draw() {
	Render::device.PushMatrix();

	Render::device.MatrixTranslate(_position);
	Render::device.MatrixRotate(math::Vector3(1, 0, 0), _rotation.x);
	Render::device.MatrixRotate(math::Vector3(0, 1, 0), _rotation.y);
	Render::device.MatrixRotate(math::Vector3(0, 0, 1), _rotation.z);

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

