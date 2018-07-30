#include "stdafx.h"
#include "Node.h"
#include "Scene.h"

void Node::Init(Scene* scene)
{
	NodePtr node(this);
	scene->_nodes.push_back(node);
	_scene = scene;
}

void Node::Draw() {
	Render::device.PushMatrix();

	Render::device.MatrixTranslate(_position);
	Render::device.MatrixRotate(math::Vector3(1, 0, 0), _rotation.x);
	Render::device.MatrixRotate(math::Vector3(0, 1, 0), _rotation.y);
	Render::device.MatrixRotate(math::Vector3(0, 0, 1), _rotation.z);

	DrawNode();

	Render::device.PopMatrix();
}