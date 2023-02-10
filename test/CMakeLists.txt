file(GLOB CURRENT_PROJ_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(TEST_SRC ${CURRENT_PROJ_SRC})
# message(STATUS "test" ${TEST_SRC})
add_executable(Test ${MODEL_SRC} ${TEST_SRC})