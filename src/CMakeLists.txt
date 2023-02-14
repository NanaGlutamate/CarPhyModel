file(GLOB_RECURSE CURRENT_PROJ_SRC ${CMAKE_CURRENT_SOURCE_DIR}/model/**.cpp)
set(MODEL_SRC ${CURRENT_PROJ_SRC} PARENT_SCOPE)
set(MODEL_SRC ${CURRENT_PROJ_SRC})
# message(STATUS "src file:" ${MODEL_SRC})

add_subdirectory(platform)