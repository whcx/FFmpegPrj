cmake_minimum_required(VERSION 3.16)
project(FFmpegWin)

message("begain compile ...")
message("CMAKE_CURRENT_LIST_DIR... ${CMAKE_CURRENT_LIST_DIR}")
message("CMAKE_CURRENT_SOURCE_DIR...${CMAKE_CURRENT_SOURCE_DIR}")
message("CMAKE_BINARY_DIR... ${CMAKE_BINARY_DIR}")

set(CONFIG_NAME "$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

message("srcs ... ${SRC_FILES}")

add_executable(FFmpegWin ${SRC_FILES})

set_property(TARGET FFmpegWin PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CONFIG_NAME}/")

target_include_directories(FFmpegWin
	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(FFmpegWin
	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/lib/avcodec.lib
	${CMAKE_CURRENT_LIST_DIR}/lib/avformat.lib
	${CMAKE_CURRENT_LIST_DIR}/lib/avutil.lib
	${CMAKE_CURRENT_LIST_DIR}/lib/swresample.lib
	${CMAKE_CURRENT_LIST_DIR}/lib/swscale.lib)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
	copy_if_different "${CMAKE_CURRENT_LIST_DIR}/lib/avcodec-58.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CONFIG_NAME}")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
	copy_if_different "${CMAKE_CURRENT_LIST_DIR}/lib/avformat-58.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CONFIG_NAME}")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
	copy_if_different "${CMAKE_CURRENT_LIST_DIR}/lib/avutil-56.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CONFIG_NAME}")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
	copy_if_different "${CMAKE_CURRENT_LIST_DIR}/lib/swresample-3.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CONFIG_NAME}")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
	copy_if_different "${CMAKE_CURRENT_LIST_DIR}/lib/swscale-5.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CONFIG_NAME}")