ENABLE_TESTING()

FILE(GLOB_RECURSE TEST_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)
ADD_EXECUTABLE(
        Jubjub_Test
        ${TEST_FILES}
)

TARGET_LINK_LIBRARIES(
        Jubjub_Test
        GTest::gtest_main
        Jubjub
)

INCLUDE(GoogleTest)
GTEST_DISCOVER_TESTS(Jubjub_Test)