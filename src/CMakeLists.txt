add_library(lib_editor parser.cpp interactive.cpp edit_impl.cpp)

target_include_directories(lib_editor INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})