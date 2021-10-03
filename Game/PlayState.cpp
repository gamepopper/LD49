#include "PlayState.h"
#include <VFrame/VText.h>
#include <VFrame/VSprite.h>
#include <VFrame/VShape.h>
#include <VFrame/VGlobal.h>
#include <VFrame/VEmitter.h>
#include <VFrame/VPhysicsGroup.h>

#include "PauseScreen.h"
#include "TitleState.h"

class BuildingSprite : public VSprite
{
public:
	BuildingSprite() = default;

	PlayState::BuildingType Type;
	int Occupied = 0;
};

class PersonSprite : public VSprite
{
public:
	PersonSprite() = default;

	BuildingSprite* Destination;
};

void PlayState::CreateBuilding(BuildingType type, float x)
{
	BuildingSprite* box = new BuildingSprite();
	box->Type = type;
	switch (type)
	{
	case HOUSE:
		box->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(0, 0, 32, 32));
		box->Occupied = 4;
		break;
	case POWER:
		box->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(32, 64, 64, 48));
		break;
	case SHOP:
		box->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(64, 0, 48, 32));
		break;
	case APARTMENT:
		box->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(32, 0, 32, 64));
		box->Occupied = 12;
		break;
	case POLICE:
		box->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(112, 0, 48, 48));
		break;
	case HOSPITAL:
		box->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(112, 48, 48, 48));
		break;
	};

	personCount += box->Occupied;

	box->SetPositionAtCentre(ground->Position.x + x, preview->Position.y + (preview->Size.y * 0.5f));
	box->Mass = 0.1f;

	VPhysicsObject* obj = group->AddObject(box, VPhysicsObject::DYNAMIC);
	obj->SetFriction(1.f);
	obj->UpdateCollisionFilter(0, ~(1 << 0), ~(1 << 0));
	buildings->Add(box);

	VGlobal::p()->Sound->Play("land");

	availableWeights[type]--;
	availableWeightsTotal--;
}

