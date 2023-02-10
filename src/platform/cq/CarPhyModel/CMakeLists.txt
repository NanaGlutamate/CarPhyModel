# set(PROJ_NAME CarPhyModel)

file(GLOB_RECURSE CQ_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
# set(EXPORTED_HDR ${CMAKE_CURRENT_SOURCE_DIR}/CarPhyModel_export.h)
set(SRCS ${MODEL_SRC} ${CQ_SRC})

# include_directories(${CMAKE_CURRENT_BINARY_DIR}
#   ${CMAKE_CURRENT_SOURCE_DIR}/../csconfig/
#   ${CMAKE_CURRENT_SOURCE_DIR}/../csmodel_base/
# )

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

message(STATUS "file:" ${SRCS})
add_library(${PROJ_NAME} SHARED ${SRCS})

if(UNIX)
target_link_libraries(${PROJ_NAME} dl)
set_target_properties(${PROJ_NAME} 
  PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin/models/test-model-id)
else(UNIX)
set_target_properties(${PROJ_NAME} 
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin/$<CONFIG>/models/test-model-id)
endif(UNIX)

# include(GenerateExportHeader)
# generate_export_header(${PROJ_NAME} EXPORT_FILE_NAME ${EXPORTED_HDR})
