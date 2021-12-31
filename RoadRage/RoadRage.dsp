# Microsoft Developer Studio Project File - Name="rb11DP" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=rb11DP - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "rb11DP.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rb11DP.mak" CFG="rb11DP - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rb11DP - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "rb11DP - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "rb11DP - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "rb11DP - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "D:\RoadRage\"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ddraw.lib dplayx.lib winmm.lib comctl32.lib dsound.lib dinput.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /profile /debug /machine:I386 /out:"D:\RoadRage\RoadRage.exe"

!ENDIF 

# Begin Target

# Name "rb11DP - Win32 Release"
# Name "rb11DP - Win32 Debug"
# Begin Source File

SOURCE=.\bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\comm.cpp
# End Source File
# Begin Source File

SOURCE=.\comm.h
# End Source File
# Begin Source File

SOURCE=.\Controls.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d.ico
# End Source File
# Begin Source File

SOURCE=.\d3dapp.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dcalls.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dmain.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dres.h
# End Source File
# Begin Source File

SOURCE=.\ddcalls.cpp
# End Source File
# Begin Source File

SOURCE=.\ddutil.cpp
# End Source File
# Begin Source File

SOURCE=.\dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\dpchat.cpp
# End Source File
# Begin Source File

SOURCE=.\dpchat.h
# End Source File
# Begin Source File

SOURCE=.\dsutil.cpp
# End Source File
# Begin Source File

SOURCE=.\duel.h
# End Source File
# Begin Source File

SOURCE=.\engine.wav
# End Source File
# Begin Source File

SOURCE=.\gameproc.h
# End Source File
# Begin Source File

SOURCE=.\Import3DS.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportMD2.cpp
# End Source File
# Begin Source File

SOURCE=.\info.cpp
# End Source File
# Begin Source File

SOURCE=.\input.cpp
# End Source File
# Begin Source File

SOURCE=.\lclib.cpp
# End Source File
# Begin Source File

SOURCE=.\lobby.cpp
# End Source File
# Begin Source File

SOURCE=.\lobby.h
# End Source File
# Begin Source File

SOURCE=.\m4fire.wav
# End Source File
# Begin Source File

SOURCE=.\matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\misc.cpp
# End Source File
# Begin Source File

SOURCE=.\mp5fire.bin
# End Source File
# Begin Source File

SOURCE=.\mp5fire.wav
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\RoadRage.cpp
# End Source File
# Begin Source File

SOURCE=.\skid.wav
# End Source File
# Begin Source File

SOURCE=.\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\stats.cpp
# End Source File
# Begin Source File

SOURCE=.\texture.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=.\RR11dp\winmain.rc
# End Source File
# Begin Source File

SOURCE=.\wizard.h
# End Source File
# Begin Source File

SOURCE=.\world.cpp
# End Source File
# End Target
# End Project
