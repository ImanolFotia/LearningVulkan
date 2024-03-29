cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 20)

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)
set(DBUILD_SHARED_LIBS ON CACHE BOOL "Build as a shared library")
set(DGENERATE_RPM_PACKAGE OFF CACHE BOOL "Generate RPM deployment package")
set(EPSILON_BUILD_LOCATION ${CMAKE_BINARY_DIR})
set(DUSE_GLFW 1)
set(DBUILD_ANDROID 0)

add_compile_definitions(USE_GLFW=${DUSE_GLFW})
add_compile_definitions(BUILD_ANDROID=${DBUILD_ANDROID})

# use, i.e. don't skip the full RPATH for the build tree
set(SET_CUSTOM_RPATH TRUE)
set(DESTDIR ~)
set(CPACK_SET_DESTDIR ~/.local)
if(SET_CUSTOM_RPATH)
set(CMAKE_SKIP_BUILD_RPATH FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib:../lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_BUILD_RPATH "${CMAKE_INSTALL_PREFIX}/lib:../lib")

set_target_properties( ${PROJECT_NAME} PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_RPATH}" )

endif()

option(BUILD_AS_LIBRARY "Build as a library" ON)

include(ExternalProject)
include(FindPkgConfig)

#Download and include glm
ExternalProject_Add(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        INSTALL_COMMAND cmake -E echo "Skipping install step."
        UPDATE_COMMAND ""
        )

ExternalProject_Get_property(glm SOURCE_DIR)
set(GLM_BUILD_DIR ${SOURCE_DIR})
message(${GLM_BUILD_DIR})

include_directories(${GLM_BUILD_DIR})


#Download and include vma
ExternalProject_Add(vma
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        INSTALL_COMMAND cmake -E echo "Skipping install step."
        UPDATE_COMMAND ""
        )

ExternalProject_Get_property(vma SOURCE_DIR)
set(VMA_BUILD_DIR ${SOURCE_DIR})
message(${VMA_BUILD_DIR})

include_directories(${VMA_BUILD_DIR}/include)


#Download and include stb-image
ExternalProject_Add(stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        INSTALL_COMMAND cmake -E echo "Skipping install step for stb-image."
        BUILD_COMMAND ""
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        )

ExternalProject_Get_property(stb SOURCE_DIR)
set(STB_BUILD_DIR ${SOURCE_DIR})
message(${STB_BUILD_DIR})

include_directories(${STB_BUILD_DIR})


#Download and include Beacon
ExternalProject_Add(beacon
        GIT_REPOSITORY https://github.com/ImanolFotia/beacon.git
        INSTALL_COMMAND cmake -E echo "Skipping install step for beacon."
        BUILD_COMMAND ""
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        )

ExternalProject_Get_property(beacon SOURCE_DIR)
set(BEACON_BUILD_DIR ${SOURCE_DIR})
message(${BEACON_BUILD_DIR})

include_directories(${BEACON_BUILD_DIR})


#Download and include nlohmann/json
ExternalProject_Add(json
        GIT_REPOSITORY https://www.github.com/nlohmann/json.git
        INSTALL_COMMAND cmake -E echo "Skipping install step for json."
        BUILD_COMMAND ""
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        )

ExternalProject_Get_property(json SOURCE_DIR)
set(JSON_BUILD_DIR ${SOURCE_DIR})
message(${JSON_BUILD_DIR})

include_directories(${JSON_BUILD_DIR}/include/nlohmann)


##
#Download and include Assimp
set(ASSIMP_WARNINGS_AS_ERRORS 0)
ExternalProject_Add(assimp_d
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        INSTALL_COMMAND cmake -E echo "Skipping install step."
        UPDATE_COMMAND ""
        )

ExternalProject_Get_property(assimp_d SOURCE_DIR)
ExternalProject_Get_property(assimp_d BINARY_DIR)
set(ASSIMP_BUILD_DIR ${BINARY_DIR})
message(${SOURCE_DIR})
message(${ASSIMP_BUILD_DIR})

include_directories(${SOURCE_DIR}/include)
link_directories(${ASSIMP_BUILD_DIR}/bin)

FILE(GLOB files "${ASSIMP_BUILD_DIR}/bin/*.so.*" "${ASSIMP_BUILD_DIR}/bin/*.so")

install(FILES ${files}

  DESTINATION lib
  )


#install( assimp_d LIBRARY DESTINATION lib)

##

