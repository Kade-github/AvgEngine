#include "DemoMenu.h"

void DemoMenu::load()
{
	channel = new AvgEngine::Audio::Channel();
	AvgEngine::External::OpenAL::AL::LoadOgg("Assets/knower.ogg", channel);

	AvgEngine::Base::Menu::load();

	channel->Play();
}

void DemoMenu::draw()
{
	AvgEngine::Base::Menu::draw();
}
