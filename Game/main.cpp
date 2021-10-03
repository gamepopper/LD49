#include <VFrame/VGame.h>
#include <memory>

#include "TitleState.h"

int main()
{
	std::unique_ptr<VGame> game = std::make_unique<VGame>();

	sf::ContextSettings settings;
	/*settings.depthBits = 24;
	settings.stencilBits = 0;
	settings.antialiasingLevel = 0;
	settings.majorVersion = 4;
	settings.minorVersion = 5;*/
	
	return game->Run("Unstable Ground", new TitleState(), 640, 360, 60.0f, 7, settings);
}
