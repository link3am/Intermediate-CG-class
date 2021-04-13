#pragma once
#include "Graphics/Post/PostEffect.h"

class BloomEffect : public PostEffect
{
public:
	
	void Init(unsigned width, unsigned height) override;

	
	void ApplyEffect(PostEffect* buffer) override;
	
	
	float GetThreshold() const;
	float GetBlurmod() const;

	void SetThreshold(float intensity);
	void SetBlurmod(float mod);

private:

	float _threshold = 0.5f;
	int _blurMod = 0;

};