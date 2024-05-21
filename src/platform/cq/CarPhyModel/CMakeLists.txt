file(GLOB_RECURSE CQ_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(SRCS ${MODEL_SRC} ${CQ_SRC})

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

add_library(${PROJ_NAME} SHARED ${SRCS})
find_package(GDAL CONFIG REQUIRED)
target_link_libraries(${PROJ_NAME} PRIVATE GDAL::GDAL)

# if(MSVC)
#   SET_TARGET_PROPERTIES(${PROJ_NAME} PROPERTIES LINK_FLAGS "/PROFILE")
# endif()

if(UNIX)
target_link_libraries(${PROJ_NAME} dl)
# set_target_properties(${PROJ_NAME} 
#   PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin/models/test-model-id)
else(UNIX)
# set_target_properties(${PROJ_NAME} 
#   PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin/$<CONFIG>/models/test-model-id)
endif(UNIX)

# include(GenerateExportHeader)
# generate_export_header(${PROJ_NAME} EXPORT_FILE_NAME ${EXPORTED_HDR})
