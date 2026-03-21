#pragma once

#include "E2EFramework/Driver.h"

#include <functional>
#include <memory>
#include <string>

namespace E2EFramework
{

class Interaction
{
public:
    using Predicate = std::function<bool(const Driver&)>;
    using Action = std::function<void(Driver&)>;

    static Interaction Custom(const std::string& name, Action action);

    Interaction& When(Predicate predicate);
    Interaction& Do(Action action);
    Interaction& ThenExpect(Predicate predicate, const std::string& expectationDescription = "condition to become true");

    [[nodiscard]] const std::string& name() const;
    [[nodiscard]] const std::string& expectationDescription() const;

    [[nodiscard]] bool canRun(const Driver& driver) const;
    [[nodiscard]] bool isExpected(const Driver& driver) const;
    void run(Driver& driver) const;

private:
    explicit Interaction(std::string label);

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
    Driver& driver,
    const Interaction& interaction,
    const RetryPolicy& policy = RetryPolicy{});

struct SessionConfig
{
    std::function<std::unique_ptr<Driver>()> driverFactory;
    std::function<void(Driver&)> bootstrap;
    RetryPolicy retryPolicy{};
};

class Session
{
public:
    explicit Session(SessionConfig config = SessionConfig{});

    [[nodiscard]] Driver& driver();
    [[nodiscard]] const Driver& driver() const;

    ExecutionResult Run(const Interaction& interaction);

private:
    RetryPolicy retryPolicy_;
    std::unique_ptr<Driver> driver_;
};

}
