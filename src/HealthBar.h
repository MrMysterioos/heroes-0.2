#pragma once

#include "Node.h"

class HealthBar : public Node {
public:
	static boost::intrusive_ptr<HealthBar> Create();

	void Init();

	void DrawNode() override;
	void Update(float dt) override;

	void Damage(float dh);

	inline void SetValue(float newValue) { _value = newValue; };
	inline float GetValue() { return _value; };

private:
	float _track = 0.0f;
	float _value = 0.0f;
	float _timer = 0.0f;

};

typedef boost::intrusive_ptr<HealthBar> HealthBarPtr;