#include "stdafx.h"
#include "Node.h"
#include "Scene.h"

void Node::Init(Scene* scene)
{
	NodePtr node(this);
	scene->_nodes.push_back(node);
	_scene = scene;
}