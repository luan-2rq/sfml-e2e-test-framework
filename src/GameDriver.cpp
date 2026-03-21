#include "E2EFramework/GameDriver.h"

#include "Engine/Game.h"
#include "GameScene.h"

#include <SFML/Graphics.hpp>

static constexpr float kRestartCenterX = 300.f;
static constexpr float kRestartCenterY = 597.f;

static sf::Event makeLeftClick(float x, float y)
{
    sf::Event e{};
    e.type = sf::Event::MouseButtonPressed;
    e.mouseButton.button = sf::Mouse::Left;
    e.mouseButton.x = static_cast<int>(x);
    e.mouseButton.y = static_cast<int>(y);
    return e;
}

GameDriver::GameDriver(Mode mode, GameModelConfig cfg)
    : mode_(mode)
{
    if (mode_ == Mode::Headless)
    {
        headlessModel_ = std::make_unique<GameModel>(cfg);
        return;
    }

    game_ = std::make_unique<Engine::Game>("Memory Game E2E", 600, 630);
    auto scene = std::make_unique<GameScene>(*game_, cfg);
    scenePtr_ = scene.get();
    game_->setScene(std::move(scene));
    game_->step(0.f);
}

GameDriver::~GameDriver() = default;

void GameDriver::clickCard(int index)
{
    if (mode_ == Mode::Headless)
    {
        headlessModel_->tryFlip(index);
        return;
    }

    const sf::FloatRect b = scenePtr_->model().cards()[index].getBounds();
    scenePtr_->handleEvent(makeLeftClick(
        b.left + b.width * 0.5f,
        b.top + b.height * 0.5f));
}

void GameDriver::clickRestart()
{
    if (mode_ == Mode::Headless)
    {
        headlessModel_->reset();
        return;
    }

    scenePtr_->handleEvent(makeLeftClick(kRestartCenterX, kRestartCenterY));
}

void GameDriver::advance(float dt)
{
    if (mode_ == Mode::Headless)
    {
        headlessModel_->update(dt);
        return;
    }

    game_->step(dt);
}

void GameDriver::settle()
{
    advance(2.f);
    advance(2.f);
}

const GameModel& GameDriver::model() const
{
    if (mode_ == Mode::Headless)
    {
        return *headlessModel_;
    }

    return scenePtr_->model();
}

bool GameDriver::isRunning() const
{
    if (mode_ == Mode::Headless)
    {
        return true;
    }

    return game_->getWindow().isOpen();
}
