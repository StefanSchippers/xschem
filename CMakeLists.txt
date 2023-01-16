cmake_minimum_required(VERSION 3.12)

project(xschem LANGUAGES C) 

file(GLOB_RECURSE PROJECT_SOURCE_FILES CONFIGURE_DEPENDS src/*.c)
list(REMOVE_ITEM PROJECT_SOURCE_FILES /home/rafmag/xschem/src/rawtovcd.c)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC src/ /usr/include/cairo /usr/include/tcl8.6)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${CMAKE_DL_LIBS} tcl8.6 tk8.6 cairo xcb X11 xcb-render X11-xcb Xpm m jpeg)

set(EXPAND_LABEL expandlabel)
add_custom_target(expandlabel
    bison -d -o ../src/expandlabel.c ../src/expandlabel.y
)
set(PARSE_LABEL parselabel)
add_custom_target(parselabel
    flex -l -o ../src/parselabel.c ../src/parselabel.l
)
add_dependencies(${PROJECT_NAME} ${EXPAND_LABEL} ${PARSE_LABEL})
