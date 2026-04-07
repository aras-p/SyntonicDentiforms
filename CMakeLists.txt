cmake_minimum_required(VERSION 3.20)
project(SyntonicDentiforms CXX)

if(NOT WIN32)
    message(FATAL_ERROR "Windows only for now")
endif()

add_library(dingus STATIC
    dingus/dingus/stdafx.cpp

    dingus/dingus/animator/AnimationBunch.cpp
    dingus/dingus/animator/AnimType.cpp

    #@TODO remove
    #dingus/dingus/dxutils/D3DApplication.cpp
    #dingus/dingus/dxutils/D3DEnumeration.cpp
    #dingus/dingus/dxutils/D3DSettings.cpp
    #dingus/dingus/dxutils/D3Dutil.cpp
    #dingus/dingus/dxutils/DXUtil.cpp
    #dingus/dingus/dxutils/DXUtils.rc

    dingus/dingus/gfx/geometry/DynamicVBManager.cpp
    dingus/dingus/gfx/Mesh.cpp
    dingus/dingus/gfx/RenderableOrderedBillboards.cpp
    dingus/dingus/gfx/Texture.cpp
    dingus/dingus/gfx/VertexFormat.cpp

    dingus/dingus/math/Vector3.cpp
    dingus/dingus/math/Vector4.cpp

    dingus/dingus/renderer/RenderableMesh.cpp

    dingus/dingus/resource/AnimationBundle.cpp
    dingus/dingus/resource/MeshBundle.cpp
    dingus/dingus/resource/MeshSerializer.cpp
    dingus/dingus/resource/TextureBundle.cpp
    dingus/dingus/resource/dds_loader.cpp

    dingus/dingus/utils/Errors.cpp
    dingus/dingus/utils/StringHelper.cpp
    dingus/dingus/utils/Timer.cpp
)

target_include_directories(dingus PUBLIC
    ${CMAKE_SOURCE_DIR}/dingus
)
target_link_directories(dingus PUBLIC
)

target_compile_definitions(dingus PRIVATE
    WIN32
    _LIB
    _MBCS
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    _CRT_SECURE_NO_WARNINGS
    NOMINMAX
)

target_precompile_headers(dingus PRIVATE dingus/dingus/stdafx.h)

add_executable(Demo WIN32
    tw/src/stdafx.cpp

    tw/src/demo/Anim.cpp
    tw/src/demo/Demo.cpp
    tw/src/demo/Effect.cpp
    tw/src/demo/Entity.cpp
    tw/src/demo/Glare.cpp
    tw/src/demo/MeshEntity.cpp
    tw/src/demo/RenderCamera.cpp
    tw/src/demo/RenderContext.cpp
    tw/src/demo/Scene.cpp
    tw/src/demo/SceneOut.cpp
    tw/src/demo/SceneTeeth.cpp

    tw/src/system/main.cpp
    tw/src/system/MusicPlayer.cpp

    tw/src/util/LineRenderer.cpp
    tw/src/util/TextureProjector.cpp

    tw/src/Demo.rc
)

target_include_directories(Demo PRIVATE
    ${CMAKE_SOURCE_DIR}/dingus
    ${CMAKE_SOURCE_DIR}/tw/src
)

set_property(TARGET dingus PROPERTY CXX_STANDARD 17)
set_property(TARGET dingus PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set_property(TARGET Demo PROPERTY CXX_STANDARD 17)
set_property(TARGET Demo PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_compile_definitions(Demo PRIVATE
    WIN32
    _WINDOWS
    _MBCS
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    _CRT_SECURE_NO_WARNINGS
    NOMINMAX
)

target_precompile_headers(Demo PRIVATE tw/src/stdafx.h)

target_link_libraries(Demo PRIVATE
    dingus
    winmm
    kernel32
    user32
    gdi32
    comdlg32
    advapi32
)

set_target_properties(Demo PROPERTIES
    OUTPUT_NAME_DEBUG   "Demo_d"
    OUTPUT_NAME_RELEASE "Demo"
    RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"
    VS_DEBUGGER_WORKING_DIRECTORY    "${CMAKE_SOURCE_DIR}"
)

function(add_sokol_shader target_name shader_file)
    set(SOKOL_SHDC "${CMAKE_SOURCE_DIR}/tools/sokol-shdc")
    set(SHADER_OUTPUT "${CMAKE_SOURCE_DIR}/${shader_file}.h")
    add_custom_command(
        OUTPUT ${SHADER_OUTPUT}
        COMMAND ${SOKOL_SHDC}
            --input ${CMAKE_SOURCE_DIR}/${shader_file}
            --output ${SHADER_OUTPUT}
            --slang glsl430:hlsl5:metal_macos
        DEPENDS ${CMAKE_SOURCE_DIR}/${shader_file}
        COMMENT "Compiling shader ${shader_file} with sokol-shdc"
        VERBATIM
    )
    target_sources(${target_name} PRIVATE ${SHADER_OUTPUT})
endfunction()

set(SHADER_FILES
    tw/src/fx/caster.glsl
    tw/src/fx/receiverHi.glsl
    tw/src/fx/receiverLo.glsl
    tw/src/fx/reflective.glsl
)
foreach(shader ${SHADER_FILES})
    add_sokol_shader(Demo ${shader})
endforeach()