# download and include glfw3
ExternalProject_Add(glfw3_d
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        INSTALL_COMMAND cmake -E echo "Skipping install step."
        UPDATE_COMMAND ""
        )

ExternalProject_Get_property(glfw3_d SOURCE_DIR)
ExternalProject_Get_property(glfw3_d BINARY_DIR)
set(GLFW3_BUILD_DIR ${BINARY_DIR})
message(${SOURCE_DIR})
message(${GLFW3_BUILD_DIR})

include_directories(${SOURCE_DIR}/include)
link_directories(${GLFW3_BUILD_DIR}/src)

set(CMAKE_OPENAL_ARGS "")
if(WIN32)
        set(CMAKE_OPENAL_ARGS "")
else()
        set(CMAKE_OPENAL_ARGS "-DALSOFT_REQUIRE_PULSEAUDIO=ON -DALSOFT_REQUIRE_PIPEWIRE=ON")
endif()

# download and include openal
#requires installation of pulse audio and pipewire developer libs in the OS
ExternalProject_Add(openal_d
        GIT_REPOSITORY https://github.com/kcat/openal-soft.git
        CMAKE_ARGS ${CMAKE_OPENAL_ARGS}
        INSTALL_COMMAND cmake -E echo "Skipping install step."
        UPDATE_COMMAND ""
        )

ExternalProject_Get_property(openal_d SOURCE_DIR)
ExternalProject_Get_property(openal_d BINARY_DIR)
set(OPENAL_BUILD_DIR ${BINARY_DIR})
message(${SOURCE_DIR})
message(${OPENAL_BUILD_DIR})

include_directories(${SOURCE_DIR}/include)
link_directories(${OPENAL_BUILD_DIR})


# download and include imgui
#requires installation of pulse audio and pipewire developer libs in the OS
ExternalProject_Add(imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        INSTALL_COMMAND cmake -E echo "Skipping install step for beacon."
        BUILD_COMMAND ""
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        )

ExternalProject_Get_property(imgui SOURCE_DIR)
set(IMGUI_BUILD_DIR ${SOURCE_DIR})
message(${IMGUI_BUILD_DIR})

include_directories(${IMGUI_BUILD_DIR})

SET(HEADERS
        ${HEADERS}
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/framework
        ${CMAKE_SOURCE_DIR}/vk
        ${CMAKE_SOURCE_DIR}/engine
        )

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

set(EPSILON_SOURCE_LOCATION ${CMAKE_SOURCE_DIR})

option(CREATE_STATIC_LIB "Build static library" OFF)

project(Epsilon VERSION 0.1.0)

set(PROJECT_FILES
        ${CMAKE_SOURCE_DIR}/core/framework/env.cpp
        ${CMAKE_SOURCE_DIR}/main_lib.cpp
        ${CMAKE_SOURCE_DIR}/core/engine/renderer/vulkan/vulkan_impl.cpp
        ${CMAKE_SOURCE_DIR}/core/engine/renderer/vulkan/vulkan.cpp
        ${CMAKE_SOURCE_DIR}/core/engine/renderer/vulkan/resource_manager.cpp
        ${CMAKE_SOURCE_DIR}/core/framework/IO/IO.hpp
        ${CMAKE_SOURCE_DIR}/core/framework/IO/KeyBoard.cpp
        ${CMAKE_SOURCE_DIR}/core/framework/IO/KeyBoard.hpp
        ${CMAKE_SOURCE_DIR}/core/framework/IO/KeyboardWrapper.hpp
        ${CMAKE_SOURCE_DIR}/core/framework/IO/Mouse.hpp
        ${CMAKE_SOURCE_DIR}/core/framework/IO/Joystick/Joystick.hpp
        ${CMAKE_SOURCE_DIR}/core/framework/IO/Joystick/JoystickWrapper.hpp
        ${CMAKE_SOURCE_DIR}/core/framework/IO/Joystick/Mappings.hpp
        ${CMAKE_SOURCE_DIR}/core/framework/IO/IO.cpp
        ${CMAKE_SOURCE_DIR}/core/framework/utils/helpers/camera.cpp
        ${CMAKE_SOURCE_DIR}/core/framework/utils/helpers/camera.hpp
        ${CMAKE_SOURCE_DIR}/apis/al/listener.hpp
        ${CMAKE_SOURCE_DIR}/apis/al/source.hpp
        ${CMAKE_SOURCE_DIR}/apis/al/al_data.hpp
        ${CMAKE_SOURCE_DIR}/apis/al/device.hpp
        ${CMAKE_SOURCE_DIR}/apis/al/listener.hpp
        ${CMAKE_SOURCE_DIR}/core/framework/audio/WAVFile.h
        ${CMAKE_SOURCE_DIR}/core/framework/window.cpp
        ${CMAKE_SOURCE_DIR}/core/framework/window.cpp
        ${CMAKE_SOURCE_DIR}/core/framework/clock.cpp
        ${CMAKE_SOURCE_DIR}/modules/mono/mono-instance.hpp
        ${CMAKE_SOURCE_DIR}/Epsilon.cpp
        ${CMAKE_SOURCE_DIR}/Epsilon.hpp
        ${CMAKE_SOURCE_DIR}/core/framework/singleton.hpp)

