# Microsoft Developer Studio Generated NMAKE File, Based on TeXLive.dsp
!IF "$(CFG)" == ""
CFG=TeXLive - Win32 Debug
!MESSAGE No configuration specified. Defaulting to TeXLive - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "TeXLive - Win32 Release" && "$(CFG)" != "TeXLive - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TeXLive.mak" CFG="TeXLive - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TeXLive - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "TeXLive - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TeXLive - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : ".\hlp\TeXLive.hm" ".\Release\TeXLive.hlp" ".\Release\TeXLive.cnt" "$(OUTDIR)\TeXLive.exe" "$(OUTDIR)\TeXLive.pch"

!ELSE 

ALL : "unzip - Win32 Release" ".\hlp\TeXLive.hm" ".\Release\TeXLive.hlp" ".\Release\TeXLive.cnt" "$(OUTDIR)\TeXLive.exe" "$(OUTDIR)\TeXLive.pch"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"unzip - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ActionProgressDlg.obj"
	-@erase "$(INTDIR)\ColorStaticST.obj"
	-@erase "$(INTDIR)\KeyValFile.obj"
	-@erase "$(INTDIR)\ListChoiceDlg.obj"
	-@erase "$(INTDIR)\Registry.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TeXLive.obj"
	-@erase "$(INTDIR)\TeXLive.pch"
	-@erase "$(INTDIR)\TeXLive.res"
	-@erase "$(INTDIR)\TeXLiveCmds.obj"
	-@erase "$(INTDIR)\TeXLiveDlg.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\Win32Util.obj"
	-@erase "$(OUTDIR)\TeXLive.exe"
	-@erase ".\Release\TeXLive.cnt"
	-@erase ".\Release\TeXLive.hlp"
	-@erase "hlp\TeXLive.hm"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "../TeXSetup" /I "../unzip" /I "../unzip/windll" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\TeXLive.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TeXLive.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=../unzip/Release/unzip.lib DelayImp.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\TeXLive.pdb" /machine:I386 /out:"$(OUTDIR)\TeXLive.exe" /delayload:advapi32.dll 
LINK32_OBJS= \
	"$(INTDIR)\ActionProgressDlg.obj" \
	"$(INTDIR)\ColorStaticST.obj" \
	"$(INTDIR)\KeyValFile.obj" \
	"$(INTDIR)\ListChoiceDlg.obj" \
	"$(INTDIR)\Registry.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TeXLive.obj" \
	"$(INTDIR)\TeXLiveCmds.obj" \
	"$(INTDIR)\TeXLiveDlg.obj" \
	"$(INTDIR)\Win32Util.obj" \
	"$(INTDIR)\TeXLive.res" \
	"..\unzip\Release\unzip.lib"

"$(OUTDIR)\TeXLive.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TeXLive - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : ".\hlp\TeXLive.hm" ".\Debug\TeXLive.hlp" ".\Debug\TeXLive.cnt" "$(OUTDIR)\TeXLive.exe" "$(OUTDIR)\TeXLive.pch"

!ELSE 

ALL : "unzip - Win32 Debug" ".\hlp\TeXLive.hm" ".\Debug\TeXLive.hlp" ".\Debug\TeXLive.cnt" "$(OUTDIR)\TeXLive.exe" "$(OUTDIR)\TeXLive.pch"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"unzip - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ActionProgressDlg.obj"
	-@erase "$(INTDIR)\ColorStaticST.obj"
	-@erase "$(INTDIR)\KeyValFile.obj"
	-@erase "$(INTDIR)\ListChoiceDlg.obj"
	-@erase "$(INTDIR)\Registry.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TeXLive.obj"
	-@erase "$(INTDIR)\TeXLive.pch"
	-@erase "$(INTDIR)\TeXLive.res"
	-@erase "$(INTDIR)\TeXLiveCmds.obj"
	-@erase "$(INTDIR)\TeXLiveDlg.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\Win32Util.obj"
	-@erase "$(OUTDIR)\TeXLive.exe"
	-@erase "$(OUTDIR)\TeXLive.ilk"
	-@erase "$(OUTDIR)\TeXLive.pdb"
	-@erase ".\Debug\TeXLive.cnt"
	-@erase ".\Debug\TeXLive.hlp"
	-@erase "hlp\TeXLive.hm"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "../TeXSetup" /I "../unzip" /I "../unzip/windll" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "TEST" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\TeXLive.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TeXLive.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=../unzip/Debug/unzip.lib DelayImp.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\TeXLive.pdb" /debug /machine:I386 /out:"$(OUTDIR)\TeXLive.exe" /pdbtype:sept /delayload:advapi32.dll 
LINK32_OBJS= \
	"$(INTDIR)\ActionProgressDlg.obj" \
	"$(INTDIR)\ColorStaticST.obj" \
	"$(INTDIR)\KeyValFile.obj" \
	"$(INTDIR)\ListChoiceDlg.obj" \
	"$(INTDIR)\Registry.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TeXLive.obj" \
	"$(INTDIR)\TeXLiveCmds.obj" \
	"$(INTDIR)\TeXLiveDlg.obj" \
	"$(INTDIR)\Win32Util.obj" \
	"$(INTDIR)\TeXLive.res" \
	"..\unzip\Debug\unzip.lib"

