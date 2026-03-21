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

Every interaction also emits timestamped execution logs to stdout for the
`WHEN`, `DO`, `THEN`, and `EXPECT` phases, for example:

```text
[0.016s] [Click card 0] WHEN start
[0.016s] [Click card 0] WHEN satisfied immediately
[0.016s] [Click card 0] DO attempt 1
[0.016s] [Click card 0] THEN evaluating expectation
[0.024s] [Click card 0] EXPECT satisfied after 1 attempt(s) at elapsed=0.000s
```

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

## GIF Capture

For a headed E2E run with execution logs rendered beside the gameplay, use:

```bash
./Tests/Tools/run_e2e_gif.sh
```

Optional test filter:

```bash
./Tests/Tools/run_e2e_gif.sh Board_Cat_Smoke.RestartResetsGame
```

What it does:

- configures a headed build in `BuildHeadedGif/`
- captures rendered frames via `MEMORYGAME_CAPTURE_FRAMES_DIR`
- writes the timestamped `WHEN/DO/THEN/EXPECT` logs to `Artifacts/e2e_gif/e2e.log`
- composes `Artifacts/e2e_gif/memorygame_e2e.gif` with the game on the left and logs on the right

Requirements:

- `ffmpeg`
- `python3`
- a desktop session that can open the headed SFML window
