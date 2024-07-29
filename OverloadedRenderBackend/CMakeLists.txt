set(PROJECT_NAME OverloadedRenderBackend)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "framework.h"
    "pch.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "Vertex.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(Source_Files__Distrib
    "OverloadedRenderBackend.cpp"
    "OverloadedRenderBackend.h"
)
source_group("Source Files\\Distrib" FILES ${Source_Files__Distrib})

set(Source_Files__Meshes__Library
    "Mesh Library.cpp"
    "Mesh Library.h"
)
source_group("Source Files\\Meshes\\Library" FILES ${Source_Files__Meshes__Library})

set(Source_Files__Meshes__Mesh_types
    "Mesh.cpp"
    "Mesh.h"
)
source_group("Source Files\\Meshes\\Mesh types" FILES ${Source_Files__Meshes__Mesh_types})

set(Source_Files__Meshes__Mesh_types__Textured
    "TexturedMesh.cpp"
    "TexturedMesh.h"
)
source_group("Source Files\\Meshes\\Mesh types\\Textured" FILES ${Source_Files__Meshes__Mesh_types__Textured})

set(Source_Files__Renderers
    "RenderBackend.cpp"
    "RenderBackend.h"
)
source_group("Source Files\\Renderers" FILES ${Source_Files__Renderers})

set(Source_Files__Shaders
    "RenderPass.cpp"
    "RenderPass.h"
    "ShaderLog.cpp"
    "ShaderLog.hpp"
    "ShaderStage.cpp"
    "ShaderStage.h"
)
source_group("Source Files\\Shaders" FILES ${Source_Files__Shaders})

set(Source_Files__Text
    "Fonts.cpp"
    "Fonts.h"
)
source_group("Source Files\\Text" FILES ${Source_Files__Text})

set(Source_Files__Texutres
    "Textures.cpp"
    "Textures.h"
)
source_group("Source Files\\Texutres" FILES ${Source_Files__Texutres})

set(Source_Files__Utility
    "../GLAD/glad.c"
    "Camera.h"
    "dllmain.cpp"
    "pch.cpp"
    "Stream.cpp"
    "Stream.h"
    "Vertex.h"
)
source_group("Source Files\\Utility" FILES ${Source_Files__Utility})

set(Source_Files__Utility__Werml
    "Wermal Reader.cpp"
    "Wermal Reader.h"
)
source_group("Source Files\\Utility\\Werml" FILES ${Source_Files__Utility__Werml})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
    ${Source_Files__Distrib}
    ${Source_Files__Meshes__Library}
    ${Source_Files__Meshes__Mesh_types}
    ${Source_Files__Meshes__Mesh_types__Textured}
    ${Source_Files__Renderers}
    ${Source_Files__Shaders}
    ${Source_Files__Text}
    ${Source_Files__Texutres}
    ${Source_Files__Utility}
    ${Source_Files__Utility__Werml}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/pch.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE OverloadedRenderBackend)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE      "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASECLANG "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE      "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASECLANG "TRUE"
    )
endif()
################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugClang>:"
            "_DEBUG;"
            "__CLANG;"
            "_CLANG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseClang>:"
            "NDEBUG;"
            "__CLANG;"
            "_CLANG"
        ">"
        "OVERLOADEDRENDERBACKEND_EXPORTS;"
        "_WINDOWS;"
        "_USRDLL;"
        "ORB_BUILD;"
        ";"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugClang>:"
            "_DEBUG;"
            "_CLANG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseClang>:"
            "NDEBUG;"
            "_CLANG"
        ">"
        "WIN32;"
        "OVERLOADEDRENDERBACKEND_EXPORTS;"
        "_WINDOWS;"
        "_USRDLL;"
        "ORB_BUILD;"
        ";"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            $<$<CONFIG:ReleaseClang>:
                /Oi;
                /Gy
            >
            /permissive-;
            /std:c++latest;
            /Gm-;
            /sdl;
            /GF-;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /wd4006;
            /EHa
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            $<$<CONFIG:ReleaseClang>:
                /Oi;
                /Gy
            >
            /permissive-;
            /std:c++latest;
            /Gm-;
            /sdl;
            /GF-;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /wd4006;
            /EHa
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(../GLAD/glad.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:ReleaseClang>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG;
            /SUBSYSTEM:WINDOWS
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:ReleaseClang>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG;
            /SUBSYSTEM:WINDOWS
        )
    endif()
