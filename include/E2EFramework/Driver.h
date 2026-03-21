#pragma once

#include <string>

namespace E2EFramework
{

class Driver
{
public:
    virtual ~Driver() = default;

    virtual void advance(float dt) = 0;
    [[nodiscard]] virtual bool isRunning() const = 0;

    // Optional hook for headed drivers to persist a screenshot artifact.
    virtual bool saveScreenshot(const std::string& outputPath)
    {
        (void)outputPath;
        return false;
    }
};

}
