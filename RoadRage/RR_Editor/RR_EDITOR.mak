# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=RR_EDITOR - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to RR_EDITOR - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "RR_EDITOR - Win32 Release" && "$(CFG)" !=\
 "RR_EDITOR - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "RR_EDITOR.mak" CFG="RR_EDITOR - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RR_EDITOR - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "RR_EDITOR - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "RR_EDITOR - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"

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
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\RR_EDITOR.exe"

CLEAN : 
	-@erase "$(INTDIR)\bitmap.obj"
	-@erase "$(INTDIR)\d3dapp.obj"
	-@erase "$(INTDIR)\d3dcalls.obj"
	-@erase "$(INTDIR)\d3dmain.obj"
	-@erase "$(INTDIR)\d3dmain.res"
	-@erase "$(INTDIR)\ddcalls.obj"
	-@erase "$(INTDIR)\ddutil.obj"
	-@erase "$(INTDIR)\dsutil.obj"
	-@erase "$(INTDIR)\editor.obj"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\lclib.obj"
	-@erase "$(INTDIR)\matrix.obj"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\Models.obj"
	-@erase "$(INTDIR)\RoadRage.obj"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\stats.obj"
	-@erase "$(INTDIR)\texture.obj"
	-@erase "$(INTDIR)\world.obj"
	-@erase "$(OUTDIR)\RR_EDITOR.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/RR_EDITOR.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
RSC_PROJ=/l 0x809 /fo"$(INTDIR)/d3dmain.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/RR_EDITOR.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 dxguid.lib ddraw.lib dsound.lib dinput.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=dxguid.lib ddraw.lib dsound.lib dinput.lib kernel32.lib user32.lib\
 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib\
 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/RR_EDITOR.pdb" /machine:I386\
 /out:"$(OUTDIR)/RR_EDITOR.exe" 
LINK32_OBJS= \
	"$(INTDIR)\bitmap.obj" \
	"$(INTDIR)\d3dapp.obj" \
	"$(INTDIR)\d3dcalls.obj" \
	"$(INTDIR)\d3dmain.obj" \
	"$(INTDIR)\d3dmain.res" \
	"$(INTDIR)\ddcalls.obj" \
	"$(INTDIR)\ddutil.obj" \
	"$(INTDIR)\dsutil.obj" \
	"$(INTDIR)\editor.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\lclib.obj" \
	"$(INTDIR)\matrix.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\Models.obj" \
	"$(INTDIR)\RoadRage.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\stats.obj" \
	"$(INTDIR)\texture.obj" \
	"$(INTDIR)\world.obj"

"$(OUTDIR)\RR_EDITOR.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\RR_EDITOR.exe" "$(OUTDIR)\RR_EDITOR.bsc"

CLEAN : 
	-@erase "$(INTDIR)\bitmap.obj"
	-@erase "$(INTDIR)\bitmap.sbr"
	-@erase "$(INTDIR)\d3dapp.obj"
	-@erase "$(INTDIR)\d3dapp.sbr"
	-@erase "$(INTDIR)\d3dcalls.obj"
	-@erase "$(INTDIR)\d3dcalls.sbr"
	-@erase "$(INTDIR)\d3dmain.obj"
	-@erase "$(INTDIR)\d3dmain.res"
	-@erase "$(INTDIR)\d3dmain.sbr"
	-@erase "$(INTDIR)\ddcalls.obj"
	-@erase "$(INTDIR)\ddcalls.sbr"
	-@erase "$(INTDIR)\ddutil.obj"
	-@erase "$(INTDIR)\ddutil.sbr"
	-@erase "$(INTDIR)\dsutil.obj"
	-@erase "$(INTDIR)\dsutil.sbr"
	-@erase "$(INTDIR)\editor.obj"
	-@erase "$(INTDIR)\editor.sbr"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\input.sbr"
	-@erase "$(INTDIR)\lclib.obj"
	-@erase "$(INTDIR)\lclib.sbr"
	-@erase "$(INTDIR)\matrix.obj"
	-@erase "$(INTDIR)\matrix.sbr"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\misc.sbr"
	-@erase "$(INTDIR)\Models.obj"
	-@erase "$(INTDIR)\Models.sbr"
	-@erase "$(INTDIR)\RoadRage.obj"
	-@erase "$(INTDIR)\RoadRage.sbr"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\sound.sbr"
	-@erase "$(INTDIR)\stats.obj"
	-@erase "$(INTDIR)\stats.sbr"
	-@erase "$(INTDIR)\texture.obj"
	-@erase "$(INTDIR)\texture.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\world.obj"
	-@erase "$(INTDIR)\world.sbr"
	-@erase "$(OUTDIR)\RR_EDITOR.bsc"
	-@erase "$(OUTDIR)\RR_EDITOR.exe"
	-@erase "$(OUTDIR)\RR_EDITOR.ilk"
	-@erase "$(OUTDIR)\RR_EDITOR.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR"$(INTDIR)/" /Fp"$(INTDIR)/RR_EDITOR.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