endif()

################################################################################
# Post build events
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    add_custom_command_if(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMANDS
        COMMAND        $<CONFIG:Debug> xcopy "$ENV{Outdir}OverloadedRenderBackend.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND        $<CONFIG:Debug> xcopy "$ENV{Outdir}OverloadedRenderBackend.pdb" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND        $<CONFIG:Debug> xcopy "$ENV{Outdir}OverloadedRenderBackend.lib" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND        $<CONFIG:Debug> xcopy "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>OverloadedRenderBackend.h" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Inc\\" /s /r /y /q /I
        COMMAND   $<CONFIG:DebugClang> xcopy "$ENV{Outdir}OverloadedRenderBackend.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND   $<CONFIG:DebugClang> xcopy "$ENV{Outdir}OverloadedRenderBackend.pdb" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND   $<CONFIG:DebugClang> xcopy "$ENV{Outdir}OverloadedRenderBackend.lib" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND   $<CONFIG:DebugClang> xcopy "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>OverloadedRenderBackend.h" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Inc\\" /s /r /y /q /I
        COMMAND      $<CONFIG:Release> xcopy "$ENV{Outdir}OverloadedRenderBackend.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND      $<CONFIG:Release> xcopy "$ENV{Outdir}OverloadedRenderBackend.pdb" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND      $<CONFIG:Release> xcopy "$ENV{Outdir}OverloadedRenderBackend.lib" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND      $<CONFIG:Release> xcopy "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>OverloadedRenderBackend.h" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Inc\\" /s /r /y /q /I
        COMMAND $<CONFIG:ReleaseClang> xcopy "$ENV{Outdir}OverloadedRenderBackend.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND $<CONFIG:ReleaseClang> xcopy "$ENV{Outdir}OverloadedRenderBackend.pdb" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND $<CONFIG:ReleaseClang> xcopy "$ENV{Outdir}OverloadedRenderBackend.lib" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND $<CONFIG:ReleaseClang> xcopy "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>OverloadedRenderBackend.h" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Inc\\" /s /r /y /q /I
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    add_custom_command_if(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMANDS
        COMMAND        $<CONFIG:Debug> xcopy "$ENV{Outdir}OverloadedRenderBackend.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND        $<CONFIG:Debug> xcopy "$ENV{Outdir}OverloadedRenderBackend.pdb" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND        $<CONFIG:Debug> xcopy "$ENV{Outdir}OverloadedRenderBackend.lib" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND        $<CONFIG:Debug> xcopy "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>OverloadedRenderBackend.h" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Inc\\" /s /r /y /q /I
        COMMAND   $<CONFIG:DebugClang> xcopy "$ENV{Outdir}OverloadedRenderBackend.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND   $<CONFIG:DebugClang> xcopy "$ENV{Outdir}OverloadedRenderBackend.pdb" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND   $<CONFIG:DebugClang> xcopy "$ENV{Outdir}OverloadedRenderBackend.lib" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND   $<CONFIG:DebugClang> xcopy "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>OverloadedRenderBackend.h" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Inc\\" /s /r /y /q /I
        COMMAND      $<CONFIG:Release> xcopy "$ENV{Outdir}OverloadedRenderBackend.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND      $<CONFIG:Release> xcopy "$ENV{Outdir}OverloadedRenderBackend.pdb" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND      $<CONFIG:Release> xcopy "$ENV{Outdir}OverloadedRenderBackend.lib" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND      $<CONFIG:Release> xcopy "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>OverloadedRenderBackend.h" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Inc\\" /s /r /y /q /I
        COMMAND $<CONFIG:ReleaseClang> xcopy "$ENV{Outdir}OverloadedRenderBackend.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND $<CONFIG:ReleaseClang> xcopy "$ENV{Outdir}OverloadedRenderBackend.pdb" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND $<CONFIG:ReleaseClang> xcopy "$ENV{Outdir}OverloadedRenderBackend.lib" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Lib\\$<CONFIG>\\" /s /r /y /q /I
        COMMAND $<CONFIG:ReleaseClang> xcopy "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>OverloadedRenderBackend.h" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\ORB\\Inc\\" /s /r /y /q /I
    )
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "SDL2;"
        "SDL2_ttf"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "SDL2;"
        "SDL2_ttf"
    )
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")
