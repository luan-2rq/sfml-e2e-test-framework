#pragma once

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Test category system
//
// Annotate any E2E test with a category so it can be run on its own:
//
//   E2E_TEST(BoardPage, CardFlipsOnClick, Smoke)
//   {
//       // test body
//   }
//
// Then on the command line:
//   ./memorygame_e2e --category=Smoke
//   ./memorygame_e2e --category=Regression
//
// The category is encoded in the test name as <Suite>_Cat_<Category>/<Name>,
// making it selectable via the standard --gtest_filter= flag too:
//   ./memorygame_e2e --gtest_filter=*_Cat_Smoke*
//
// Multiple categories per test are not supported intentionally — use a
// descriptive suite name instead.
// ---------------------------------------------------------------------------

// Registers a test associated with the given category string.
// The physical GTest suite name becomes <Suite>_Cat_<Category>.
#define E2E_TEST(Suite, Name, Category) \
    TEST(Suite##_Cat_##Category, Name)

// Fixture variant — the fixture class is unchanged; only the suite name gains
// the category segment so existing SetUp/TearDown logic still applies.
#define E2E_TEST_F(Fixture, Name, Category) \
    TEST_F(Fixture##_Cat_##Category, Name)
