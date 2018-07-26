#pragma once

#include "Node.h"

class Scene {
public:

	Scene();

	void Init();

	void Draw();

	void Update(float dt);

	inline void PushNode(NodePtr node) { _nodes.push_back(node); };
	void PushVector(std::vector<NodePtr> nodes);

	inline void SetCamraPosition(FPoint newPosition) { _cameraPosition = newPosition; };
	inline FPoint GetCamraPosition() { return _cameraPosition; };

	inline void SetCameraZoom(float newValue) { _cameraZoom = newValue; };
	inline float GetCameraZoom() { return _cameraZoom; };

private:
	std::vector<NodePtr> _nodes;
	FPoint _cameraPosition = FPoint(0.0f, 0.0f);
	float _cameraZoom = 1;
};