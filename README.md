# E2E Framework

This module is a standalone CMake project in `E2EFramework/`.

The framework is application-agnostic and depends only on Engine.
It provides only generic driver/session orchestration plus a small set of
baseline predicates in `include/E2EFramework/Conditions.h`.
App-specific drivers, actions, and model predicates live outside this module.

## Public Headers

- `include/E2EFramework/Driver.h`
- `include/E2EFramework/Interaction.h`
- `include/E2EFramework/Conditions.h`
- `include/E2EFramework/PageObject.h`
- `include/E2EFramework/TestCategories.h`

## Core Pattern

Use structured interaction steps:

```cpp
auto result = session.Run(
    E2EFramework::Interaction::Custom("click-card", [](E2EFramework::Driver& driver) {
            // App-specific action implemented by caller.
        })
        .ThenExpect([](const E2EFramework::Driver& driver) {
            // App-specific condition implemented by caller.
            return true;
        }, "clicked card to become face up")
);
```

Each step retries until its expectation is met or a timeout is reached.
Timeout failures return a clear message.

Every interaction also emits timestamped execution logs to stdout for the
`WHEN`, `DO`, `THEN`, and `EXPECT` phases, for example:

```text
[0.016s] [Click card 0] WHEN start
[0.016s] [Click card 0] WHEN satisfied immediately
[0.016s] [Click card 0] DO attempt 1
[0.016s] [Click card 0] THEN evaluating expectation
[0.024s] [Click card 0] EXPECT satisfied after 1 attempt(s) at elapsed=0.000s
```

For generic checks that do not depend on any app model, use the framework
predicates directly:

```cpp
auto result = session.Run(
    E2EFramework::Interaction::Custom("wait-until-running", [](E2EFramework::Driver&) {})
        .ThenExpect(E2EFramework::Conditions::DriverIsRunning(),
            "driver to be running")
);
```

## User-Provided App Bootstrap

`SessionConfig` exposes `driverFactory` and `bootstrap` so consumers can
provide app-specific setup logic:

```cpp
E2EFramework::SessionConfig config;
config.driverFactory = []() -> std::unique_ptr<E2EFramework::Driver> {
    // Return your app-specific driver implementation.
    return makeDriver();
};
config.bootstrap = [](E2EFramework::Driver& driver) {
    driver.advance(0.016f);
};

// Optional: save a screenshot artifact whenever an interaction times out.
config.saveFailureScreenshotArtifact = true;
config.failureArtifactsDirectory = "e2e_failures";
```

If enabled and the driver supports screenshots, timeout failures append the saved
path to `ExecutionResult.message` and also set `ExecutionResult.failureScreenshotPath`.

Drivers can opt in by overriding:

```cpp
bool saveScreenshot(const std::string& outputPath) override;
```

## Test Category Macros

For category-friendly test naming, use macros from
`include/E2EFramework/TestCategories.h`:

```cpp
#include "E2EFramework/TestCategories.h"

E2E_TEST(BoardPage, CardFlipsOnClick, Smoke)
{
    // test logic
}

E2E_TEST_F(BoardFixture, RestartResetsSession, Regression)
{
    // test logic
}
```

## Example Game

Example integration run output (GIF):

![Example E2E test run](docs/example-e2e.gif)
