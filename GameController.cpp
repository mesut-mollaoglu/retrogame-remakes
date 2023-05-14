#include "GameController.h"

char BaseStateMachine::lastKeyHit = 'S';
int GameFile::nHighScore = 0;
int GameFile::nScore = 0;
std::string SaveSystem::fileName;
std::fstream SaveSystem::currentFile;

void GameController::Load() {
    vLevels = {
        new MainMenu(),
        new MainLevel(),
        new GameOver()
    };
    vLevels[nLevelIndex]->Load();
}

void GameController::Unload() {
    vLevels[nLevelIndex]->UnLoad();
}

void GameController::Update() {
    if (vLevels[nLevelIndex]->nLevel == Level::ManageLevel::CurrentLevel) {

    }
    else if (vLevels[nLevelIndex]->nLevel == Level::ManageLevel::PrevLevel) {
        vLevels[nLevelIndex]->nLevel = Level::ManageLevel::CurrentLevel;
        vLevels[nLevelIndex]->UnLoad();
        nLevelIndex = (nLevelIndex - 1) % vLevels.size();
        vLevels[nLevelIndex]->Load();
    }
    else if (vLevels[nLevelIndex]->nLevel == Level::ManageLevel::NextLevel) {
        vLevels[nLevelIndex]->nLevel = Level::ManageLevel::CurrentLevel;
        vLevels[nLevelIndex]->UnLoad();
        nLevelIndex = (nLevelIndex + 1) % vLevels.size();
        vLevels[nLevelIndex]->Load();
    }
    if (!bInit)
    {
        start = now = Clock::now();
        bInit = true;
    }
    now = Clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    if (duration.count() > 75) {
        vLevels[nLevelIndex]->FixedUpdate();
    }
    vLevels[nLevelIndex]->Update();
    Graphics::SetEyePosition(Math::float3(0, 0, 7.5));
}

void GameController::Render() {
    vLevels[nLevelIndex]->Render();
}