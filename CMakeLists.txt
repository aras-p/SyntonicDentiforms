cmake_minimum_required(VERSION 3.20)
project(SyntonicDentiforms)

if(NOT WIN32)
    message(FATAL_ERROR "Windows only for now")
endif()

add_executable(Demo WIN32
    src/animator/AnimationBunch.cpp
    src/gfx/DynamicVBManager.cpp
    src/gfx/Mesh.cpp
    src/gfx/RenderableOrderedBillboards.cpp
    src/gfx/Texture.cpp
    src/gfx/VertexFormat.cpp
    src/math/Vector3.cpp
    src/math/Vector4.cpp
    src/resource/AnimationBundle.cpp
    src/resource/MeshBundle.cpp
    src/resource/MeshSerializer.cpp
    src/utils/StringHelper.cpp

    src/demo/Anim.cpp
    src/demo/DataFiles.cpp
    src/demo/Demo.cpp
    src/demo/Effect.cpp
    src/demo/Entity.cpp
    src/demo/Glare.cpp
    src/demo/LineRenderer.cpp
    src/demo/main.cpp
    src/demo/MeshEntity.cpp
    src/demo/MusicPlayer.cpp
    src/demo/RenderCamera.cpp
    src/demo/Scene.cpp
    src/demo/SceneOut.cpp
    src/demo/SceneTeeth.cpp
    src/demo/TextureProjector.cpp
)

target_include_directories(Demo PRIVATE
    ${CMAKE_SOURCE_DIR}
)

set_property(TARGET Demo PROPERTY CXX_STANDARD 17)
set_property(TARGET Demo PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_compile_definitions(Demo PRIVATE
    WIN32
    _WINDOWS
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
    set(SHADER_OUTPUT "${CMAKE_SOURCE_DIR}/${shader_file}.h")
    if(WIN32)
        set(SHADER_BACKENDS "hlsl5")
    else()
        set(SHADER_BACKENDS "glsl430:metal_macos")
    endif()

    add_custom_command(
        OUTPUT ${SHADER_OUTPUT}
        COMMAND ${SOKOL_SHDC}
            --input ${CMAKE_SOURCE_DIR}/${shader_file}
            --output ${SHADER_OUTPUT}
            --slang ${SHADER_BACKENDS}
        DEPENDS ${CMAKE_SOURCE_DIR}/${shader_file}
        COMMENT "Compiling shader ${shader_file} with sokol-shdc"
        VERBATIM
    )
    target_sources(${target_name} PRIVATE ${SHADER_OUTPUT})
endfunction()

set(SHADER_FILES
    src/fx/billboards.glsl
    src/fx/blit.glsl
    src/fx/caster.glsl
    src/fx/compositeAdd.glsl
    src/fx/compositeAlpha.glsl
    src/fx/filterBloom.glsl
    src/fx/filterToon.glsl
    src/fx/lines.glsl
    src/fx/noshadowHi.glsl
    src/fx/overlay.glsl
    src/fx/receiverHi.glsl
    src/fx/reflective.glsl
)
foreach(shader ${SHADER_FILES})
    add_sokol_shader(Demo ${shader})
endforeach()

