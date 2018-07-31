#pragma once

#include "Node.h"

class HealthBar : public Node {
public:
	static boost::intrusive_ptr<HealthBar> Create();

	void Init();

	void DrawNode() override;
	void Update(float dt) override {};

	inline void SetValue(float newValue) { value = newValue; };
	inline float GetValue() { return value; };

private:
	float value = 0.0f;

};

typedef boost::intrusive_ptr<HealthBar> HealthBarPtr;