void PlayState::Initialise()
{
	VSUPERCLASS::Initialise();

	VShape* g = new VShape((VGlobal::p()->Width * 0.5f) - (VGlobal::p()->Width * 0.05f), VGlobal::p()->Height - 30.0f);
	g->SetRectangle((float)VGlobal::p()->Width * 0.1f, 30.0f);
	g->SetFillTint(sf::Color::Transparent);
	g->SetOutlineThickness(2.f);
	g->SetOutlineTint(sf::Color::White);
	g->Elasticity = 0.0f;

	weatherEmitter = new VEmitter(VGlobal::p()->Width * 0.5f, -10.0f, 200);
	weatherEmitter->LoadParticlesFromFile(200, "Assets/Texturess.png", false, 2, 2, sf::IntRect(12, 3, 2, 2));
	weatherEmitter->Size.x = VGlobal::p()->Width * 1.2f;
	weatherEmitter->AccelerationRange = sf::Vector2f(0.f, 200.f);
	weatherEmitter->VelocityRange = VRange<sf::Vector2f>(sf::Vector2f(-50, 50.f), sf::Vector2f(-10.f, 50.f));
	weatherEmitter->Lifespan = 2.f;
	weatherEmitter->Constant = true;
	weatherEmitter->Frequency = 0.05f;
	weatherEmitter->AmountPerEmit = 5;

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

	VShape* bowl = new VShape();
	bowl->SetConvex(points);
	bowl->SetPositionAtCentre(VGlobal::p()->Width * 0.5f, VGlobal::p()->Height * 0.5f);
	bowl->Position.y = VGlobal::p()->Height - 30.f - 120.f;
	bowl->SetFillTint(sf::Color::White);
	bowl->SetOutlineThickness(2.f);
	bowl->SetOutlineTint(sf::Color::White);
	bowl->SetTextureFromFile("Assets/ground.png");
	bowl->SetTextureRect(sf::IntRect(0, 0, 480, 120));

	preview = new BuildingSprite();

	ground = bowl;

	buildings = new VGroup();
	people = new VGroup();

	VShape* circle = new VShape();
	circle->SetCircle(5.f);
	cursor = circle;

	Add(g);
	Add(bowl);
	Add(preview);
	Add(buildings);
	Add(people);
	Add(circle);
	Add(weatherEmitter);

	for (int i = 0; i < 5; i++)
	{
		availableOptions[i] = new VSprite();
		availableOptions[i]->Position = sf::Vector2f(VGlobal::p()->Width - 44.f - (i * 34.f), 15.f);
		Add(availableOptions[i]);
		
		availableBuildings[i] = NUMBUILDINGS;
	}

	availableText = new VText(0.0f, 15.f, VGlobal::p()->Width * 0.5f, "Available: ", 28);
	availableText->SetFormat("Assets/ROCKB.ttf", 28);
	availableText->Position.x = VGlobal::p()->Width - 60.f - (4 * 34.f) - (VGlobal::p()->Width * 0.5f);
	availableText->SetAlignment(VText::ALIGNRIGHT);
	availableText->visible = false;
	Add(availableText);

	requiredText = new VText(10.f, 15.f, VGlobal::p()->Width * 1.f, "", 28);
	requiredText->SetFormat("Assets/ROCKB.ttf", 28, sf::Color::White, VText::ALIGNCENTER);
	Add(requiredText);

	dayText = new VText(10, VGlobal::p()->Height - 32.f, VGlobal::p()->Width * 1.f, "Day: 1", 28);
	dayText->SetFormat("Assets/ROCKB.ttf", 28);
	Add(dayText);

	mouse = new VSprite(10.f, 160.f, "Assets/mouse.png");
	mouse->visible = false;
	Add(mouse);

	group = new VPhysicsGroup();
	group->SetGravity(sf::Vector2f(0.f, 200.f));
	group->AddObject(g, VPhysicsObject::STATIC)->SetFriction(1.f);
	VPhysicsObject* obj = group->AddObject(bowl, VPhysicsObject::DYNAMIC, VPhysicsObject::CUSTOM, points);
	obj->SetFriction(1.f);
	obj->UpdateCollisionFilter(0, ~0, ~0);
	Add(group);

	requiredBuildings[0] = HOUSE;
	requiredBuildings[1] = POWER;
	requiredBuildings[2] = SHOP;

	std::shuffle(requiredBuildings, requiredBuildings + (sizeof(requiredBuildings)/sizeof(requiredBuildings[0])), std::default_random_engine(VGlobal::p()->Random->GetInt(255)));
	requiredTimer = TimeManager->AddTimer();

	availableWeights[HOUSE] = 3;
	availableWeights[APARTMENT] = 3;
	availableWeights[POWER] = 1;
	availableWeights[SHOP] = 2;
	availableWeights[POLICE] = 2;
	availableWeights[HOSPITAL] = 2;
	availableWeightsTotal = 14;

	VGlobal::p()->Sound->Load("Assets/birth.wav", "birth");
	VGlobal::p()->Sound->Load("Assets/crash.wav", "crash");
	VGlobal::p()->Sound->Load("Assets/dead.wav", "dead");
	VGlobal::p()->Sound->Load("Assets/gameover.wav", "gameover");
	VGlobal::p()->Sound->Load("Assets/land.wav", "land");

	TimeManager->AddTimerEvent(5000, [this]() 
	{
		if (requiredBuildings[0] != NUMBUILDINGS)
			return;

		mouse->visible = days < 3;

		if (days % 5 == 0)
		{
			for (int i = 0; i < 5; i++)
			{
				if (availableBuildings[i] == NUMBUILDINGS)
				{
					int pick = VGlobal::p()->Random->GetInt(availableWeightsTotal);

					for (unsigned int j = 0; j < NUMBUILDINGS; j++)
					{
						if (availableWeights[j] > 0 && pick <= availableWeights[j])
						{
							availableBuildings[i] = (BuildingType)j;
							break;
						}

						pick -= availableWeights[j];
					}

					break;
				}
			}
		}

		if (weather = (VGlobal::p()->Random->GetInt(10) == 0))
		{
			VPhysicsObject* obj = group->FindPhysicsObject(ground);
			obj->ApplyImpulseAtLocalPoint(sf::Vector2f(0.f, 10.f + days), sf::Vector2f(VGlobal::p()->Random->GetFloat(ground->Size.x), 0.f));

			weatherEmitter->Start();
		}
		else
		{
			weatherEmitter->Stop();
		}

		int birth = (buildings->Length() > 0) * VGlobal::p()->Random->GetInt(8 * (2 - availableWeights[HOSPITAL])) / 8;

		if (birth)
		{
			VGlobal::p()->Sound->Play("birth");

			for (int i = 0; i < birth; i++)
			{
				BuildingSprite* b;
				do
				{
					b = dynamic_cast<BuildingSprite*>(buildings->GetRandom());
				} while (b != NULL && b->Type != HOSPITAL);

				b->Occupied++;
			}
		}

		personCount += birth;

		int dead = (buildings->Length() > 0) * VGlobal::p()->Random->GetInt(8 * availableWeights[POLICE]) / 8;

		if (dead)
		{
			VGlobal::p()->Sound->Play("dead");

			for (int i = 0; i < dead; i++)
			{
				if (people->CountAlive() > 0)
				{
					VObject* p = dynamic_cast<VObject*>(people->FirstAlive());
					p->Kill();
					group->RemoveObject(p);
				}
				else
				{
					BuildingSprite* b;
					do
					{
						b = dynamic_cast<BuildingSprite*>(buildings->GetRandom());
					} while (b != NULL && b->Occupied > 0);

					b->Occupied--;
				}
			}
		}

		personCount -= dead;

		if (buildings->Length())
		{
			int numPeople = 12 - availableWeightsTotal;
			for (int i = 0; i < numPeople; i++)
			{
				//Spawn person
				PersonSprite* p = NULL;

				if (people->Length() >= personCount)
					p = dynamic_cast<PersonSprite*>(people->FirstAvailable());

				SpawnPerson(p);
			}
		}

		days++;
	}, true);
}