RSC_PROJ=/l 0x809 /fo"$(INTDIR)/d3dmain.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/RR_EDITOR.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\bitmap.sbr" \
	"$(INTDIR)\d3dapp.sbr" \
	"$(INTDIR)\d3dcalls.sbr" \
	"$(INTDIR)\d3dmain.sbr" \
	"$(INTDIR)\ddcalls.sbr" \
	"$(INTDIR)\ddutil.sbr" \
	"$(INTDIR)\dsutil.sbr" \
	"$(INTDIR)\editor.sbr" \
	"$(INTDIR)\input.sbr" \
	"$(INTDIR)\lclib.sbr" \
	"$(INTDIR)\matrix.sbr" \
	"$(INTDIR)\misc.sbr" \
	"$(INTDIR)\Models.sbr" \
	"$(INTDIR)\RoadRage.sbr" \
	"$(INTDIR)\sound.sbr" \
	"$(INTDIR)\stats.sbr" \
	"$(INTDIR)\texture.sbr" \
	"$(INTDIR)\world.sbr"

"$(OUTDIR)\RR_EDITOR.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 dxguid.lib ddraw.lib dsound.lib dinput.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=dxguid.lib ddraw.lib dsound.lib dinput.lib kernel32.lib user32.lib\
 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib\
 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/RR_EDITOR.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/RR_EDITOR.exe" 
LINK32_OBJS= \
	"$(INTDIR)\bitmap.obj" \
	"$(INTDIR)\d3dapp.obj" \
	"$(INTDIR)\d3dcalls.obj" \
	"$(INTDIR)\d3dmain.obj" \
	"$(INTDIR)\d3dmain.res" \
	"$(INTDIR)\ddcalls.obj" \
	"$(INTDIR)\ddutil.obj" \
	"$(INTDIR)\dsutil.obj" \
	"$(INTDIR)\editor.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\lclib.obj" \
	"$(INTDIR)\matrix.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\Models.obj" \
	"$(INTDIR)\RoadRage.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\stats.obj" \
	"$(INTDIR)\texture.obj" \
	"$(INTDIR)\world.obj"

"$(OUTDIR)\RR_EDITOR.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "RR_EDITOR - Win32 Release"
# Name "RR_EDITOR - Win32 Debug"

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"

!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\world.cpp
DEP_CPP_WORLD=\
	"..\..\..\..\dx6sdk\include\d3dcaps.h"\
	"..\..\..\..\dx6sdk\include\d3dtypes.h"\
	"..\..\..\..\dx6sdk\include\d3dvec.inl"\
	".\3Dengine.h"\
	".\d3dapp.h"\
	".\d3ddemo.h"\
	".\d3dmacs.h"\
	".\d3dmain.h"\
	".\d3dmath.h"\
	".\d3dres.h"\
	".\d3dsphr.h"\
	".\models.h"\
	".\texture.h"\
	".\world.h"\
	{$(INCLUDE)}"\d3d.h"\
	

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"


