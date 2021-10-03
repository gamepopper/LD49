#include "PauseScreen.h"
#include <VFrame/VText.h>
#include <VFrame/VShape.h>
#include <VFrame/VTimer.h>
#include <VFrame/VGlobal.h>
#include <VFrame/VColour.h>
#include <VFrame/VInterpolate.h>

#include "TitleState.h"
#include "Config.h"

void PauseScreen::Initialise()
{
	VSUPERCLASS::Initialise();

	config = std::make_unique<Config>();

	ParentState->active = false;
	overText = new VText(0.0f, 0.0f, (float)VGlobal::p()->Width, titleScreen ? "OPTIONS" : "PAUSED", 32);
	overText->SetFormat("Assets/ROCKB.ttf", 32);
	overText->SetPositionAtCentre(VGlobal::p()->Width * .5f, VGlobal::p()->Height * .25f);
	overText->SetAlignment(VText::ALIGNCENTER);
	overText->SetOutlineTint(sf::Color::Black);
	overText->SetOutlineThickness(2.0f);

	options = new VGroup(NUM_TITLE_OPTIONS);
	for (int i = 0; i < NUM_TITLE_OPTIONS; i++)
	{
		VText* o = new VText(0.0f, (VGlobal::p()->Height * .5f) + (20.0f * i), (float)VGlobal::p()->Width, ItemOptions[i].name, 16);
		o->SetFormat("Assets/ROCKB.ttf", 16);
		o->SetOutlineTint(sf::Color::Black);
		o->SetAlignment(VText::ALIGNCENTER);
		o->Origin = sf::Vector2f(0.5f, 0.5f);
		o->SetOutlineThickness(2.0f);
		options->Add(o);
	}

	ItemOptions[TITLE_MUSICVOLUME].number = config->GetPropertyAsInt("Audio_MusicVolume");
	ItemOptions[TITLE_SFXVOLUME].number = config->GetPropertyAsInt("Audio_SoundVolume");
	ItemOptions[TITLE_FULLSCREEN].number = config->GetPropertyAsBool("Visual_Fullscreen");

	if (!titleScreen)
		strncpy(ItemOptions[TITLE_EXIT].name, "RETURN TO GAME", 255);

	UpdateEntries();

	Add(options);
	Add(overText);

	VShape* circle = new VShape();
	circle->SetCircle(5.f);
	cursor = circle;
	Add(circle);

	timer = TimeManager->AddTimer();
}

void PauseScreen::HandleEvents(const sf::Event& e)
{
	
}

