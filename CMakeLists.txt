cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

#Setting the project name
project("TerraMater")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Adding Vulkan
find_package(Vulkan REQUIRED)

#Adding GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/glfw)

#Adding PhysX
include_directories(${CMAKE_SOURCE_DIR}/external/PhysX/physx/include)
include_directories(${CMAKE_SOURCE_DIR}/external/PhysX/pxshared/include)

set(PX_BUILDSNIPPETS ON)
#set(PX_GENERATE_STATIC_LIBRARIES ON)
set(NV_USE_STATIC_WINCRT ON)
#set(NV_USE_DEBUG_WINCRT ON)
set(PHYSX_ROOT_DIR ${CMAKE_SOURCE_DIR}/external/PhysX/physx)
set(PXSHARED_PATH ${PHYSX_ROOT_DIR}/../pxshared)
set(PXSHARED_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CMAKEMODULES_VERSION "1.27")
set(CMAKEMODULES_PATH ${PHYSX_ROOT_DIR}/../externals/cmakemodules)
set(PX_OUTPUT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/PhysX)
set(PX_OUTPUT_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/PhysX)

if (MSVC)
    SET(TARGET_BUILD_PLATFORM "windows")
    set(CMAKE_CXX_FLAGS_RELEASE "/MT")
    #set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
endif (MSVC)

add_subdirectory(${CMAKE_SOURCE_DIR}/external/PhysX/physx/compiler/public)

#include_directories(PX_OUTPUT_LIB_DIR)
include_directories(${CMAKE_SOURCE_DIR}/lib)

#Adding GLM
add_subdirectory(${CMAKE_SOURCE_DIR}/external/glm)

#Adding stb
include_directories(${CMAKE_SOURCE_DIR}/external/stb)

#Adding tinyobjloader
include_directories(${CMAKE_SOURCE_DIR}/external/tinyobjloader)

#Setting the include directory for our code
include_directories(${CMAKE_SOURCE_DIR}/src)

#Adding the headers to the add_executable for them to show up as header files in Visual Studio
SET(HEADERS 
    src/Application.h
    src/Vertex.h
    src/VulkanWrapper.h
    src/DeviceFinder.h
    src/Settings.h
    src/Input.h
    src/InputHandler.h
    src/QueueFamilyIndices.h
    src/PhysicalDeviceSurfaceInfo.h
    src/Queues.h
    src/SwapchainWrapper.h
    src/ImageCreator.h
    src/MemoryHelper.h
    src/FileLoader.h
    src/BufferCreator.h
    src/RenderPassCreator.h
    src/DescriptorWrapper.h
    src/UniformBufferObject.h
    src/PipelineCreator.h
    src/Shader.h
    src/CommandWrapper.h
    src/Texture.h
    src/Camera.h
    src/RenderSynchronisation.h
    src/Cube.h
    src/Chunk.h
    src/LoadedChunks.h
    src/Map.h
    src/RandomSampler.h
    src/PerlinNoise.h
    src/MapGenerator.h
    src/FileWriter.h
    src/BiomData.h
    src/ChunkStack.h
    src/Coordinates.h
    src/LoadedChunkStack.h
    src/ThreadPool.h
    src/TextureArray.h
    src/CubeType.h
    src/LSystem.h
    src/LSystemRule.h
    src/TreeDescription.h
    src/TreeBuilder.h
    src/ComputeWrapper.h
    src/ComputeSettings.h
    src/CameraData.h
    src/BoxData.h
    src/Quad.h
    src/PointLight.h
    src/SkyBox.h
    src/BigVertex.h
    src/Obj.h
    src/ObjType.h
    src/ObjData.h
    src/ObjArray.h
    src/Frustum.h
    src/Plane.h
    src/AABB.h
    src/GuiType.h
    src/GuiHud.h
    src/Profiler.h
    src/BenchmarkStatus.h
    src/WriteBackData.h
    src/SkyWrapper.h
    src/Grass.h
    src/GrassType.h
    src/SkyUBO.h
    src/Noise.h
    src/CloudTexture.h
)
    #src/Physic.h
    #src/PhysicErrorCallback.h