"$(OUTDIR)\TeXLive.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("TeXLive.dep")
!INCLUDE "TeXLive.dep"
!ELSE 
!MESSAGE Warning: cannot find "TeXLive.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "TeXLive - Win32 Release" || "$(CFG)" == "TeXLive - Win32 Debug"
SOURCE=.\ActionProgressDlg.cpp

"$(INTDIR)\ActionProgressDlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ColorStaticST.cpp

"$(INTDIR)\ColorStaticST.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\texsetup\KeyValFile.cpp

"$(INTDIR)\KeyValFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\texsetup\ListChoiceDlg.cpp

"$(INTDIR)\ListChoiceDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\texsetup\Registry.cpp

"$(INTDIR)\Registry.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "TeXLive - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "../TeXSetup" /I "../unzip" /I "../unzip/windll" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\TeXLive.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\TeXLive.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "TeXLive - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "../TeXSetup" /I "../unzip" /I "../unzip/windll" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "TEST" /Fp"$(INTDIR)\TeXLive.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\TeXLive.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\TeXLive.cpp

"$(INTDIR)\TeXLive.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hlp\TeXLive.hpj

!IF  "$(CFG)" == "TeXLive - Win32 Release"

OutDir=.\Release
InputPath=.\hlp\TeXLive.hpj
InputName=TeXLive

"$(INTDIR)\TeXLive.hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	start /wait hcw /C /E /M "hlp\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "hlp\$(InputName).hlp" goto :Error 
	copy "hlp\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo hlp\$(InputName).hpj(1) : error: 
	type "hlp\$(InputName).log" 
	:done 
<< 
	

!ELSEIF  "$(CFG)" == "TeXLive - Win32 Debug"

OutDir=.\Debug
InputPath=.\hlp\TeXLive.hpj
InputName=TeXLive

"$(INTDIR)\TeXLive.hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	start /wait hcw /C /E /M "hlp\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "hlp\$(InputName).hlp" goto :Error 
	copy "hlp\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo hlp\$(InputName).hpj(1) : error: 
	type "hlp\$(InputName).log" 
	:done 
<< 
	

!ENDIF 

SOURCE=.\TeXLive.rc

"$(INTDIR)\TeXLive.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\TeXLiveCmds.cpp

"$(INTDIR)\TeXLiveCmds.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TeXLiveDlg.cpp

"$(INTDIR)\TeXLiveDlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\texsetup\Win32Util.cpp

!IF  "$(CFG)" == "TeXLive - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "../TeXSetup" /I "../unzip" /I "../unzip/windll" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\TeXLive.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Win32Util.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "TeXLive - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "../TeXSetup" /I "../unzip" /I "../unzip/windll" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "TEST" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\Win32Util.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Resource.h

!IF  "$(CFG)" == "TeXLive - Win32 Release"

TargetName=TeXLive
InputPath=.\Resource.h

".\hlp\TeXLive.hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
<< 
	

!ELSEIF  "$(CFG)" == "TeXLive - Win32 Debug"

TargetName=TeXLive
InputPath=.\Resource.h

".\hlp\TeXLive.hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
<< 
	

!ENDIF 

SOURCE=.\hlp\TeXLive.cnt

!IF  "$(CFG)" == "TeXLive - Win32 Release"

OutDir=.\Release
InputPath=.\hlp\TeXLive.cnt
InputName=TeXLive

"$(INTDIR)\TeXLive.cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "hlp\$(InputName).cnt" $(OutDir)
<< 
	

!ELSEIF  "$(CFG)" == "TeXLive - Win32 Debug"

OutDir=.\Debug
InputPath=.\hlp\TeXLive.cnt
InputName=TeXLive

"$(INTDIR)\TeXLive.cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "hlp\$(InputName).cnt" $(OutDir)
<< 
	

!ENDIF 

!IF  "$(CFG)" == "TeXLive - Win32 Release"

"unzip - Win32 Release" : 
   cd "\Source\fptex\win32\unzip"
   $(MAKE) /$(MAKEFLAGS) /F .\unzip.mak CFG="unzip - Win32 Release" 
   cd "..\TeXLive"

"unzip - Win32 ReleaseCLEAN" : 
   cd "\Source\fptex\win32\unzip"
   $(MAKE) /$(MAKEFLAGS) /F .\unzip.mak CFG="unzip - Win32 Release" RECURSE=1 CLEAN 
   cd "..\TeXLive"

!ELSEIF  "$(CFG)" == "TeXLive - Win32 Debug"

"unzip - Win32 Debug" : 
   cd "\Source\fptex\win32\unzip"
   $(MAKE) /$(MAKEFLAGS) /F .\unzip.mak CFG="unzip - Win32 Debug" 
   cd "..\TeXLive"

"unzip - Win32 DebugCLEAN" : 
   cd "\Source\fptex\win32\unzip"
   $(MAKE) /$(MAKEFLAGS) /F .\unzip.mak CFG="unzip - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\TeXLive"

!ENDIF 


!ENDIF 

