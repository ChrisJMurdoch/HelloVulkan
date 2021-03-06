
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
        src/command/commandBuffer.cpp
        src/command/commandPool.cpp
        src/configuration/debugMessenger.cpp
        src/configuration/device.cpp
        src/configuration/instance.cpp
        src/configuration/physicalDevice.cpp
        src/configuration/queue.cpp
        src/configuration/shaderModule.cpp
        src/configuration/surface.cpp
        src/configuration/window.cpp
        src/core/main.cpp
        src/display/display.cpp
        src/frame/frame.cpp
        src/frame/framePool.cpp
        src/memory/descriptorPool.cpp
        src/memory/descriptorSet.cpp
        src/memory/descriptorSetLayout.cpp
        src/memory/voidBuffer.cpp
        src/swapchain/image.cpp
        src/swapchain/pipeline.cpp
        src/swapchain/renderPass.cpp
        src/swapchain/swapchain.cpp
        src/utility/io.cpp
        src/vertex/vertex.cpp
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
