#pragma once
#include "VFrame/VPostEffect.h"
#include <array>

class WavePostEffect : public VPostEffectBase
{
public:
	sf::Vector2f waveSpeed;
	sf::Vector2f waveMultiply;
	sf::Vector2f waveAmplitude;

	WavePostEffect();
	~WavePostEffect() = default;

	virtual void Apply(const sf::Texture& input, sf::RenderTarget& output);
	virtual void Update(float dt);

private:
	void	prepareTextures(sf::Vector2u size);

	void	horizontalPass(const sf::Texture& input, sf::RenderTarget& output);
	void	verticalPass(const sf::Texture& input, sf::RenderTarget& output);

	sf::Shader horizontalWave;
	sf::Shader verticalWave;
	float timerX = 0.0f;
	float timerY = 0.0f;

	sf::RenderTexture mPassTexture;
};

