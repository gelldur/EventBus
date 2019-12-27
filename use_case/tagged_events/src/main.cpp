#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "EventBus.hpp"
#include "Gui.hpp"
#include "Team.hpp"

int main(int argc, char* argv[])
{
	auto eventBus = std::make_shared<EventBus>();

	Gui gui{eventBus};
	Team myTeam{eventBus};

	auto updateFrame = [&]() {
		// single frame update ;)
		eventBus->process();
		gui.draw();
		std::cout << "###################################################\n";
		std::cout << "###################################################" << std::endl;
	};

	{ // single update
		myTeam.addPlayer("Gelldur");
		updateFrame();
	}

	{ // single update
		myTeam.getMember("Gelldur").pickGold(100);
		updateFrame();
	}

	{ // single update
		myTeam.addPlayer("Gosia");
		myTeam.addPlayer("Dexter");
		updateFrame();
	}

	return 0;
}
