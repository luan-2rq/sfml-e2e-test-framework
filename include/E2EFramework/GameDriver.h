#pragma once

#include "GameModel.h"

#include <memory>

namespace Engine { class Game; }
class GameScene;

class GameDriver
{
public:
    enum class Mode { Headless, Headed };

    explicit GameDriver(Mode mode = Mode::Headless,
                        GameModelConfig cfg = GameModelConfig{});
    ~GameDriver();

    GameDriver(const GameDriver&) = delete;
    GameDriver& operator=(const GameDriver&) = delete;

    void clickCard(int index);
    void clickRestart();
    void advance(float dt = 1.f / 60.f);
    void settle();

    [[nodiscard]] const GameModel& model() const;
    [[nodiscard]] bool isRunning() const;

private:
    Mode mode_;

    std::unique_ptr<GameModel> headlessModel_;

    std::unique_ptr<Engine::Game> game_;
    GameScene* scenePtr_{ nullptr };
};