void PlayState::SpawnPerson(PersonSprite* p)
{
	BuildingSprite* b = NULL;
	if (p == NULL)
	{
		do
		{
			b = dynamic_cast<BuildingSprite*>(buildings->GetRandom());
		} while (b != NULL && !(b->Type == HOUSE || b->Type == APARTMENT) && b->Occupied == 0);

		if (b != NULL)
		{
			p = new PersonSprite();
			p->LoadGraphic("Assets/Texturess.png", true, 6, 9, sf::IntRect(0, 32, 18, 9));
			p->Animation.AddAnimation("default", { 0, 1, 0, 2 }, 6.0f, true);
			p->Animation.Play("default");
			p->Mass = 0.1f;
			people->Add(p);
		}
		else
		{
			return;
		}
	}
	else
	{
		b = p->Destination;
	}

	b->Occupied--;
	p->Revive();
	p->SetPositionAtCentre(b->Position + (b->Size * 0.5f));

	VPhysicsObject* obj = group->AddObject(p, VPhysicsObject::DYNAMIC);
	//obj->SetFriction(1.f);
	obj->UpdateCollisionFilter(1, 1 << 0, ~(1 << 0));
	obj->Lock |= VPhysicsObject::XVEL;
	obj->Lock |= VPhysicsObject::ANGLE;

	if (b->Type == HOUSE || b->Type == APARTMENT)
	{
		do
		{
			p->Destination = dynamic_cast<BuildingSprite*>(buildings->GetRandom());
		} while (p->Destination != NULL && (p->Destination->Type == HOUSE || p->Destination->Type == APARTMENT));
	}
	else
	{
		do
		{
			p->Destination = dynamic_cast<BuildingSprite*>(buildings->GetRandom());
		} while (p->Destination != NULL && p->Destination->Type != HOUSE && p->Destination->Type != APARTMENT);
	}
}

