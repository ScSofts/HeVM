
add_library(HeVM STATIC hevm.cc)
target_include_directories(HeVM PUBLIC ../include)
set_target_properties(HeVM PROPERTIES CMAKE_CXX_STANDARD 17)