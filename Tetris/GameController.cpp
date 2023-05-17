#include "GameController.h"

char BaseStateMachine::lastKeyHit = 'S';
int Tetris::nHighScore = 0;
int Tetris::nScore = 0;
std::string SaveSystem::fileName;
std::fstream SaveSystem::currentFile;
std::vector<Tetris::Node*> Tetris::vGrounded = {};
std::vector<Tetris::Node*> Tetris::vTemp = {};
int Tetris::board[nMaxHorizontal * nMaxVertical] = {};
Math::float3 Tetris::colors[8] = {
       Math::float3(0, 0, 0),
       Math::float3(1, 0, 0),
       Math::float3(0, 1, 0),
       Math::float3(0, 0, 1),
       Math::float3(1, 1, 0),
       Math::float3(1, 0, 1),
       Math::float3(0, 1, 1),
       Math::float3(1, 1, 1),
};

Tetris::Matrix Tetris::Tetrominos[7] = {
    Tetris::Matrix({
                {0, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 1, 0, 0}
                }),
    Tetris::Matrix({
                {0, 0, 1, 0},
                {0, 1, 1, 0},
                {0, 1, 0, 0},
                {0, 0, 0, 0}
                }),
    Tetris::Matrix({
                {0, 1, 0, 0},
                {0, 1, 1, 0},
                {0, 0, 1, 0},
                {0, 0, 0, 0}
                }),
    Tetris::Matrix({
                {0, 0, 0, 0},
                {0, 1, 1, 0},
                {0, 1, 1, 0},
                {0, 0, 0, 0}
                }),
    Tetris::Matrix({
                {0, 0, 0, 0},
                {0, 1, 1, 0},
                {0, 0, 1, 0},
                {0, 0, 1, 0}
                }),
    Tetris::Matrix({
                {0, 0, 1, 0},
                {0, 1, 1, 0},
                {0, 0, 1, 0},
                {0, 0, 0, 0}
                }),
    Tetris::Matrix({
                {0, 0, 0, 0},
                {0, 1, 1, 0},
                {0, 1, 0, 0},
                {0, 1, 0, 0}
                })
};

void GameController::Load() {
    vLevels = {
        new TetrisMainMenu(),
        new GameLoop(),
        new TetrisGameOver()
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
    if (duration.count() > vLevels[nLevelIndex]->ticks) {
        vLevels[nLevelIndex]->FixedUpdate();
    }
    vLevels[nLevelIndex]->Update();
    Graphics::SetEyePosition(Math::float3(0, 0, 7.5));
}

void GameController::Render() {
    vLevels[nLevelIndex]->Render();
}