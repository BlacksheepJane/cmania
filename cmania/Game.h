﻿#pragma once
#include <vector>
#include <string>
#include "SettingStorage.h"

class Game
{
public:
	class Component
	{
	protected:
		class Game* parent = 0;
	public:
		void Init(Game& game_inst)
		{
			parent = &game_inst;
		}
		virtual void ProcessEvent(const char* evt, const void* evtargs) = 0;
	};
private:
	std::vector<Component*> records;
public:
	BinaryStorage Settings;
	Game()
	{
		Settings.Read();
	}
	~Game()
	{
		Settings.Write();
	}
	template<typename T>
	Game& Use()
	{
		auto com = new T();
		com->Init(*this);
		records.emplace_back(com);
		return *this;
	}
	template<typename func>
	Game& Use(func f)
	{
		auto com = f();
		com->Init(*this);
		records.emplace_back(com);
		return *this;
	}
	void Raise(const char* evt)
	{
		for (auto comp : records)
		{
			comp->ProcessEvent(evt,0);
		}
	}
	template<typename EventArgs>
	void Raise(const char* evt, const EventArgs& evtargs)
	{
		for (auto comp : records)
		{
			comp->ProcessEvent(evt, &evtargs);
		}
	}
};
using GameComponent = Game::Component;