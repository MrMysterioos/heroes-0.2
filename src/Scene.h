#pragma once

#include "Node.h"

class Scene {
public:

	friend class Node;

	Scene();

	void Init();

	void Draw();

	void Update(float dt);

	void DeleteNode(NodePtr node);
	void PushVector(std::vector<NodePtr> nodes);

	inline void SetCamraPosition(FPoint newPosition) { _cameraPosition = newPosition; };
	inline FPoint GetCamraPosition() { return _cameraPosition; };

	inline void SetCameraZoom(float newValue) { _cameraZoom = newValue; };
	inline float GetCameraZoom() { return _cameraZoom; };

private:
	std::vector<NodePtr> _nodes;
	std::vector<EffectsContainer> _effects;
	FPoint _cameraPosition = FPoint(0.0f, 0.0f);
	float _cameraZoom = 1;
};