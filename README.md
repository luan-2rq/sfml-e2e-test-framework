# MemoryGame E2E Framework

This module is a standalone CMake project in `E2EFramework/`.

The framework is game-agnostic and depends only on Engine.
It provides only generic driver/session orchestration plus a small set of
baseline predicates in `E2EFramework/Conditions.h`.
Game-specific drivers, actions, and model predicates live outside this module.

## Core Pattern

Use structured interaction steps:

```cpp
auto result = session.Run(
    E2EFramework::Interaction::Custom("click-card", [](E2EFramework::Driver& driver) {
            // Game-specific action implemented by caller.
        })
        .ThenExpect([](const E2EFramework::Driver& driver) {
            // Game-specific condition implemented by caller.
            return true;
        }, "clicked card to become face up")
);
```

Each step retries until its expectation is met or a timeout is reached.
Timeout failures return a clear message.

For generic checks that do not depend on any game model, use the framework
predicates directly:

```cpp
auto result = session.Run(
    E2EFramework::Interaction::Custom("wait-until-running", [](E2EFramework::Driver&) {})
        .ThenExpect(E2EFramework::Conditions::DriverIsRunning(),
            "driver to be running")
);
```

## User-Provided Game Bootstrap

`SessionConfig` exposes `driverFactory` and `bootstrap` so consumers can
provide game-specific setup logic:

```cpp
E2EFramework::SessionConfig config;
config.driverFactory = []() -> std::unique_ptr<E2EFramework::Driver> {
    // Return your game-specific driver implementation.
    return makeDriver();
};
config.bootstrap = [](E2EFramework::Driver& driver) {
    driver.advance(0.016f);
};
```

## Flaky Detection Repeat Option

`memorygame_e2e` supports repeat count via:

- `MEMORYGAME_E2E_REPEAT=<n>`
- `--repeat=<n>`

Examples:

```bash
MEMORYGAME_E2E_REPEAT=10 ./Build/bin/memorygame_e2e
./Build/bin/memorygame_e2e --repeat=10 --gtest_filter=E2E_Headless*
```

## Headless vs Headed

- CI: headless mode (default)
- Local visual runs: configure with `-DMEMORYGAME_HEADED_E2E=ON`
