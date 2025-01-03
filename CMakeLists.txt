# CMakeLists.txt at root folder
cmake_minimum_required (VERSION 3.0.2)

#-----------------------Options--------------------------------------
#option (LEARNTOOL_WITH_SSL "[LEARNTOOL] Support working with SSL" ON)
#option (LEARNTOOL_PURE_CORE "[LEARNTOOL] Only pure core" OFF)
#--------------------------------------------------------------------

#-----------------------Version--------------------------------------
include(cmake/version.cmake)
#--------------------------------------------------------------------

project (learntool)
set(PROJECT ${PROJECT_NAME})
string(TOLOWER "${PROJECT}" PROJECT_LC)

set (STD_CXX "c++17")

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# CMake Scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Future lib (TODO)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -std=${STD_CXX}")
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -g0 -DNDEBUG")
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

#--------------------------------------------------------------------
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/version/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/version/version.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/core/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/core/config.h)

set (LIBRARIES "")

#------------------------ SFML --------------------------
#/usr/share/SFML/cmake/Modules/FindSFML.cmake
find_package(SFML 2 REQUIRED system window graphics network audio)
include_directories(${SFML_INCLUDE_DIRS})
link_directories(${SFML_LIBRARY_DIRS})

set (LIBRARIES
	${LIBRARIES}
	${SFML_LIBRARIES}
)


#------------------------ OpenCV --------------------------
#/usr/share/OpenCV/OpenCVModules.cmake
#find_package(OpenCV 3 REQUIRED)

#/usr/lib/x86_64-linux-gnu/cmake/opencv4
set(OpenCV_DIR /usr/lib/x86_64-linux-gnu/cmake/opencv4)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})

set (LIBRARIES
	${LIBRARIES}
	${OpenCV_LIBRARIES}
)

#---------------------------------------------------------
set(HEADERS
    ${HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

set(SOURCES
    ${SOURCES}
)

set(SOURCES
	${SOURCES}
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/src/UIState.cpp  
	${CMAKE_CURRENT_SOURCE_DIR}/src/UIMain.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/FolderNavigation.cpp  
	${CMAKE_CURRENT_SOURCE_DIR}/src/Quiz.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/VideoCapture.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SFML_SDK/Game.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SFML_SDK/GUI/Button.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SFML_SDK/GUI/GuiQuiz.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SFML_SDK/GUI/Minimap.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SFML_SDK/GUI/ProgressBar.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SFML_SDK/GUI/StackMenu.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SFML_SDK/GUI/Textbox.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SFML_SDK/GUI/Widget.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SFML_SDK/ResourceManager/ResourceHolder.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SFML_SDK/States/StatePlaying.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SFML_SDK/Util/Animation.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SFML_SDK/Util/FPSCounter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SFML_SDK/Util/FileUtil.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SFML_SDK/Util/Maths.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SFML_SDK/Util/ToggleKey.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/tinyfiledialogs/tinyfiledialogs.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/tinyxml2/tinyxml2.cpp
 )


set(LIBRARIES
    ${LIBRARIES}
)

include_directories (src)
include_directories (${HEADERS})

# Future install include (TODO)
#set (INCLUDE_INSTALL_DIR ${PROJECT_LC}/include
set (INCLUDE_INSTALL_DIR ${PROJECT_LC}/src)
set (LIBS_INSTALL_DIR ${PROJECT_LC}/lib)
set (LIB_INSTALL_DIR lib)

include(CMakePackageConfigHelpers)

# relocatable package
#CONFIGURE_PACKAGE_CONFIG_FILE(<input> <output> INSTALL_DESTINATION <path>
#                                               [PATH_VARS <var1> <var2> ... <varN>]
#                                               [NO_SET_AND_CHECK_MACRO]
#                                               [NO_CHECK_REQUIRED_COMPONENTS_MACRO])
configure_package_config_file(cmake/config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_LC}-config.cmake
    INSTALL_DESTINATION ${LIB_INSTALL_DIR}
    PATH_VARS INCLUDE_INSTALL_DIR LIBS_INSTALL_DIR
)

# generates installation rules for a project
#install(TARGETS targets... [EXPORT <export-name>]
#        [[ARCHIVE|LIBRARY|RUNTIME|FRAMEWORK|BUNDLE|PRIVATE_HEADER|PUBLIC_HEADER|RESOURCE]
#         [DESTINATION <dir>]
#         [INCLUDES DESTINATION [<dir> ...]]
#         [PERMISSIONS permissions...]
#         [CONFIGURATIONS [Debug|Release|...]]
#         [COMPONENT <component>]
#         [OPTIONAL] [NAMELINK_ONLY|NAMELINK_SKIP]
#        ] [...])
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_LC}-config.cmake
    DESTINATION ${LIB_INSTALL_DIR}/../
)

# Future lib
#add_library (${PROJECT_LC} STATIC ${SOURCES})
#target_link_libraries (${PROJECT_LC} ${LIBRARIES})
#install (TARGETS ${PROJECT_LC} EXPORT ${PROJECT_LC} ARCHIVE DESTINATION lib
#install (DIRECTORY src/${PROJECT_LC} DESTINATION src)

add_executable (
	learntool 
	${SOURCES}
)

target_link_libraries(learntool 
	${OpenCV_LIBRARIES} 
	${OpenCV_DEPENDENCIES}
	${SFML_LIBRARIES} 
	${SFML_DEPENDENCIES}
        -lpthread 
)


# Recurse into subdirectories. 
# add_subdirectory (src)

