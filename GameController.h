#pragma once
#include "SaveSystem.h"
#include "GameFile.h"
#include <chrono>
#include <string>
#include <tuple>

class GameController {
public:
	typedef std::chrono::steady_clock Clock;
	std::chrono::time_point<std::chrono::steady_clock> start;
	std::chrono::time_point<std::chrono::steady_clock> now;
	std::chrono::milliseconds duration;
	bool bInit = false;
	int nLevelIndex = 0;
	std::vector<Level*> vLevels;
	void Load();
	void Unload();
	void Update();
	void Render();
};