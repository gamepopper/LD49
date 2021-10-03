#pragma once
#include <VFrame/VState.h>
#include <VFrame/VText.h>
#include <VFrame/VSprite.h>

#include <VFrame/VRenderLayer.h>

#include "Menu.h"

class VPhysicsGroup;
class TitleState : public VState
{
public:
	typedef VState VSUPERCLASS;
	TitleState() = default;

	VRenderLayer* title;
	VText* credits;
	VObject* cursor;

	VPhysicsGroup* group;
	VGroup* blocks;

	virtual void Initialise() override;
	virtual void Update(float dt) override;

	void UpdateEntries();
	void SelectEntries();

	VGroup* Options;
	int CurrentOption = 0;
	
	enum
	{
		TITLE_START,
		TITLE_OPTIONS,
		TITLE_EXIT,
		NUM_TITLE_OPTIONS
	};

	Item ItemOptions[NUM_TITLE_OPTIONS] = 
	{
		{"START"},
		{"OPTION"},
		{"EXIT"},
	};
};

