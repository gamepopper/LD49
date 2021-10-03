#pragma once
#include <VFrame/VState.h>

/*
City building game where city is on an unstable ground.
Game ends when city topples over or zero people are alive.

*/

class VText;
class VShape;
class VTimer;
class VSprite;
class VObject;
class VEmitter;
class VPhysicsGroup;
class PersonSprite;
class BuildingSprite;
class PlayState : public VState
{
public:
	typedef VState VSUPERCLASS;
	PlayState() = default;

	virtual void Initialise();
	virtual void Update(float dt);

	void ProcessAvailable();

	void ProcessGrabbed();

	void ProcessRequired();
	void PlacedRequiredBuilding(float x);

	enum BuildingType : unsigned char
	{
		POWER, //Needed to run the city
		HOUSE, //More houses mean more people
		APARTMENT, //Apartments mean even more people
		SHOP, //People can travel to and from
		POLICE, //Buildings less likely to be destroyed
		HOSPITAL, //People less likely to die
		NUMBUILDINGS
	};

	void ShowPreview(PlayState::BuildingType b, float x);
	void CreateBuilding(BuildingType type, float x);
	void SpawnPerson(PersonSprite* p);

	VObject* cursor;

	VPhysicsGroup* group;

	VGroup* buildings;
	BuildingSprite* grabbedBuilding = NULL;

	VGroup* people;

	VObject* ground;
	
	BuildingSprite* preview;

	VTimer* requiredTimer;
	BuildingType requiredBuildings[3];
	VText* requiredText;

	VEmitter* weatherEmitter;

	VSprite* mouse;

	BuildingType availableBuildings[5];
	unsigned int availableWeights[NUMBUILDINGS];
	unsigned int availableWeightsTotal;
	VSprite* availableOptions[5];
	VText* availableText;

	bool weather = false;

	unsigned int days;
	VText* dayText;
	int personCount;
};