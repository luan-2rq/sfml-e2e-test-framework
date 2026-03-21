#pragma once

#include "E2EFramework/Interaction.h"

namespace E2EFramework
{

// ---------------------------------------------------------------------------
// PageObject — base class for page objects that wrap a GameDriver.
//
// Usage:
//   GameBoardPage board(session);
//   board.clickCard(0).thenExpect(Expect::card(0).isFaceUp());
// ---------------------------------------------------------------------------
class PageObject
{
public:
    explicit PageObject(Session& session) : session_(session) {}
    virtual ~PageObject() = default;

    PageObject(const PageObject&) = delete;
    PageObject& operator=(const PageObject&) = delete;

protected:
    [[nodiscard]] Session& session() { return session_; }
    [[nodiscard]] const GameDriver& driver() const { return session_.driver(); }

    ExecutionResult run(Interaction interaction) const
    {
        return session_.Run(std::move(interaction));
    }

private:
    Session& session_;
};

}  // namespace E2EFramework