"$(INTDIR)\world.obj" : $(SOURCE) $(DEP_CPP_WORLD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"


"$(INTDIR)\world.obj" : $(SOURCE) $(DEP_CPP_WORLD) "$(INTDIR)"

"$(INTDIR)\world.sbr" : $(SOURCE) $(DEP_CPP_WORLD) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\d3dapp.c
DEP_CPP_D3DAP=\
	"..\..\..\..\dx6sdk\include\d3dcaps.h"\
	"..\..\..\..\dx6sdk\include\d3dtypes.h"\
	"..\..\..\..\dx6sdk\include\d3dvec.inl"\
	".\d3dapp.h"\
	".\d3dappi.h"\
	".\d3dmacs.h"\
	".\lclib.h"\
	{$(INCLUDE)}"\d3d.h"\
	

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"


"$(INTDIR)\d3dapp.obj" : $(SOURCE) $(DEP_CPP_D3DAP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"


"$(INTDIR)\d3dapp.obj" : $(SOURCE) $(DEP_CPP_D3DAP) "$(INTDIR)"

"$(INTDIR)\d3dapp.sbr" : $(SOURCE) $(DEP_CPP_D3DAP) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\d3dcalls.c
DEP_CPP_D3DCA=\
	"..\..\..\..\dx6sdk\include\d3dcaps.h"\
	"..\..\..\..\dx6sdk\include\d3dtypes.h"\
	"..\..\..\..\dx6sdk\include\d3dvec.inl"\
	".\d3dapp.h"\
	".\d3dappi.h"\
	".\d3dmacs.h"\
	".\lclib.h"\
	{$(INCLUDE)}"\d3d.h"\
	

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"


"$(INTDIR)\d3dcalls.obj" : $(SOURCE) $(DEP_CPP_D3DCA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"


"$(INTDIR)\d3dcalls.obj" : $(SOURCE) $(DEP_CPP_D3DCA) "$(INTDIR)"

"$(INTDIR)\d3dcalls.sbr" : $(SOURCE) $(DEP_CPP_D3DCA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\d3dmain.cpp
DEP_CPP_D3DMA=\
	"..\..\..\..\dx6sdk\include\d3dcaps.h"\
	"..\..\..\..\dx6sdk\include\d3dtypes.h"\
	"..\..\..\..\dx6sdk\include\d3dvec.inl"\
	".\3Dengine.h"\
	".\bitmap.h"\
	".\d3dapp.h"\
	".\d3dappi.h"\
	".\d3ddemo.h"\
	".\d3dmacs.h"\
	".\d3dmain.h"\
	".\d3dmath.h"\
	".\d3dres.h"\
	".\d3dsphr.h"\
	".\editor.h"\
	".\input.h"\
	".\lclib.h"\
	".\sound.h"\
	".\texture.h"\
	".\toolbar.h"\
	".\world.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\dinput.h"\
	

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"


"$(INTDIR)\d3dmain.obj" : $(SOURCE) $(DEP_CPP_D3DMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"


"$(INTDIR)\d3dmain.obj" : $(SOURCE) $(DEP_CPP_D3DMA) "$(INTDIR)"

"$(INTDIR)\d3dmain.sbr" : $(SOURCE) $(DEP_CPP_D3DMA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ddcalls.c
DEP_CPP_DDCAL=\
	"..\..\..\..\dx6sdk\include\d3dcaps.h"\
	"..\..\..\..\dx6sdk\include\d3dtypes.h"\
	"..\..\..\..\dx6sdk\include\d3dvec.inl"\
	".\d3dapp.h"\
	".\d3dappi.h"\
	".\d3dmacs.h"\
	".\lclib.h"\
	{$(INCLUDE)}"\d3d.h"\
	

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"


"$(INTDIR)\ddcalls.obj" : $(SOURCE) $(DEP_CPP_DDCAL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"


"$(INTDIR)\ddcalls.obj" : $(SOURCE) $(DEP_CPP_DDCAL) "$(INTDIR)"

"$(INTDIR)\ddcalls.sbr" : $(SOURCE) $(DEP_CPP_DDCAL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ddutil.cpp
DEP_CPP_DDUTI=\
	".\ddutil.h"\
	

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"


"$(INTDIR)\ddutil.obj" : $(SOURCE) $(DEP_CPP_DDUTI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"


"$(INTDIR)\ddutil.obj" : $(SOURCE) $(DEP_CPP_DDUTI) "$(INTDIR)"

"$(INTDIR)\ddutil.sbr" : $(SOURCE) $(DEP_CPP_DDUTI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dsutil.c
DEP_CPP_DSUTI=\
	".\dsutil.h"\
	

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"


"$(INTDIR)\dsutil.obj" : $(SOURCE) $(DEP_CPP_DSUTI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"


"$(INTDIR)\dsutil.obj" : $(SOURCE) $(DEP_CPP_DSUTI) "$(INTDIR)"

"$(INTDIR)\dsutil.sbr" : $(SOURCE) $(DEP_CPP_DSUTI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editor.c
DEP_CPP_EDITO=\
	"..\..\..\..\dx6sdk\include\d3dcaps.h"\
	"..\..\..\..\dx6sdk\include\d3dtypes.h"\
	"..\..\..\..\dx6sdk\include\d3dvec.inl"\
	".\3Dengine.h"\
	".\d3dapp.h"\
	".\d3ddemo.h"\
	".\d3dmacs.h"\
	".\d3dmath.h"\
	".\d3dsphr.h"\
	".\editor.h"\
	".\toolbar.h"\
	".\world.h"\
	{$(INCLUDE)}"\d3d.h"\
	

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"


"$(INTDIR)\editor.obj" : $(SOURCE) $(DEP_CPP_EDITO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"


"$(INTDIR)\editor.obj" : $(SOURCE) $(DEP_CPP_EDITO) "$(INTDIR)"

"$(INTDIR)\editor.sbr" : $(SOURCE) $(DEP_CPP_EDITO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\input.c
DEP_CPP_INPUT=\
	".\input.h"\
	{$(INCLUDE)}"\dinput.h"\
	

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"


"$(INTDIR)\input.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"


"$(INTDIR)\input.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"

"$(INTDIR)\input.sbr" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\lclib.c
DEP_CPP_LCLIB=\
	".\lclib.h"\
	

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"


"$(INTDIR)\lclib.obj" : $(SOURCE) $(DEP_CPP_LCLIB) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"


"$(INTDIR)\lclib.obj" : $(SOURCE) $(DEP_CPP_LCLIB) "$(INTDIR)"

"$(INTDIR)\lclib.sbr" : $(SOURCE) $(DEP_CPP_LCLIB) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\matrix.cpp
DEP_CPP_MATRI=\
	"..\..\..\..\dx6sdk\include\d3dcaps.h"\
	"..\..\..\..\dx6sdk\include\d3dtypes.h"\
	"..\..\..\..\dx6sdk\include\d3dvec.inl"\
	".\3Dengine.h"\
	".\d3dapp.h"\
	".\d3ddemo.h"\
	".\d3dmacs.h"\
	".\d3dmath.h"\
	".\d3dsphr.h"\
	".\matrix.h"\
	".\world.h"\
	{$(INCLUDE)}"\d3d.h"\
	

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"


"$(INTDIR)\matrix.obj" : $(SOURCE) $(DEP_CPP_MATRI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"


"$(INTDIR)\matrix.obj" : $(SOURCE) $(DEP_CPP_MATRI) "$(INTDIR)"

"$(INTDIR)\matrix.sbr" : $(SOURCE) $(DEP_CPP_MATRI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc.c
DEP_CPP_MISC_=\
	"..\..\..\..\dx6sdk\include\d3dcaps.h"\
	"..\..\..\..\dx6sdk\include\d3dtypes.h"\
	"..\..\..\..\dx6sdk\include\d3dvec.inl"\
	".\d3dapp.h"\
	".\d3dappi.h"\
	".\d3dmacs.h"\
	".\lclib.h"\
	{$(INCLUDE)}"\d3d.h"\
	

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"


"$(INTDIR)\misc.obj" : $(SOURCE) $(DEP_CPP_MISC_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"


"$(INTDIR)\misc.obj" : $(SOURCE) $(DEP_CPP_MISC_) "$(INTDIR)"

"$(INTDIR)\misc.sbr" : $(SOURCE) $(DEP_CPP_MISC_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sound.c
DEP_CPP_SOUND=\
	".\dsutil.h"\
	".\sound.h"\
	

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"


"$(INTDIR)\sound.obj" : $(SOURCE) $(DEP_CPP_SOUND) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"


"$(INTDIR)\sound.obj" : $(SOURCE) $(DEP_CPP_SOUND) "$(INTDIR)"

"$(INTDIR)\sound.sbr" : $(SOURCE) $(DEP_CPP_SOUND) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stats.cpp
DEP_CPP_STATS=\
	"..\..\..\..\dx6sdk\include\d3dcaps.h"\
	"..\..\..\..\dx6sdk\include\d3dtypes.h"\
	"..\..\..\..\dx6sdk\include\d3dvec.inl"\
	".\3Dengine.h"\
	".\d3dapp.h"\
	".\d3ddemo.h"\
	".\d3dmacs.h"\
	".\d3dmain.h"\
	".\d3dmath.h"\
	".\d3dres.h"\
	".\d3dsphr.h"\
	".\world.h"\
	{$(INCLUDE)}"\d3d.h"\
	

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"


"$(INTDIR)\stats.obj" : $(SOURCE) $(DEP_CPP_STATS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"


"$(INTDIR)\stats.obj" : $(SOURCE) $(DEP_CPP_STATS) "$(INTDIR)"

"$(INTDIR)\stats.sbr" : $(SOURCE) $(DEP_CPP_STATS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\texture.c
DEP_CPP_TEXTU=\
	"..\..\..\..\dx6sdk\include\d3dcaps.h"\
	"..\..\..\..\dx6sdk\include\d3dtypes.h"\
	"..\..\..\..\dx6sdk\include\d3dvec.inl"\
	".\d3dapp.h"\
	".\d3dappi.h"\
	".\d3dmacs.h"\
	".\lclib.h"\
	".\texture.h"\
	".\world.h"\
	{$(INCLUDE)}"\d3d.h"\
	

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"


"$(INTDIR)\texture.obj" : $(SOURCE) $(DEP_CPP_TEXTU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"


"$(INTDIR)\texture.obj" : $(SOURCE) $(DEP_CPP_TEXTU) "$(INTDIR)"

"$(INTDIR)\texture.sbr" : $(SOURCE) $(DEP_CPP_TEXTU) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\bitmap.cpp
DEP_CPP_BITMA=\
	"..\..\..\..\dx6sdk\include\d3dcaps.h"\
	"..\..\..\..\dx6sdk\include\d3dtypes.h"\
	"..\..\..\..\dx6sdk\include\d3dvec.inl"\
	".\3Dengine.h"\
	".\bitmap.h"\
	".\d3dapp.h"\
	".\d3ddemo.h"\
	".\d3dmacs.h"\
	".\d3dmain.h"\
	".\d3dmath.h"\
	".\d3dres.h"\
	".\d3dsphr.h"\
	".\world.h"\
	{$(INCLUDE)}"\d3d.h"\
	

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"


"$(INTDIR)\bitmap.obj" : $(SOURCE) $(DEP_CPP_BITMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"


"$(INTDIR)\bitmap.obj" : $(SOURCE) $(DEP_CPP_BITMA) "$(INTDIR)"

"$(INTDIR)\bitmap.sbr" : $(SOURCE) $(DEP_CPP_BITMA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\d3dmain.rc
DEP_RSC_D3DMAI=\
	".\bitmap1.bmp"\
	".\colt.wav"\
	".\d3d.ico"\
	".\d3dres.h"\
	".\engine.wav"\
	".\skid.wav"\
	

"$(INTDIR)\d3dmain.res" : $(SOURCE) $(DEP_RSC_D3DMAI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Models.cpp
DEP_CPP_MODEL=\
	"..\..\..\..\dx6sdk\include\d3dcaps.h"\
	"..\..\..\..\dx6sdk\include\d3dtypes.h"\
	"..\..\..\..\dx6sdk\include\d3dvec.inl"\
	".\3Dengine.h"\
	".\d3dapp.h"\
	".\d3ddemo.h"\
	".\d3dmacs.h"\
	".\d3dmain.h"\
	".\d3dmath.h"\
	".\d3dres.h"\
	".\d3dsphr.h"\
	".\models.h"\
	".\texture.h"\
	".\world.h"\
	{$(INCLUDE)}"\d3d.h"\
	

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"


"$(INTDIR)\Models.obj" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"


"$(INTDIR)\Models.obj" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"

"$(INTDIR)\Models.sbr" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\RoadRage.cpp
DEP_CPP_ROADR=\
	"..\..\..\..\dx6sdk\include\d3dcaps.h"\
	"..\..\..\..\dx6sdk\include\d3dtypes.h"\
	"..\..\..\..\dx6sdk\include\d3dvec.inl"\
	".\3Dengine.h"\
	".\d3dapp.h"\
	".\d3ddemo.h"\
	".\d3dmacs.h"\
	".\d3dmain.h"\
	".\d3dmath.h"\
	".\d3dres.h"\
	".\d3dsphr.h"\
	".\matrix.h"\
	".\world.h"\
	{$(INCLUDE)}"\d3d.h"\
	

!IF  "$(CFG)" == "RR_EDITOR - Win32 Release"


"$(INTDIR)\RoadRage.obj" : $(SOURCE) $(DEP_CPP_ROADR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RR_EDITOR - Win32 Debug"


"$(INTDIR)\RoadRage.obj" : $(SOURCE) $(DEP_CPP_ROADR) "$(INTDIR)"

"$(INTDIR)\RoadRage.sbr" : $(SOURCE) $(DEP_CPP_ROADR) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
