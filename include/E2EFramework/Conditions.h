#pragma once

#include "E2EFramework/Interaction.h"

#include <utility>

namespace E2EFramework::Conditions
{

using Condition = Interaction::Predicate;

inline Condition Always()
{
    return [](const Driver&)
    {
        return true;
    };
}

inline Condition Never()
{
    return [](const Driver&)
    {
        return false;
    };
}

inline Condition DriverIsRunning()
{
    return [](const Driver& driver)
    {
        return driver.isRunning();
    };
}

inline Condition DriverHasStopped()
{
    return [](const Driver& driver)
    {
        return !driver.isRunning();
    };
}

inline Condition Not(Condition condition)
{
    return [condition = std::move(condition)](const Driver& driver)
    {
        return condition && !condition(driver);
    };
}

}