void PlayState::Update(float dt)
{
	VSUPERCLASS::Update(dt);

	preview->visible = false;
	cursor->visible = true;

	cursor->SetPositionAtCentre(VGlobal::p()->GetMousePosition());

	if (ground->alive)
	{
		dayText->SetText("Day: " + std::to_string(days + 1) + " - People: " + std::to_string(personCount));

		if (weather)
			dayText->SetText(dayText->GetText() + " - WEATHER WARNING!");

		requiredText->visible = false;
		if (requiredBuildings[0] == NUMBUILDINGS)
		{
			ProcessAvailable();
			ProcessGrabbed();

			if (days < 4)
			{
				if (requiredText->GetAlignment() != VText::ALIGNLEFT)
					requiredText->SetAlignment(VText::ALIGNLEFT);

				mouse->visible = true;
				requiredText->visible = true;
				requiredText->SetText("Left click:\nPlace/Land\nRight Click:\nDelete");
			}

			if (personCount == 0)
			{
				requiredText->SetAlignment(VText::ALIGNCENTER);
				VGlobal::p()->Sound->Play("gameover");
				ground->alive = false;

				requiredText->SetText("Everyone is Dead! Game Over!");
				requiredText->visible = true;
				availableText->visible = false;

				TimeManager->AddTimerEvent(4000, [this]()
				{
					VGlobal::p()->ChangeState(new TitleState());
				});

				return;
			}
		}
		else
		{
			requiredText->visible = true;
			ProcessRequired();
		}

		people->ForEachAlive(
			[this](VBase* b)
		{
			PersonSprite* p = dynamic_cast<PersonSprite*>(b);

			if (p->Destination != NULL)
			{
				if (p->Destination->destroyed)
				{
					p->Destination = NULL;
					return;
				}

				float vel = p->Destination->Position.x - p->Position.x;
				vel = ((vel >= 0) * 50.f) + ((vel < 0) * -50.f);
				p->Velocity.x = vel;
			}

			if (VGlobal::p()->Overlaps(p->Destination, p))
			{
				p->Kill();
				group->RemoveObject(p);
			}
		}
		);

		if (grabbedBuilding && grabbedBuilding->destroyed)
		{
			delete grabbedBuilding;
			grabbedBuilding = NULL;
		}

		if (ground->Position.y >= VGlobal::p()->Height)
		{
			VGlobal::p()->Sound->Play("gameover");
			ground->Kill();

			requiredText->SetAlignment(VText::ALIGNCENTER);
			requiredText->SetText("The City is Gone! Game Over!");
			requiredText->visible = true;
			availableText->visible = false;

			TimeManager->Clear();
			TimeManager->AddTimerEvent(4000, [this]()
			{
				VGlobal::p()->Sound->Play("crash");

				TimeManager->AddTimerEvent(4000, [this]()
				{
					VGlobal::p()->ChangeState(new TitleState());
				});
			});
		}

		if (VGlobal::p()->Input->IsButtonPressed("Pause"))
		{
			OpenSubState(new PauseScreen());
		}
	}
}

void PlayState::ProcessAvailable()
{
	availableText->visible = false;
	for (int i = 0; i < 5; i++)
	{
		availableOptions[i]->visible = availableBuildings[i] != NUMBUILDINGS;
		availableText->visible |= availableOptions[i]->visible;
		switch (availableBuildings[i])
		{
		case HOUSE:
			availableOptions[i]->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(0, 0, 32, 32));
			break;
		case POWER:
			availableOptions[i]->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(32, 64, 64, 48));
			break;
		case SHOP:
			availableOptions[i]->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(64, 0, 48, 32));
			break;
		case APARTMENT:
			availableOptions[i]->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(32, 0, 32, 64));
			break;
		case POLICE:
			availableOptions[i]->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(112, 0, 48, 48));
			break;
		case HOSPITAL:
			availableOptions[i]->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(112, 48, 48, 48));
			break;
		};

		availableOptions[i]->Scale = sf::Vector2f(32.f / availableOptions[i]->Size.x, 32.f / availableOptions[i]->Size.y);

		if (VGlobal::p()->OverlapAtPoint(VGlobal::p()->GetMousePosition(), availableOptions[i]))
			availableOptions[i]->Tint = (sf::Color(255, 255, 255, 255));
		else
			availableOptions[i]->Tint = (sf::Color(255, 255, 255, 128));

		if (grabbedBuilding == NULL && availableOptions[i]->visible && availableOptions[i]->Tint.a == 255 && VGlobal::p()->Input->IsButtonPressed("Click"))
		{
			CreateBuilding(availableBuildings[i], 0);
			grabbedBuilding = buildings->GetGroupItemAsType<BuildingSprite>(buildings->Length() - 1);
			grabbedBuilding->Kill();

			group->RemoveObject(grabbedBuilding);
			availableBuildings[i] = NUMBUILDINGS;
		}
	}
}