void PauseScreen::Update(float dt)
{
	VSUPERCLASS::Update(dt);

	cursor->SetPositionAtCentre(VGlobal::p()->GetMousePosition());

	if (fade == 0)
	{
		if ((VGlobal::p()->Input->CurrentAxisValue("Vert") < -20.0f && VGlobal::p()->Input->LastAxisValue("Vert") >= 0.0f) ||
			VGlobal::p()->Input->IsButtonPressed("U"))
		{
			currentOption = (currentOption - 1 + NUM_TITLE_OPTIONS) % NUM_TITLE_OPTIONS;
			UpdateEntries();
			VGlobal::p()->Sound->Play("ui");
		}

		if ((VGlobal::p()->Input->CurrentAxisValue("Vert") > 20.0f && VGlobal::p()->Input->LastAxisValue("Vert") <= 0.0f) ||
			VGlobal::p()->Input->IsButtonPressed("D"))
		{
			currentOption = (currentOption + 1 + NUM_TITLE_OPTIONS) % NUM_TITLE_OPTIONS;
			UpdateEntries();
			VGlobal::p()->Sound->Play("ui");
		}

		bool hover = false;
		for (int i = 0; i < NUM_TITLE_OPTIONS; i++)
		{
			VText* o = options->GetGroupItemAsType<VText>(i);
			if (VGlobal::p()->OverlapAtPoint(cursor->Position + (cursor->Size * 0.5f), o))
			{
				hover = true;
				if (i != currentOption)
				{
					currentOption = i;
					UpdateEntries();
				}
			}
		}

		if ((VGlobal::p()->Input->CurrentAxisValue("Hori") < 0.0f && VGlobal::p()->Input->LastAxisValue("Hori") >= 0.0f) ||
			VGlobal::p()->Input->IsButtonPressed("L") || (hover && VGlobal::p()->Input->IsButtonPressed("Click")))
		{
			if (currentOption == TITLE_MUSICVOLUME && ItemOptions[TITLE_MUSICVOLUME].number > 0)
			{
				ItemOptions[TITLE_MUSICVOLUME].number -= 10;
				UpdateEntries();
				VGlobal::p()->Sound->Play("ui");
				VGlobal::p()->Music->SetVolume((float)ItemOptions[TITLE_MUSICVOLUME].number);
				config->SetPropertyAsInt("Audio_MusicVolume", ItemOptions[TITLE_MUSICVOLUME].number);
			}

			if (currentOption == TITLE_SFXVOLUME && ItemOptions[TITLE_SFXVOLUME].number > 0)
			{
				ItemOptions[TITLE_SFXVOLUME].number -= 10;
				UpdateEntries();
				VGlobal::p()->Sound->Play("ui");
				VGlobal::p()->Sound->SetMasterVolume((float)(ItemOptions[TITLE_SFXVOLUME].number));
				config->SetPropertyAsInt("Audio_SoundVolume", ItemOptions[TITLE_SFXVOLUME].number);
			}
		}

		if ((VGlobal::p()->Input->CurrentAxisValue("Hori") > 0.0f && VGlobal::p()->Input->LastAxisValue("Hori") <= 0.0f) ||
			VGlobal::p()->Input->IsButtonPressed("R") || (hover && VGlobal::p()->Input->IsButtonPressed("UnClick")))
		{
			if (currentOption == TITLE_MUSICVOLUME && ItemOptions[TITLE_MUSICVOLUME].number < 100)
			{
				ItemOptions[TITLE_MUSICVOLUME].number += 10;
				UpdateEntries();
				VGlobal::p()->Sound->Play("ui");
				VGlobal::p()->Music->SetVolume((float)ItemOptions[TITLE_MUSICVOLUME].number);
				config->SetPropertyAsInt("Audio_MusicVolume", ItemOptions[TITLE_MUSICVOLUME].number);
			}

			if (currentOption == TITLE_SFXVOLUME && ItemOptions[TITLE_SFXVOLUME].number < 100)
			{
				ItemOptions[TITLE_SFXVOLUME].number += 10;
				UpdateEntries();
				VGlobal::p()->Sound->Play("ui");
				VGlobal::p()->Sound->SetMasterVolume((float)ItemOptions[TITLE_SFXVOLUME].number);
				config->SetPropertyAsInt("Audio_SoundVolume", ItemOptions[TITLE_SFXVOLUME].number);
			}
		}

		if (VGlobal::p()->Input->IsButtonPressed("Select") || (hover && currentOption >= TITLE_FULLSCREEN && VGlobal::p()->Input->IsButtonPressed("Click")))
		{
			SelectEntries();
			UpdateEntries();
		}
	}
	else if (fade == -1) //Fade In
	{
		SetFillColour(VColour::HSVtoRGB(0.0f, 0.0f, 0.0f, titleScreen ? 1.0f : 0.8f * (timer->Seconds() / fadeTime)));
		if (timer->Seconds() > fadeTime)
			fade = 0;
	}
	else if (fade == 1) //Fade Out
	{
		SetFillColour(VColour::HSVtoRGB(0.0f, 0.0f, 0.0f, titleScreen ? 1.0f : 0.8f * ((fadeTime - timer->Seconds()) / fadeTime)));
		if (timer->Seconds() > fadeTime)
			Close();
	}
}

void PauseScreen::UpdateEntries()
{
	sf::String s1 = "FULLSCREEN: ";
	s1.insert(s1.getSize(), config->GetPropertyAsBool("Visual_Fullscreen") ? "TRUE" : "FALSE");
	strncpy(ItemOptions[TITLE_FULLSCREEN].name, s1.toAnsiString().c_str(), 255);

	for (unsigned int i = 0; i < NUM_TITLE_OPTIONS; i++)
	{
		VText* o = options->GetGroupItemAsType<VText>(i);
		o->SetText(VBase::VString(ItemOptions[i].name, ItemOptions[i].number));

		if (currentOption == i)
			o->Scale = sf::Vector2f(1.2f, 1.2f);
		else
			o->Scale = sf::Vector2f(1.0f, 1.0f);
	}
}

void PauseScreen::SelectEntries()
{
	switch (currentOption)
	{
	case TITLE_FULLSCREEN:
		config->SetPropertyAsBool("Visual_Fullscreen", !config->GetPropertyAsBool("Visual_Fullscreen"));
		VGlobal::p()->SetFullscreen(config->GetPropertyAsBool("Visual_Fullscreen"));
		VGlobal::p()->SetMouseCursorVisible(true);
		break;
	case TITLE_EXIT:
		if (titleScreen)
		{
			VGlobal::p()->ChangeState(new TitleState());
		}
		else
		{
			timer->Restart();
			fade = 1;
		}
		break;
	}
	
	VGlobal::p()->Sound->Play("confirm", 100.0f, 1.0f);

	UpdateEntries();
}