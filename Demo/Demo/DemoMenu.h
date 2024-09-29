#pragma once
#ifndef DEMOMENU_H
#define DEMOMENU_H

#include <AvgEngine/Base/Menu.h>

#include <AvgEngine/External/OpenAL/AL.h>


class DemoMenu : public AvgEngine::Base::Menu
{
public:
	AvgEngine::Audio::Channel* channel;
	void load() override;
	void draw() override;
};

#endif