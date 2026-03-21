# MemoryGame E2E Framework

This module is a standalone CMake project in `E2EFramework/`.

## Core Pattern

Use structured interaction steps:

```cpp
auto result = session.Run(
    E2EFramework::Interaction::Click(0)
        .When([](const GameDriver& d) {
            return d.model().state() == GameState::Playing;
        })
        .Do()
        .ThenExpect([](const GameDriver& d) {
            return d.model().cards()[0].isFaceUp();
        }, "clicked card to become face up")
);
```

Each step retries until its expectation is met or a timeout is reached.
Timeout failures return a clear message.

## User-Provided Game Bootstrap

`SessionConfig` exposes `bootstrap` so consumers can provide game-init logic:

```cpp
E2EFramework::SessionConfig config;
config.bootstrap = [](GameDriver& driver) {
    // Example: click menu/start, load save, etc.
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
