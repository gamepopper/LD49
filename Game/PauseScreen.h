#pragma once
#include <VFrame/VState.h>
#include "Menu.h"
#include "Config.h"

class VTimer;
class VText;
class PauseScreen : public VSubState
{
	VText* overText;
	VGroup* options;
	VObject* cursor;
	VTimer* timer;
	std::unique_ptr<Config> config;
	int fade = -1;
	int currentOption = 0;
	bool titleScreen = false;
	const float fadeTime = 0.1f;

	enum
	{
		TITLE_MUSICVOLUME,
		TITLE_SFXVOLUME,
		TITLE_FULLSCREEN,
		TITLE_EXIT,
		NUM_TITLE_OPTIONS
	};

	Item ItemOptions[NUM_TITLE_OPTIONS] =
	{
		{"MUSIC: %d", 100},
		{"SFX: %d", 100},
		{"FULLSCREEN: FALSE"},
		{"MAIN MENU"},
	};

public:
	typedef VSubState VSUPERCLASS;

	PauseScreen(bool title = false) : titleScreen(title) {}
	virtual void Initialise();
	virtual void HandleEvents(const sf::Event& e) override;
	virtual void Update(float dt);

	void UpdateEntries();
	void SelectEntries();
};