SET(SOURCES
    src/main.cpp
    src/Application.cpp
    src/VulkanWrapper.cpp
    src/DeviceFinder.cpp
    src/Settings.cpp
    src/InputHandler.cpp
    src/QueueFamilyIndices.cpp
    src/Queues.cpp
    src/SwapchainWrapper.cpp
    src/ImageCreator.cpp
    src/MemoryHelper.cpp
    src/FileLoader.cpp
    src/BufferCreator.cpp
    src/RenderPassCreator.cpp
    src/DescriptorWrapper.cpp
    src/PipelineCreator.cpp
    src/Shader.cpp
    src/CommandWrapper.cpp
    src/Texture.cpp
    src/Camera.cpp
    src/RenderSynchronisation.cpp
    src/Cube.cpp
    src/Chunk.cpp
    src/LoadedChunks.cpp
    src/Map.cpp
    src/RandomSampler.cpp
    src/PerlinNoise.cpp
    src/MapGenerator.cpp
    src/FileWriter.cpp
    src/ChunkStack.cpp
    src/LoadedChunkStack.cpp
    src/ThreadPool.cpp
    src/TextureArray.cpp
    src/LSystem.cpp
    src/TreeBuilder.cpp
    src/ComputeWrapper.cpp
    src/ComputeSettings.cpp
    src/SkyBox.cpp
    src/Obj.cpp
    src/ObjData.cpp
    src/ObjArray.cpp
    src/Frustum.cpp
    src/Plane.cpp
    src/AABB.cpp
    src/GuiHud.cpp
    src/Profiler.cpp
    src/SkyWrapper.cpp
    src/Grass.cpp
    src/Noise.cpp
    src/CloudTexture.cpp
)
    #src/Physic.cpp
    #src/PhysicErrorCallback.cpp

#Adding all cpp files which we need for building
add_executable("${PROJECT_NAME}" ${SOURCES} ${HEADERS})

target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_17)

#Adding Vulkan include and lib
target_include_directories("${PROJECT_NAME}" PRIVATE Vulkan::Vulkan)
target_link_libraries("${PROJECT_NAME}" Vulkan::Vulkan)

#Adding GLFW lib
target_link_libraries("${PROJECT_NAME}" glfw)

#Adding GLM lib
target_link_libraries("${PROJECT_NAME}" glm)

#Adding PhysX lib
#target_link_libraries("${PROJECT_NAME}" PhysX PhysXCommon PhysXCooking PhysXFoundation PhysXExtensions_static)
target_link_libraries("${PROJECT_NAME}" 
    ${CMAKE_SOURCE_DIR}/lib/LowLevel_static_64.lib
    ${CMAKE_SOURCE_DIR}/lib/LowLevelAABB_static_64.lib
    ${CMAKE_SOURCE_DIR}/lib/LowLevelDynamics_static_64.lib
    ${CMAKE_SOURCE_DIR}/lib/PhysX_64.lib
    ${CMAKE_SOURCE_DIR}/lib/PhysXCharacterKinematic_static_64.lib
    ${CMAKE_SOURCE_DIR}/lib/PhysXCommon_64.lib
    ${CMAKE_SOURCE_DIR}/lib/PhysXCooking_64.lib
    ${CMAKE_SOURCE_DIR}/lib/PhysXExtensions_static_64.lib
    ${CMAKE_SOURCE_DIR}/lib/PhysXFoundation_64.lib
    ${CMAKE_SOURCE_DIR}/lib/PhysXPvdSDK_static_64.lib
    ${CMAKE_SOURCE_DIR}/lib/PhysXTask_static_64.lib
    ${CMAKE_SOURCE_DIR}/lib/PhysXVehicle_static_64.lib
    #${CMAKE_SOURCE_DIR}/lib/SampleBase_static_64.lib
    #${CMAKE_SOURCE_DIR}/lib/SampleFramework_static_64.lib
    #${CMAKE_SOURCE_DIR}/lib/SamplePlatform_static_64.lib
    #${CMAKE_SOURCE_DIR}/lib/SamplesToolkit_static_64.lib
    ${CMAKE_SOURCE_DIR}/lib/SceneQuery_static_64.lib
    ${CMAKE_SOURCE_DIR}/lib/SimulationController_static_64.lib
    ${CMAKE_SOURCE_DIR}/lib/SnippetRender_static_64.lib
    ${CMAKE_SOURCE_DIR}/lib/SnippetUtils_static_64.lib
    )