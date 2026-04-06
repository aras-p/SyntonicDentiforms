# Microsoft Developer Studio Project File - Name="Demo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Demo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Demo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Demo.mak" CFG="Demo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Demo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Demo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Demo - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "build/release"
# PROP Intermediate_Dir "build/release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../dingus" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x427 /d "NDEBUG"
# ADD RSC /l 0x427 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib d3d9.lib d3dx9.lib winmm.lib src/fmodvc.lib /nologo /subsystem:windows /machine:I386 /out:"Demo.exe"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "Demo - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "build/debug"
# PROP Intermediate_Dir "build/debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../dingus" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x427 /d "_DEBUG"
# ADD RSC /l 0x427 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib d3d9.lib d3dx9dt.lib winmm.lib src/fmodvc.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Demo_d.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Demo - Win32 Release"
# Name "Demo - Win32 Debug"
# Begin Group "demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\demo\Anim.cpp
# End Source File
# Begin Source File

SOURCE=.\src\demo\Anim.h
# End Source File
# Begin Source File

SOURCE=.\src\demo\Demo.cpp
# End Source File
# Begin Source File

SOURCE=.\src\demo\Demo.h
# End Source File
# Begin Source File

SOURCE=.\src\demo\DemoResources.h
# End Source File
# Begin Source File

SOURCE=.\src\demo\Entity.cpp
# End Source File
# Begin Source File

SOURCE=.\src\demo\Entity.h
# End Source File
# Begin Source File

SOURCE=.\src\demo\Glare.cpp
# End Source File
# Begin Source File

SOURCE=.\src\demo\Glare.h
# End Source File
# Begin Source File

SOURCE=.\src\demo\MeshEntity.cpp
# End Source File
# Begin Source File

SOURCE=.\src\demo\MeshEntity.h
# End Source File
# Begin Source File

SOURCE=.\src\demo\Scene.cpp
# End Source File
# Begin Source File

SOURCE=.\src\demo\Scene.h
# End Source File
# Begin Source File

SOURCE=.\src\demo\SceneOut.cpp
# End Source File
# Begin Source File

SOURCE=.\src\demo\SceneOut.h
# End Source File
# Begin Source File

SOURCE=.\src\demo\SceneTeeth.cpp
# End Source File
# Begin Source File

SOURCE=.\src\demo\SceneTeeth.h
# End Source File
# End Group
# Begin Group "system"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\system\Globals.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\Globals.h
# End Source File
# Begin Source File

SOURCE=.\src\system\main.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\MusicPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\MusicPlayer.h
# End Source File
# Begin Source File

SOURCE=.\src\system\System.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\System.h
# End Source File
# End Group
# Begin Group "util"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\util\LineRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\util\LineRenderer.h
# End Source File
# Begin Source File

SOURCE=.\src\util\TextureProjector.cpp
# End Source File
# Begin Source File

SOURCE=.\src\util\TextureProjector.h
# End Source File
# End Group
# Begin Group "fx"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\data\fx\_defines.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\_global.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\_lib.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\_shared.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\billboards.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\billboardsNoDestA.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\caster.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\compositeAdd.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\compositeAlpha.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\filterBloom.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\filterToon.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\lines.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\noshadowHi.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\overlay.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\overlay2.fx
# End Source File
# Begin Source File

SOURCE=".\data\fx\receiver-ps11path.fx"
# End Source File
# Begin Source File

SOURCE=".\data\fx\receiver-ps14path.fx"
# End Source File
# Begin Source File

SOURCE=.\data\fx\receiverHi.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\receiverLo.fx
# End Source File
# Begin Source File

SOURCE=".\data\fx\recvall-ps11path.fx"
# End Source File
# Begin Source File

SOURCE=".\data\fx\recvall-ps14path.fx"
# End Source File
# Begin Source File

SOURCE=.\data\fx\recvallHi.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\recvallLo.fx
# End Source File
# Begin Source File

SOURCE=.\data\fx\reflective.fx
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\src\Demo.rc
# End Source File
# Begin Source File

SOURCE=.\src\DirectX.ico
# End Source File
# Begin Source File

SOURCE=.\src\fmod.h
# End Source File
# Begin Source File

SOURCE=.\src\logo.bmp
# End Source File
# Begin Source File

SOURCE=.\src\resource.h
# End Source File
# Begin Source File

SOURCE=.\src\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\src\stdafx.h
# End Source File
# End Target
# End Project