void PlayState::ProcessGrabbed()
{
	if (grabbedBuilding == NULL)
	{
		if (VGlobal::p()->Input->IsButtonPressed("Click"))
		{
			for (int i = 0; i < buildings->Length(); i++)
			{
				BuildingSprite* obj = buildings->GetGroupItemAsType<BuildingSprite>(i);
				if (VGlobal::p()->OverlapAtPoint(VGlobal::p()->GetMousePosition(), obj))
				{
					grabbedBuilding = obj;
					grabbedBuilding->Kill();

					group->RemoveObject(obj);
					break;
				}
			}
		}
		else if (VGlobal::p()->Input->IsButtonPressed("UnClick"))
		{
			for (int i = 0; i < buildings->Length(); i++)
			{
				BuildingSprite* obj = buildings->GetGroupItemAsType<BuildingSprite>(i);
				if (VGlobal::p()->OverlapAtPoint(VGlobal::p()->GetMousePosition(), obj))
				{
					group->RemoveObject(obj);

					grabbedBuilding = obj;
					grabbedBuilding->Kill();
					grabbedBuilding->Destroy();
					buildings->Remove(grabbedBuilding, true);
					availableWeights[grabbedBuilding->Type]++;

					availableWeightsTotal++;

					break;
				}
			}
		}
	}
	else if (cursor->Position.x + cursor->Size.x * 0.5f >= ground->Position.x && cursor->Position.x + cursor->Size.x * 0.5f < ground->Position.x + ground->Size.x)
	{
		ShowPreview(grabbedBuilding->Type, cursor->Position.x + cursor->Size.x * 0.5f);

		if (preview->Tint.g == 255 && VGlobal::p()->Input->IsButtonPressed("Click"))
		{
			grabbedBuilding->Angle = 0;
			grabbedBuilding->AngleVelocity = 0;
			grabbedBuilding->Revive();
			grabbedBuilding->SetPositionAtCentre(cursor->Position.x + cursor->Size.x * 0.5f, preview->Position.y + (preview->Size.y * 0.5f));
			
			VPhysicsObject* obj = group->AddObject(grabbedBuilding, VPhysicsObject::DYNAMIC);
			obj->SetFriction(1.f);
			obj->UpdateCollisionFilter(0, ~(1 << 0), ~(1 << 0));
			VGlobal::p()->Sound->Play("land");
			
			grabbedBuilding = NULL;
		}
		
		if (VGlobal::p()->Input->IsButtonPressed("UnClick"))
		{
			availableWeights[grabbedBuilding->Type]++;
			availableWeightsTotal++;

			buildings->Remove(grabbedBuilding, true);
			grabbedBuilding->Destroy();
		}
	}
}

void PlayState::ProcessRequired()
{
	requiredText->SetText("Click to place down this building\nor else we will place it down for you in " + std::to_string(static_cast<int>(ceilf(5.f - requiredTimer->Seconds()))) + ".");

	float x = VGlobal::p()->GetMousePosition().x;

	if (x >= ground->Position.x &&
		x < ground->Position.x + ground->Size.x)
	{
		ShowPreview(requiredBuildings[0], x);
	}

	if (preview->Tint.g == 255 && VGlobal::p()->Input->IsButtonPressed("Click"))
	{
		PlacedRequiredBuilding(x - ground->Position.x);
	}

	if (requiredTimer->Seconds() > 5.f)
	{
		x = VGlobal::p()->Random->GetFloat(ground->Size.x);
		PlacedRequiredBuilding(x);
	}
}

void PlayState::ShowPreview(BuildingType b, float x)
{
	preview->visible = true;
	cursor->visible = false;

	switch (b)
	{
	case HOUSE:
		preview->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(0, 0, 32, 32));
		break;
	case POWER:
		preview->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(32, 64, 64, 48));
		break;
	case SHOP:
		preview->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(64, 0, 48, 32));
		break;
	case APARTMENT:
		preview->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(32, 0, 32, 64));
		break;
	case POLICE:
		preview->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(112, 0, 48, 48));
		break;
	case HOSPITAL:
		preview->LoadGraphic("Assets/Texturess.png", false, 0, 0, sf::IntRect(112, 48, 48, 48));
		break;
	};

	if (VGlobal::p()->Overlaps(preview, buildings))
		preview->Tint = (sf::Color(255, 128, 128, 128));
	else
		preview->Tint = (sf::Color(255, 255, 255, 128));

	float angle = ground->Angle * (VFRAME_PI / 180.f);
	float y = sinf(angle) * (ground->Size.x * 0.5f);
	y *= -(x - (ground->Size.x * 0.5f)) / (ground->Size.x * 0.5f);
	y += ground->Size.y * 0.5f;

	preview->SetPositionAtCentre(x, ground->Position.y + (ground->Size.y * 0.5f) - y - (preview->Size.y / 2));
}

void PlayState::PlacedRequiredBuilding(float x)
{
	CreateBuilding(requiredBuildings[0], x);

	for (int i = 1; i < 3; i++)
		requiredBuildings[i - 1] = requiredBuildings[i];
	requiredBuildings[2] = BuildingType::NUMBUILDINGS;

	requiredTimer->Restart();
}
