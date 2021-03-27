cmake_minimum_required(VERSION 3.17)
project(YitIdGen)
set(CMAKE_C_STANDARD 11)

#set(CMAKE_BUILD_TYPE DEBUG)
#set(CMAKE_BUILD_TYPE RELEASE)

aux_source_directory(. DIR_SRCS)
add_subdirectory(idgen)

##编译动态库
#set(LIB_SRC YitIdHelper.h YitIdHelper.c)
#add_library(YitIdGenLib SHARED ${LIB_SRC})
#target_link_libraries(YitIdGenLib idgen)
#set_target_properties(YitIdGenLib PROPERTIES
#        LINKER_LANGUAGE C
#        OUTPUT_NAME "yitidgenc"
#        PREFIX "")

##编译执行文件
set(LIB_SRC YitIdHelper.h YitIdHelper.c)
add_library(YitIdHelper ${LIB_SRC})
add_executable(YitIdGen main.c)
target_link_libraries(YitIdHelper idgen)
target_link_libraries(YitIdGen YitIdHelper)


