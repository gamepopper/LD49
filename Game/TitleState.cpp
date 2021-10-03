#include "TitleState.h"
#include "PlayState.h"
#include "PauseScreen.h"

#include <VFrame/VSprite.h>
#include <VFrame/VShape.h>
#include <VFrame/VGlobal.h>

#include <VFrame/VPhysicsGroup.h>

#include "WavePostEffect.h"

void TitleState::Initialise()
{
	VSUPERCLASS::Initialise();

	VGlobal::p()->BackgroundColor = sf::Color::Black;
	
	VGlobal::p()->Input->SetButtonInput("Click", sf::Keyboard::Enter, -1, sf::Mouse::Left);
	VGlobal::p()->Input->SetButtonInput("UnClick", sf::Keyboard::Delete, -1, sf::Mouse::Right);
	VGlobal::p()->Input->SetButtonInput("Pause", sf::Keyboard::Escape, GAMEPAD_BUTTON::BUTTON_START);

	VGlobal::p()->Input->SetAxisInput("Hori", sf::Keyboard::Right, sf::Keyboard::Left, VInputHandler::PovX);
	VGlobal::p()->Input->SetAxisInput("Vert", sf::Keyboard::Down, sf::Keyboard::Up, VInputHandler::PovY);

	Options = new VGroup(NUM_TITLE_OPTIONS);
	for (int i = 0; i < NUM_TITLE_OPTIONS; i++)
	{
		VText* o = new VText(0.0f, 0.0f, (float)VGlobal::p()->Width * 0.5f, ItemOptions[i].name, 16);
		o->SetFormat("Assets/ROCKB.ttf", 16);
		o->SetFillTint(sf::Color::White);
		o->SetOutlineTint(sf::Color::Black);
		o->SetOutlineThickness(2.0f);
		o->SetAlignment(VText::ALIGNCENTER);
		o->Origin = sf::Vector2f(0.5f, 0.5f);
		o->SetPositionAtCentre(VGlobal::p()->Width * 0.5f, (VGlobal::p()->Height * .65f) + (20.0f * i));

		Options->Add(o);
	}

	UpdateEntries();

	VGlobal::p()->SetMouseCursorVisible(false);

	VShape* g = new VShape((VGlobal::p()->Width * 0.5f) - (VGlobal::p()->Width * 0.05f), VGlobal::p()->Height - 30.0f);
	g->SetRectangle((float)VGlobal::p()->Width * 0.1f, 30.0f);
	g->SetFillTint(sf::Color::Transparent);
	g->SetOutlineThickness(2.f);
	g->SetOutlineTint(sf::Color(255, 255, 255, 128));
	g->Elasticity = 0.0f;

	unsigned int pointCount = 64;
	std::vector<sf::Vector2f> points;
	points.reserve(pointCount);

	//VGlobal::p()->DrawDebug = true;

	points.emplace_back();
	points.emplace_back(480.0f, 0.f);

	for (unsigned int i = 1; i < pointCount - 2; i++)
	{
		float angle = (i / (pointCount - 2.f)) * VFRAME_PI;
		float x = (cosf(angle) * 240.f) + 240.f;
		float y = sinf(angle) * 120.f;
		points.emplace_back(x, y);
	}

	sf::Texture* texture = &VGlobal::p()->Content->LoadTexture("Assets/ground.png");
	texture->setRepeated(true);

	VShape* bowl = new VShape();
	bowl->SetConvex(points);
	bowl->SetPositionAtCentre(VGlobal::p()->Width * 0.5f, VGlobal::p()->Height * 0.5f);
	bowl->Position.y = VGlobal::p()->Height - 30.f - 120.f;
	bowl->SetFillTint(sf::Color(255, 255, 255, 128));
	bowl->SetOutlineThickness(2.f);
	bowl->SetOutlineTint(sf::Color(255, 255, 255, 128));
	bowl->SetTextureFromFile("Assets/ground.png");
	bowl->SetTextureRect(sf::IntRect(0, 0, 480, 120));

	Add(g);
	Add(bowl);
	Add(blocks = new VGroup());

	title = new VRenderLayer(1);
	title->PostEffect = std::make_unique<WavePostEffect>();
	dynamic_cast<WavePostEffect*>(title->PostEffect.get())->waveAmplitude.x = VGlobal::p()->Width / 32.f;
	dynamic_cast<WavePostEffect*>(title->PostEffect.get())->waveAmplitude.y = VGlobal::p()->Height / 32.f;

	group = new VPhysicsGroup();
	group->SetGravity(sf::Vector2f(0.f, 200.f));
	group->AddObject(g, VPhysicsObject::STATIC)->SetFriction(1.f);
	VPhysicsObject* obj = group->AddObject(bowl, VPhysicsObject::DYNAMIC, VPhysicsObject::CUSTOM, points);
	obj->SetFriction(1.f);
	Add(group);

	VSprite* t = new VSprite();
	t->LoadGraphic("Assets/title.png");
	t->SetPositionAtCentre(VGlobal::p()->Width * 0.5f, VGlobal::p()->Height * 0.3f);
	title->Add(t);

	credits = new VText(0.0f, VGlobal::p()->Height - 20.0f, (float)VGlobal::p()->Width, "By Gamepopper for Ludum Dare 49", 12);
	credits->SetAlignment(VText::ALIGNCENTER);
	credits->SetOutlineThickness(2.0f);
	credits->SetOutlineTint(sf::Color::Black);

	Add(title);
	Add(credits);
	Add(Options);

	VShape* circle = new VShape();
	circle->SetCircle(5.f);
	cursor = circle;
	Add(circle);

	auto spawnBox = [this]()
	{
		VSprite* box = new VSprite();
		PlayState::BuildingType type = (PlayState::BuildingType)VGlobal::p()->Random->GetInt(PlayState::NUMBUILDINGS - 1);

		switch (type)
		{
		case PlayState::HOUSE:
			box->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(0, 0, 32, 32));
			break;
		case PlayState::POWER:
			box->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(32, 64, 64, 48));
			break;
		case PlayState::SHOP:
			box->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(64, 0, 48, 32));
			break;
		case PlayState::APARTMENT:
			box->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(32, 0, 32, 64));
			break;
		case PlayState::POLICE:
			box->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(112, 0, 48, 48));
			break;
		case PlayState::HOSPITAL:
			box->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(112, 48, 48, 48));
			break;
		};

		box->SetPositionAtCentre(VGlobal::p()->Random->GetFloat(540.f, 80.f), -64.f);
		box->Elasticity = 0.0f;
		box->Tint.a = 128;
		box->Mass = 0.01f;

		VPhysicsObject* obj = group->AddObject(box, VPhysicsObject::DYNAMIC);
		obj->SetFriction(1.f);
		blocks->Add(box);
	};

	spawnBox();

	TimeManager->AddTimerEvent(10000, spawnBox, true);

	if (VGlobal::p()->Music->Status() != sf::Music::Playing)
	{
		VGlobal::p()->Music->OpenMusicFile("Assets/music.ogg");
		VGlobal::p()->Music->SetLoop(true);
		VGlobal::p()->Music->Play();
	}

	VGlobal::p()->Sound->Load("Assets/ui.wav", "ui");
	VGlobal::p()->Sound->Load("Assets/confirm.wav", "confirm");

	Config* config = new Config();
	VGlobal::p()->Music->SetVolume((float)config->GetPropertyAsInt("Audio_MusicVolume"));
	VGlobal::p()->Sound->SetMasterVolume((float)config->GetPropertyAsInt("Audio_SoundVolume"));
	VGlobal::p()->SetFullscreen(config->GetPropertyAsBool("Visual_Fullscreen"));
}

