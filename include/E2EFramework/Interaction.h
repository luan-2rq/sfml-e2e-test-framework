#pragma once

#include "E2EFramework/GameDriver.h"

#include <functional>
#include <string>

namespace E2EFramework
{

class Interaction
{
public:
    using Predicate = std::function<bool(const GameDriver&)>;
    using Action = std::function<void(GameDriver&)>;

    static Interaction Click(int cardIndex);
    static Interaction Restart();
    static Interaction Custom(const std::string& name, Action action);

    Interaction& When(Predicate predicate);
    Interaction& Do();
    Interaction& Do(Action action);
    Interaction& ThenExpect(Predicate predicate, const std::string& expectationDescription = "condition to become true");

    [[nodiscard]] const std::string& name() const;
    [[nodiscard]] const std::string& expectationDescription() const;

    [[nodiscard]] bool canRun(const GameDriver& driver) const;
    [[nodiscard]] bool isExpected(const GameDriver& driver) const;
    void run(GameDriver& driver) const;

private:
    enum class Kind
    {
        Click,
        Restart,
        Custom
    };

    explicit Interaction(Kind kind, std::string label);

    Kind kind_;
    int cardIndex_{ -1 };
    std::string label_;
    Predicate when_;
    Action action_;
    Predicate expect_;
    std::string expectationDescription_{ "condition to become true" };
};

struct RetryPolicy
{
    float timeoutSeconds{ 2.5f };
    float pollIntervalSeconds{ 1.f / 60.f };
};

struct ExecutionResult
{
    bool passed{ false };
    int attempts{ 0 };
    float elapsedSeconds{ 0.f };
    std::string message;
};

ExecutionResult ExecuteUntilExpected(
    GameDriver& driver,
    const Interaction& interaction,
    const RetryPolicy& policy = RetryPolicy{});

struct SessionConfig
{
    GameDriver::Mode mode{ GameDriver::Mode::Headless };
    GameModelConfig modelConfig{};
    std::function<void(GameDriver&)> bootstrap;
    RetryPolicy retryPolicy{};
};

class Session
{
public:
    explicit Session(SessionConfig config = SessionConfig{});

    [[nodiscard]] GameDriver& driver();
    [[nodiscard]] const GameDriver& driver() const;

    ExecutionResult Run(const Interaction& interaction);

private:
    RetryPolicy retryPolicy_;
    GameDriver driver_;
};

}
