cmake_minimum_required(VERSION 3.20)
project(SyntonicDentiforms CXX)

if(NOT WIN32)
    message(FATAL_ERROR "Windows only for now")
endif()

# for now assume legacy DX SDK is installed here locally
set(DXSDK_DIR "${CMAKE_SOURCE_DIR}/dxsdk" CACHE PATH "DirectX SDK root directory")

add_library(dingus STATIC
    dingus/dingus/stdafx.cpp

    dingus/dingus/animator/AnimationBunch.cpp
    dingus/dingus/animator/AnimType.cpp

    dingus/dingus/console/Console.cpp
    dingus/dingus/console/D3DConsoleRenderingContext.cpp
    dingus/dingus/console/FileConsoleRenderingContext.cpp
    dingus/dingus/console/W32StdConsoleRenderingContext.cpp
    dingus/dingus/console/WDebugConsoleRenderingContext.cpp

    dingus/dingus/dxutils/D3DApplication.cpp
    dingus/dingus/dxutils/D3DEnumeration.cpp
    dingus/dingus/dxutils/D3DSettings.cpp
    dingus/dingus/dxutils/D3Dutil.cpp
    dingus/dingus/dxutils/DXUtil.cpp
    dingus/dingus/dxutils/DXUtils.rc

    dingus/dingus/gfx/geometry/DynamicVBManager.cpp
    dingus/dingus/gfx/geometry/VBChunk.cpp
    dingus/dingus/gfx/geometry/VBChunkHelper.cpp
    dingus/dingus/gfx/Mesh.cpp
    dingus/dingus/gfx/RenderableBillboards.cpp
    dingus/dingus/gfx/RenderableOrderedBillboards.cpp
    dingus/dingus/gfx/VertexFormat.cpp

    dingus/dingus/kernel/D3DDevice.cpp
    dingus/dingus/kernel/ProxyEffect.cpp
    dingus/dingus/kernel/SystemClock.cpp

    dingus/dingus/math/Transform.cpp
    dingus/dingus/math/Triangle.cpp
    dingus/dingus/math/Vector3.cpp

    dingus/dingus/renderer/EffectParams.cpp
    dingus/dingus/renderer/EffectParamsNotifier.cpp
    dingus/dingus/renderer/EffectStateManager.cpp
    dingus/dingus/renderer/Renderable.cpp
    dingus/dingus/renderer/RenderableBuffer.cpp
    dingus/dingus/renderer/RenderableMesh.cpp
    dingus/dingus/renderer/RenderCamera.cpp
    dingus/dingus/renderer/RenderContext.cpp
    dingus/dingus/renderer/RenderStats.cpp

    dingus/dingus/resource/AnimationBundle.cpp
    dingus/dingus/resource/CubeTextureBundle.cpp
    dingus/dingus/resource/EffectBundle.cpp
    dingus/dingus/resource/EffectInitListeners.cpp
    dingus/dingus/resource/IndexBufferBundle.cpp
    dingus/dingus/resource/MeshBundle.cpp
    dingus/dingus/resource/MeshSerializer.cpp
    dingus/dingus/resource/ResourceBundle.cpp
    dingus/dingus/resource/SharedSurfaceBundle.cpp
    dingus/dingus/resource/SharedTextureBundle.cpp
    dingus/dingus/resource/SurfaceCreator.cpp
    dingus/dingus/resource/TextureBundle.cpp
    dingus/dingus/resource/TextureCreator.cpp
    dingus/dingus/resource/VertexDeclBundle.cpp

    dingus/dingus/utils/Errors.cpp
    dingus/dingus/utils/MemoryPool.cpp
    dingus/dingus/utils/mmgr.cpp
    dingus/dingus/utils/StringHelper.cpp
    dingus/dingus/utils/Timer.cpp
)

target_include_directories(dingus PUBLIC
    ${CMAKE_SOURCE_DIR}/dingus
    ${DXSDK_DIR}/Include
)
target_link_directories(dingus PUBLIC
    $<$<CONFIG:Debug>: ${DXSDK_DIR}/debug/lib/x86>
    $<$<CONFIG:Release>: ${DXSDK_DIR}/release/lib/x86>
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

target_compile_options(dingus PRIVATE
    /W3
    /EHsc
    $<$<CONFIG:Debug>:  /MDd /Od /Zi /RTC1>
    $<$<CONFIG:Release>:/MD /O2 /Zi>
)

target_precompile_headers(dingus PRIVATE dingus/dingus/stdafx.h)

add_executable(Demo WIN32
    tw/src/stdafx.cpp

    tw/src/demo/Anim.cpp
    tw/src/demo/Demo.cpp
    tw/src/demo/Entity.cpp
    tw/src/demo/Glare.cpp
    tw/src/demo/MeshEntity.cpp
    tw/src/demo/Scene.cpp
    tw/src/demo/SceneOut.cpp
    tw/src/demo/SceneTeeth.cpp

    tw/src/system/Globals.cpp
    tw/src/system/main.cpp
    tw/src/system/MusicPlayer.cpp
    tw/src/system/System.cpp

    tw/src/util/LineRenderer.cpp
    tw/src/util/TextureProjector.cpp

    tw/src/Demo.rc
)

target_include_directories(Demo PRIVATE
    ${CMAKE_SOURCE_DIR}/dingus
    ${CMAKE_SOURCE_DIR}/tw/src
    ${DXSDK_DIR}/Include
)

target_link_directories(Demo PRIVATE ${DXSDK_DIR}/Lib/x86)

target_compile_definitions(Demo PRIVATE
    WIN32
    _WINDOWS
    _MBCS
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    _CRT_SECURE_NO_WARNINGS
    NOMINMAX
)

target_compile_options(Demo PRIVATE
    /W3
    /EHsc
    $<$<CONFIG:Debug>:  /MDd /Od /Zi /RTC1>
    $<$<CONFIG:Release>:/MD /O2 /Zi>
)

target_precompile_headers(Demo PRIVATE tw/src/stdafx.h)

target_link_libraries(Demo PRIVATE
    dingus
    d3d9
    $<$<CONFIG:Debug>:d3dx9d>
    $<$<CONFIG:Release>:d3dx9>
    winmm
    kernel32
    user32
    gdi32
    comdlg32
    advapi32
)

set_target_properties(Demo PROPERTIES
    OUTPUT_NAME_DEBUG "Demo_d"
    OUTPUT_NAME_RELEASE "Demo"
)
