#pragma once

namespace E2EFramework
{

class Driver
{
public:
    virtual ~Driver() = default;

    virtual void advance(float dt) = 0;
    [[nodiscard]] virtual bool isRunning() const = 0;
};

}
