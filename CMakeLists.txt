cmake_minimum_required(VERSION 3.20)
project(SyntonicDentiforms)

set (SOURCES
    src/animator/AnimationBunch.cpp
    src/animator/AnimationBunch.h
    src/animator/AnimCurve.h
    src/animator/AnimType.h
    src/animator/SampledAnimation.h
    
    src/demo/Anim.cpp
    src/demo/Anim.h
    src/demo/Billboards.cpp
    src/demo/Billboards.h
    src/demo/Camera.cpp
    src/demo/Camera.h
    src/demo/DataFiles.cpp
    src/demo/DataFiles.h
    src/demo/Demo.cpp
    src/demo/Demo.h
    src/demo/DemoResources.h
    src/demo/DynamicVB.cpp
    src/demo/DynamicVB.h
    src/demo/ExternalsImpl.cpp
    src/demo/Glare.cpp
    src/demo/Glare.h
    src/demo/LineRenderer.cpp
    src/demo/LineRenderer.h
    src/demo/main.cpp
    src/demo/MeshEntity.cpp
    src/demo/MeshEntity.h
    src/demo/MusicPlayer.cpp
    src/demo/MusicPlayer.h
    src/demo/Pipelines.cpp
    src/demo/Pipelines.h
    src/demo/Scene.cpp
    src/demo/Scene.h
    src/demo/SceneData.h
    src/demo/SceneOut.cpp
    src/demo/SceneOut.h
    src/demo/SceneTeeth.cpp
    src/demo/SceneTeeth.h
    src/demo/TextureProjector.cpp
    src/demo/TextureProjector.h

    src/gfx/Mesh.cpp
    src/gfx/Mesh.h
    src/gfx/Texture.cpp
    src/gfx/Texture.h
    
    src/math/AABox.h
    src/math/Constants.h
    src/math/Interpolation.h
    src/math/Line3.h
    src/math/MathUtils.h
    src/math/Matrix4x4.h
    src/math/Plane.h
    src/math/Quaternion.h
    src/math/TypeTraits.h
    src/math/Vector3.cpp
    src/math/Vector3.h
    src/math/Vector4.cpp
    src/math/Vector4.h
)

add_executable(Demo ${SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

target_include_directories(Demo PRIVATE
    ${CMAKE_SOURCE_DIR}
)

set_property(TARGET Demo PROPERTY CXX_STANDARD 17)
set_property(TARGET Demo PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_compile_definitions(Demo PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    _CRT_SECURE_NO_WARNINGS
    NOMINMAX
)

set_target_properties(Demo PROPERTIES
    OUTPUT_NAME_DEBUG   "Demo_d"
    OUTPUT_NAME_RELEASE "Demo"
    RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"
    VS_DEBUGGER_WORKING_DIRECTORY    "${CMAKE_SOURCE_DIR}"
)

if (APPLE)
    target_compile_options(Demo PRIVATE -x objective-c++)
    target_link_libraries(Demo PRIVATE
        "-framework Cocoa"
        "-framework Metal"
        "-framework MetalKit"
        "-framework QuartzCore"
        "-framework AudioToolbox"
    )
endif ()
if (LINUX)
    find_package(Threads REQUIRED)
    target_link_libraries(Demo PRIVATE X11 Xi Xcursor GL asound dl m Threads::Threads)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	if (MSVC)
		target_compile_options(Demo PRIVATE /Zi)
		target_link_options(Demo PRIVATE /DEBUG /OPT:ICF /OPT:REF)
	endif()
	if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		target_compile_options(Demo PRIVATE -g)
		target_link_options(Demo PRIVATE -g)
	endif()
endif ()

function(add_sokol_shader target_name shader_file)
    set(SOKOL_SHDC "${CMAKE_SOURCE_DIR}/tools/sokol-shdc")
    get_filename_component(shader_name "${shader_file}" NAME)
    set(SHADER_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${shader_name}.h")
    set(SHADER_DEPFILE "${CMAKE_CURRENT_BINARY_DIR}/${shader_name}.d")
    set(SHADER_ARGS "")
    if(WIN32)
        set(SHADER_BACKENDS "hlsl5")
        set(SHADER_ARGS "--bytecode")
    elseif(APPLE)
        set(SHADER_BACKENDS "metal_macos")
        set(SHADER_ARGS "--bytecode")
    else()
        set(SHADER_BACKENDS "glsl430")
    endif()

    add_custom_command(
        OUTPUT ${SHADER_OUTPUT}
        COMMAND ${SOKOL_SHDC}
            --input ${CMAKE_SOURCE_DIR}/${shader_file}
            --output ${SHADER_OUTPUT}
            --slang ${SHADER_BACKENDS}
            --dependency-file ${SHADER_DEPFILE}
            ${SHADER_ARGS}
        DEPENDS ${CMAKE_SOURCE_DIR}/${shader_file}
        DEPFILE "${SHADER_DEPFILE}"
        COMMENT "Compiling shader ${shader_file} with sokol-shdc"
        VERBATIM
    )
    target_sources(${target_name} PRIVATE ${SHADER_OUTPUT})
endfunction()

set(SHADER_FILES
    src/fx/billboards.glsl
    src/fx/blit.glsl
    src/fx/shadowCaster.glsl
    src/fx/postComposeBloom.glsl
    src/fx/postComposeToon.glsl
    src/fx/postBlurStep.glsl
    src/fx/postToon.glsl
    src/fx/lines.glsl
    src/fx/renderLit.glsl
    src/fx/overlay.glsl
    src/fx/renderLitShadowed.glsl
    src/fx/renderWhite.glsl
    src/fx/renderReflective.glsl
)
foreach(shader ${SHADER_FILES})
    add_sokol_shader(Demo ${shader})
endforeach()
target_include_directories(Demo PRIVATE "${CMAKE_CURRENT_BINARY_DIR}") # enable including generated shader files