if (BUILD_AS_LIBRARY)

    if (CREATE_STATIC_LIB)
        add_library(${PROJECT_NAME} STATIC main_lib.cpp)
    else ()
        add_library(${PROJECT_NAME} SHARED main_lib.cpp)
    endif ()

    if (MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W2)
    else ()
        #target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
    endif ()
else ()
    add_executable(${PROJECT_NAME} main.cpp
            ${PROJECT_FILES})
endif ()

target_sources(${PROJECT_NAME} PUBLIC
        ${PROJECT_FILES})

find_package(Vulkan REQUIRED)


#if(BUILD_ANDROID EQUAL 0)
        pkg_check_modules(Mono mono-2)

if(${Mono_FOUND})
        set(DMONO_ENABLED true)
        add_compile_definitions(MONO_ENABLED=${DMONO_ENABLED})
        target_link_libraries(${PROJECT_NAME} PRIVATE ${Mono_LINK_LIBRARIES})
        target_include_directories(${PROJECT_NAME} PRIVATE ${Mono_INCLUDE_DIRS})
endif()

        target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})
        target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIR})
#endif()

target_link_libraries(${PROJECT_NAME} PRIVATE glfw3)

if(BUILD_ANDROID EQUAL 0)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp)
endif()
if(WIN32)
target_link_libraries(${PROJECT_NAME} PRIVATE openal32)
else()
target_link_libraries(${PROJECT_NAME} PRIVATE openal)
endif()

add_dependencies(${PROJECT_NAME} vma)
add_dependencies(${PROJECT_NAME} glfw3_d)

add_dependencies(${PROJECT_NAME} openal_d)
if(DBUILD_ANDROID EQUAL 0)
add_dependencies(${PROJECT_NAME} openal_d)
add_dependencies(${PROJECT_NAME} assimp_d)
else()

endif()
add_dependencies(${PROJECT_NAME} glm)
add_dependencies(${PROJECT_NAME} beacon)
add_dependencies(${PROJECT_NAME} stb)
add_dependencies(${PROJECT_NAME} imgui)
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_DL_LIBS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${PROJECT_NAME} PUBLIC -rdynamic)

#add_dependencies(${PROJECT_NAME} LearningVulkan)
#
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
set(CPACK_PROJECT_NAME "Epsilon Engine")

if(DGENERATE_RPM_PACKAGE)
        set(CPACK_GENERATOR RPM)
        set(CPACK_RPM_COMPONENT_INSTALL ON)
        set(CPACK_RPM_PACKAGE_GROUP "Epsilon")
        set(CPACK_RPM_PACKAGE_LICENSE "MIT")
        set(CPACK_RPM_MAIN_COMPONENT major)
endif()

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)
include(CPack)

execute_process(
        COMMAND bash -c ${CMAKE_SOURCE_DIR}/build_shaders.sh
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE outVar
)

FILE(GLOB shaders "${CMAKE_SOURCE_DIR}/assets/shaders/chess/*.spv" )
install(FILES ${shaders} DESTINATION "bin/assets/shaders/chess/")


FILE(GLOB images "${CMAKE_SOURCE_DIR}/assets/images" )
install(DIRECTORY ${images} DESTINATION "bin/assets/images")

#add_subdirectory(examples/basic)
add_subdirectory(examples/chess)
add_subdirectory(examples/model)
add_subdirectory(examples/bsp)

if(${Mono_FOUND})
add_subdirectory(examples/mono-managed)
endif()

if(BUILD_ANDROID EQUAL 1)
    add_subdirectory(platform/android/app/src/main/cpp)
endif()