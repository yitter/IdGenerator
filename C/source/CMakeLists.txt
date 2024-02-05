cmake_minimum_required(VERSION 3.13)
project(YitIdGen)
set(CMAKE_C_STANDARD 11)

#set(CMAKE_BUILD_TYPE DEBUG)
#set(CMAKE_BUILD_TYPE RELEASE)

aux_source_directory(. DIR_SRCS)
add_subdirectory(idgen)

# 在idgen部分增加-fPIC选项
# 原因是由于在生成静态库 libidgen.a 时，SnowWorkerM1.c.o中的某些符号（这里是 ThreadMutex）没有被正确地编译为位置无关代码（Position Independent Code, PIC）。对于共享库，所有包含的代码都必须是位置无关的，以便能够在内存中任意位置加载
target_compile_options(idgen PRIVATE -fPIC)

#编译动态库
set(LIB_SRC YitIdHelper.h YitIdHelper.c)
add_library(YitIdGenLib SHARED ${LIB_SRC})
target_link_libraries(YitIdGenLib idgen)
set_target_properties(YitIdGenLib PROPERTIES
       LINKER_LANGUAGE C
       OUTPUT_NAME "yitidgenc"
       PREFIX "")

# 编译执行文件
# set(LIB_SRC YitIdHelper.h YitIdHelper.c)
add_library(YitIdHelper ${LIB_SRC})
add_executable(YitIdGen main.c)
target_link_libraries(YitIdGen YitIdHelper pthread)
target_link_libraries(YitIdGen idgen pthread)


