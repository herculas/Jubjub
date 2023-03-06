INCLUDE(FetchContent)
FETCHCONTENT_DECLARE(
        googletest
        URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)

SET(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FETCHCONTENT_MAKEAVAILABLE(googletest)