void TitleState::Update(float dt)
{
	VSUPERCLASS::Update(dt);

	cursor->SetPositionAtCentre(VGlobal::p()->GetMousePosition());

	if ((VGlobal::p()->Input->CurrentAxisValue("Vert") < 0.0f && VGlobal::p()->Input->LastAxisValue("Vert") >= 0.0f) ||
		VGlobal::p()->Input->IsButtonPressed("U"))
	{
		CurrentOption = (CurrentOption - 1 + NUM_TITLE_OPTIONS) % NUM_TITLE_OPTIONS;
		UpdateEntries();
	}

	if ((VGlobal::p()->Input->CurrentAxisValue("Vert") > 0.0f && VGlobal::p()->Input->LastAxisValue("Vert") <= 0.0f) ||
		VGlobal::p()->Input->IsButtonPressed("D"))
	{
		CurrentOption = (CurrentOption + 1 + NUM_TITLE_OPTIONS) % NUM_TITLE_OPTIONS;
		UpdateEntries();
	}

	for (int i = 0; i < NUM_TITLE_OPTIONS; i++)
	{
		VText* o = Options->GetGroupItemAsType<VText>(i);
		if (VGlobal::p()->OverlapAtPoint(cursor->Position + (cursor->Size * 0.5f), o))
		{
			if (i != CurrentOption)
			{
				CurrentOption = i;
				UpdateEntries();
			}

			if (VGlobal::p()->Input->IsButtonPressed("Click"))
				SelectEntries();
		}
	}

	if (VGlobal::p()->Input->IsButtonPressed("Select"))
	{
		SelectEntries();
	}
}

void TitleState::UpdateEntries()
{
	for (unsigned int i = 0; i < NUM_TITLE_OPTIONS; i++)
	{
		VText* o = dynamic_cast<VText*>(Options->GetGroupItem(i));
		o->SetText(VBase::VString(ItemOptions[i].name, ItemOptions[i].number));

		if (CurrentOption == i)
			o->Scale = sf::Vector2f(1.2f, 1.2f);
		else
			o->Scale = sf::Vector2f(1.0f, 1.0f);
	}

	VGlobal::p()->Sound->Play("ui", 100.0f, 1.0f);
}

void TitleState::SelectEntries()
{
	switch (CurrentOption)
	{
	case TITLE_START:
		VGlobal::p()->ChangeState(new PlayState());
		break;
	case TITLE_OPTIONS:
		OpenSubState(new PauseScreen(true));
		break;
	case TITLE_EXIT:
		VGlobal::p()->Exit();
		break;
	}

	VGlobal::p()->Sound->Play("confirm", 100.0f, 1.0f);
}