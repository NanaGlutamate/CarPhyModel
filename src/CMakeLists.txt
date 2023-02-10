file(GLOB_RECURSE CURRENT_PROJ_SRC ${CMAKE_CURRENT_SOURCE_DIR}/model/**.cpp)
# message(STATUS "src file:" ${CURRENT_PROJ_SRC})
set(MODEL_SRC ${CURRENT_PROJ_SRC} PARENT_SCOPE)

add_subdirectory(platform)