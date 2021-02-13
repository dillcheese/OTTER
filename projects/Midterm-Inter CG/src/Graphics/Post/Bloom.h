#pragma once

#include "Graphics/Post/PostEffect.h"

class Bloom : public PostEffect
{
public:
	//Initializes framebuffer
	//Overrides post effect Init
	void Init(unsigned width, unsigned height) override;

	//Applies the effect to this buffer
	//passes the previous framebuffer with the texture to apply as parameter
	void ApplyEffect(PostEffect* buffer) override;

	//Getters
	float GetIntensity() const;
	float GetThreshold() const;
	int GetAmount() const;

	//Setters
	void SetIntensity(float intensity);
	void SetThreshold(float threshold);
	void SetAmount(unsigned amount);

private:
	float m_intensity = 1.0f;//exposure
	float m_threshold = 0.01f;
	int m_amount = 5;
	

};