#include "E2EFramework/Interaction.h"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace E2EFramework
{

namespace
{

std::string formatSeconds(double seconds)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3) << seconds << "s";
    return oss.str();
}

void appendLogLine(ExecutionResult& result, const std::string& interactionName, const std::string& line)
{
    static const auto processStart = std::chrono::steady_clock::now();
    const auto now = std::chrono::steady_clock::now();
    const double sinceStart = std::chrono::duration<double>(now - processStart).count();

    std::ostringstream formatted;
    formatted << "[" << formatSeconds(sinceStart) << "] [" << interactionName << "] " << line;

    const std::string entry = formatted.str();
    if (!result.log.empty())
    {
        result.log += '\n';
    }
    result.log += entry;
    std::cout << entry << std::endl;
}

}

Interaction::Interaction(std::string label)
    : label_(std::move(label))
{
}

Interaction Interaction::Custom(const std::string& name, Action action)
{
    Interaction interaction(name.empty() ? "Interaction.Custom" : name);
    interaction.action_ = std::move(action);
    return interaction;
}

Interaction& Interaction::When(Predicate predicate)
{
    when_ = std::move(predicate);
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

bool Interaction::canRun(const Driver& driver) const
{
    if (!when_)
    {
        return true;
    }

    return when_(driver);
}

bool Interaction::isExpected(const Driver& driver) const
{
    if (!expect_)
    {
        return false;
    }

    return expect_(driver);
}

void Interaction::run(Driver& driver) const
{
    if (action_)
    {
        action_(driver);
    }
}

ExecutionResult ExecuteUntilExpected(
    Driver& driver,
    const Interaction& interaction,
    const RetryPolicy& policy)
{
    ExecutionResult result;
    bool loggedWhenWaiting = false;
    bool loggedExpectWaiting = false;

    const float timeoutSeconds = std::max(0.01f, policy.timeoutSeconds);
    const float pollIntervalSeconds = std::max(0.001f, policy.pollIntervalSeconds);

    appendLogLine(result, interaction.name(), "WHEN start");

    while (result.elapsedSeconds < timeoutSeconds)
    {
        const bool canRun = interaction.canRun(driver);
        if (!canRun)
        {
            if (!loggedWhenWaiting)
            {
                appendLogLine(result, interaction.name(),
                    "WHEN waiting for precondition at elapsed=" + formatSeconds(result.elapsedSeconds));
                loggedWhenWaiting = true;
            }
        }
        else
        {
            if (loggedWhenWaiting)
            {
                appendLogLine(result, interaction.name(),
                    "WHEN satisfied at elapsed=" + formatSeconds(result.elapsedSeconds));
            }
            else
            {
                appendLogLine(result, interaction.name(), "WHEN satisfied immediately");
            }

            appendLogLine(result, interaction.name(), "DO attempt " + std::to_string(result.attempts + 1));
            interaction.run(driver);
            ++result.attempts;
            appendLogLine(result, interaction.name(), "THEN evaluating expectation");
        }

        const bool expected = interaction.isExpected(driver);
        if (expected)
        {
            result.passed = true;
            appendLogLine(result, interaction.name(),
                "EXPECT satisfied after " + std::to_string(result.attempts) + " attempt(s) at elapsed="
                + formatSeconds(result.elapsedSeconds));
            std::ostringstream ok;
            ok << interaction.name() << " reached expectation in "
               << result.elapsedSeconds << "s after " << result.attempts << " attempt(s).";
            result.message = ok.str();
            return result;
        }

        if (canRun && !loggedExpectWaiting)
        {
            appendLogLine(result, interaction.name(),
                "EXPECT waiting for " + interaction.expectationDescription());
            loggedExpectWaiting = true;
        }

        driver.advance(pollIntervalSeconds);
        result.elapsedSeconds += pollIntervalSeconds;
    }

    appendLogLine(result, interaction.name(),
        "EXPECT timeout after " + formatSeconds(timeoutSeconds)
        + " waiting for " + interaction.expectationDescription());

    std::ostringstream timeout;
    timeout << interaction.name() << " timed out after " << timeoutSeconds
            << "s waiting for " << interaction.expectationDescription() << "."
            << " Attempts=" << result.attempts << ".";
    result.message = timeout.str();
    return result;
}

Session::Session(SessionConfig config)
    : retryPolicy_(config.retryPolicy),
      driver_(config.driverFactory ? config.driverFactory() : nullptr)
{
    if (config.bootstrap)
    {
        config.bootstrap(*driver_);
    }
}

Driver& Session::driver()
{
    return *driver_;
}

const Driver& Session::driver() const
{
    return *driver_;
}

ExecutionResult Session::Run(const Interaction& interaction)
{
    return ExecuteUntilExpected(*driver_, interaction, retryPolicy_);
}

}
