# Microsoft Developer Studio Project File - Name="chap13" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=chap13 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "chap13.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "chap13.mak" CFG="chap13 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "chap13 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "chap13 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "chap13 - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "chap13 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ddraw.lib dxguid.lib dinput.lib dplayx.lib winmm.lib comctl32.lib dsound.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "chap13 - Win32 Release"
# Name "chap13 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Controls.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dapp.cpp
# End Source File
# Begin Source File

SOURCE=.\d3denum.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dframe.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dmath.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dtextr.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dutil.cpp
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

SOURCE=.\LoadWorld.cpp
# End Source File
# Begin Source File

SOURCE=.\playsound.cpp
# End Source File
# Begin Source File

SOURCE=.\RoadRage.cpp
# End Source File
# Begin Source File

SOURCE=.\wavread.cpp
# End Source File
# Begin Source File

SOURCE=.\winmain.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\3Dengine.hpp
# End Source File
# Begin Source File

SOURCE=.\bitmap.hpp
# End Source File
# Begin Source File

SOURCE=.\Controls.hpp
# End Source File
# Begin Source File

SOURCE=.\d3dapp.h
# End Source File
# Begin Source File

SOURCE=.\d3denum.h
# End Source File
# Begin Source File

SOURCE=.\d3dframe.h
# End Source File
# Begin Source File

SOURCE=.\d3dmath.h
# End Source File
# Begin Source File

SOURCE=.\d3dres.h
# End Source File
# Begin Source File

SOURCE=.\d3dtextr.h
# End Source File
# Begin Source File

SOURCE=.\d3dutil.h
# End Source File
# Begin Source File

SOURCE=.\Import3DS.hpp
# End Source File
# Begin Source File

SOURCE=.\ImportMD2.h
# End Source File
# Begin Source File

SOURCE=.\ImportMD2.hpp
# End Source File
# Begin Source File

SOURCE=.\info.hpp
# End Source File
# Begin Source File

SOURCE=.\LoadWorld.h
# End Source File
# Begin Source File

SOURCE=.\LoadWorld.hpp
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\RoadRage.hpp
# End Source File
# Begin Source File

SOURCE=.\wavread.h
# End Source File
# Begin Source File

SOURCE=.\world.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\directx.ico
# End Source File
# End Group
# End Target
# End Project
