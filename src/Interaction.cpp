#include "E2EFramework/Interaction.h"

#include <algorithm>
#include <sstream>

namespace E2EFramework
{

Interaction::Interaction(Kind kind, std::string label)
    : kind_(kind), label_(std::move(label))
{
}

Interaction Interaction::Click(int cardIndex)
{
    Interaction interaction(Kind::Click, "Interaction.Click");
    interaction.cardIndex_ = cardIndex;
    return interaction;
}

Interaction Interaction::Restart()
{
    return Interaction(Kind::Restart, "Interaction.Restart");
}

Interaction Interaction::Custom(const std::string& name, Action action)
{
    Interaction interaction(Kind::Custom, name.empty() ? "Interaction.Custom" : name);
    interaction.action_ = std::move(action);
    return interaction;
}

Interaction& Interaction::When(Predicate predicate)
{
    when_ = std::move(predicate);
    return *this;
}

Interaction& Interaction::Do()
{
    if (kind_ == Kind::Click)
    {
        const int index = cardIndex_;
        action_ = [index](GameDriver& driver)
        {
            driver.clickCard(index);
        };
    }
    else if (kind_ == Kind::Restart)
    {
        action_ = [](GameDriver& driver)
        {
            driver.clickRestart();
        };
    }

    return *this;
}

Interaction& Interaction::Do(Action action)
{
    action_ = std::move(action);
    return *this;
}

Interaction& Interaction::ThenExpect(Predicate predicate, const std::string& expectationDescription)
{
    expect_ = std::move(predicate);
    expectationDescription_ = expectationDescription.empty() ? "condition to become true" : expectationDescription;
    return *this;
}

const std::string& Interaction::name() const
{
    return label_;
}

const std::string& Interaction::expectationDescription() const
{
    return expectationDescription_;
}

bool Interaction::canRun(const GameDriver& driver) const
{
    if (!when_)
    {
        return true;
    }

    return when_(driver);
}

bool Interaction::isExpected(const GameDriver& driver) const
{
    if (!expect_)
    {
        return false;
    }

    return expect_(driver);
}

void Interaction::run(GameDriver& driver) const
{
    if (action_)
    {
        action_(driver);
    }
}

ExecutionResult ExecuteUntilExpected(
    GameDriver& driver,
    const Interaction& interaction,
    const RetryPolicy& policy)
{
    ExecutionResult result;

    const float timeoutSeconds = std::max(0.01f, policy.timeoutSeconds);
    const float pollIntervalSeconds = std::max(0.001f, policy.pollIntervalSeconds);

    while (result.elapsedSeconds < timeoutSeconds)
    {
        if (interaction.canRun(driver))
        {
            interaction.run(driver);
            ++result.attempts;
        }

        if (interaction.isExpected(driver))
        {
            result.passed = true;
            std::ostringstream ok;
            ok << interaction.name() << " reached expectation in "
               << result.elapsedSeconds << "s after " << result.attempts << " attempt(s).";
            result.message = ok.str();
            return result;
        }

        driver.advance(pollIntervalSeconds);
        result.elapsedSeconds += pollIntervalSeconds;
    }

    std::ostringstream timeout;
    timeout << interaction.name() << " timed out after " << timeoutSeconds
            << "s waiting for " << interaction.expectationDescription() << "."
            << " Attempts=" << result.attempts << ".";
    result.message = timeout.str();
    return result;
}

Session::Session(SessionConfig config)
    : retryPolicy_(config.retryPolicy),
      driver_(config.mode, config.modelConfig)
{
    if (config.bootstrap)
    {
        config.bootstrap(driver_);
    }
}

GameDriver& Session::driver()
{
    return driver_;
}

const GameDriver& Session::driver() const
{
    return driver_;
}

ExecutionResult Session::Run(const Interaction& interaction)
{
    return ExecuteUntilExpected(driver_, interaction, retryPolicy_);
}

}
