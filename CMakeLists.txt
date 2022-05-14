
# ===== PARAMETERS =====

# App name
set(APP_NAME "HelloVulkan")

# C++ version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDebug)

# Dependencies
set(VULKAN_DIR "C:/VulkanSDK/1.3.211.0")
set(GLFW_DIR   "C:/glfw-3.3.7")
set(GLM_DIR    "C:/glm-0.9.9.8")

# ===== BUILDSCRIPT =====

# CMake version
cmake_minimum_required(VERSION 3.23)

# Project
project(${APP_NAME} VERSION 0.1.0)

# Executable
add_executable(${APP_NAME})

# Add sources
target_sources(${APP_NAME}
    PRIVATE
        src/core/main.cpp
        
        src/utility/io.cpp

        src/display/display.cpp
        src/display/choose.cpp
        src/display/renderPass.cpp
        src/display/shaderModule.cpp
        src/display/pipeline.cpp
        src/display/commandPool.cpp
        src/display/instance.cpp
        src/display/debugMessenger.cpp
        src/display/physicalDevice.cpp
        src/display/device.cpp
        src/display/window.cpp
        src/display/surface.cpp
        src/display/swapchain.cpp
)

# Add includes
target_include_directories(${APP_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${VULKAN_DIR}/Include
        ${GLFW_DIR}/include
        ${GLM_DIR}/include
)

# GLFW
target_link_libraries(${APP_NAME}
    ${VULKAN_DIR}/Lib/vulkan-1.lib
    ${GLFW_DIR}/lib-vc2022/glfw3.lib
)
