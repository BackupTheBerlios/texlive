# Microsoft Developer Studio Generated NMAKE File, Based on TeXSetup.dsp
!IF "$(CFG)" == ""
CFG=TeXSetup - Win32 Debug
!MESSAGE No configuration specified. Defaulting to TeXSetup - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "TeXSetup - Win32 Release" && "$(CFG)" != "TeXSetup - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TeXSetup.mak" CFG="TeXSetup - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TeXSetup - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "TeXSetup - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "TeXSetup - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\TeXSetup.exe" "$(OUTDIR)\TeXSetup.pch"

!ELSE 

ALL : "geturl_static - Win32 Release" "expat - Win32 Release" "unzip - Win32 Release" "$(OUTDIR)\TeXSetup.exe" "$(OUTDIR)\TeXSetup.pch"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"unzip - Win32 ReleaseCLEAN" "expat - Win32 ReleaseCLEAN" "geturl_static - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ConfigPage.obj"
	-@erase "$(INTDIR)\Environment.obj"
	-@erase "$(INTDIR)\ErrorPage.obj"
	-@erase "$(INTDIR)\FileCopyPage.obj"
	-@erase "$(INTDIR)\FileDirectory.obj"
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\getopt1.obj"
	-@erase "$(INTDIR)\GetTPMPage.obj"
	-@erase "$(INTDIR)\KeyValFile.obj"
	-@erase "$(INTDIR)\ListChoiceDlg.obj"
	-@erase "$(INTDIR)\Log.obj"
	-@erase "$(INTDIR)\PackagesPage.obj"
	-@erase "$(INTDIR)\RebootPage.obj"
	-@erase "$(INTDIR)\Registry.obj"
	-@erase "$(INTDIR)\ReviewPage.obj"
	-@erase "$(INTDIR)\RootPage.obj"
	-@erase "$(INTDIR)\ShellLinkData.obj"
	-@erase "$(INTDIR)\SourcePage.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\texinstall.obj"
	-@erase "$(INTDIR)\TeXSetup.obj"
	-@erase "$(INTDIR)\TeXSetup.pch"
	-@erase "$(INTDIR)\TeXSetup.res"
	-@erase "$(INTDIR)\TeXSetupCommandLineInfo.obj"
	-@erase "$(INTDIR)\TeXSetupWizard.obj"
	-@erase "$(INTDIR)\TPM.obj"
	-@erase "$(INTDIR)\TPMspecial.obj"
	-@erase "$(INTDIR)\TransBmp.obj"
	-@erase "$(INTDIR)\TreePackage.obj"
	-@erase "$(INTDIR)\UninstallPage.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\WelcomePage.obj"
	-@erase "$(INTDIR)\Win32Util.obj"
	-@erase "$(OUTDIR)\TeXSetup.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /Zi /O2 /I "c:/program files/Microsoft SDK/Include" /I "../../libs/geturl" /I "../../libs/expat/lib" /I "../unzip/windll" /I "../unzip" /I "../../texk/kpathsea" /I "c:/program files/microsoft platform sdk/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\TeXSetup.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TeXSetup.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=../../libs/expat/lib/Release/expat.lib ../unzip/Release/unzip.lib DelayImp.lib ../../libs/geturl_static/Release/geturl_static.lib version.lib advapi32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\TeXSetup.pdb" /machine:I386 /out:"$(OUTDIR)\TeXSetup.exe" /delayload:wininet.dll /delayload:wsock32.dll /delayload:advapi32.dll 
LINK32_OBJS= \
	"$(INTDIR)\ConfigPage.obj" \
	"$(INTDIR)\Environment.obj" \
	"$(INTDIR)\ErrorPage.obj" \
	"$(INTDIR)\FileCopyPage.obj" \
	"$(INTDIR)\FileDirectory.obj" \
	"$(INTDIR)\getopt.obj" \
	"$(INTDIR)\getopt1.obj" \
	"$(INTDIR)\GetTPMPage.obj" \
	"$(INTDIR)\KeyValFile.obj" \
	"$(INTDIR)\ListChoiceDlg.obj" \
	"$(INTDIR)\Log.obj" \
	"$(INTDIR)\PackagesPage.obj" \
	"$(INTDIR)\RebootPage.obj" \
	"$(INTDIR)\Registry.obj" \
	"$(INTDIR)\ReviewPage.obj" \
	"$(INTDIR)\RootPage.obj" \
	"$(INTDIR)\ShellLinkData.obj" \
	"$(INTDIR)\SourcePage.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\texinstall.obj" \
	"$(INTDIR)\TeXSetup.obj" \
	"$(INTDIR)\TeXSetupCommandLineInfo.obj" \
	"$(INTDIR)\TeXSetupWizard.obj" \
	"$(INTDIR)\TPM.obj" \
	"$(INTDIR)\TPMspecial.obj" \
	"$(INTDIR)\TransBmp.obj" \
	"$(INTDIR)\TreePackage.obj" \
	"$(INTDIR)\UninstallPage.obj" \
	"$(INTDIR)\WelcomePage.obj" \
	"$(INTDIR)\Win32Util.obj" \
	"$(INTDIR)\TeXSetup.res" \
	"..\unzip\Release\unzip.lib" \
	"..\..\libs\expat\lib\Release\expat.lib" \
	"..\..\libs\geturl_static\Release\geturl_static.lib"

"$(OUTDIR)\TeXSetup.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\TeXSetup.exe" "$(OUTDIR)\TeXSetup.pch" "$(OUTDIR)\TeXSetup.bsc"

!ELSE 

ALL : "geturl_static - Win32 Debug" "expat - Win32 Debug" "unzip - Win32 Debug" "$(OUTDIR)\TeXSetup.exe" "$(OUTDIR)\TeXSetup.pch" "$(OUTDIR)\TeXSetup.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"unzip - Win32 DebugCLEAN" "expat - Win32 DebugCLEAN" "geturl_static - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ConfigPage.obj"
	-@erase "$(INTDIR)\ConfigPage.sbr"
	-@erase "$(INTDIR)\Environment.obj"
	-@erase "$(INTDIR)\Environment.sbr"
	-@erase "$(INTDIR)\ErrorPage.obj"
	-@erase "$(INTDIR)\ErrorPage.sbr"
	-@erase "$(INTDIR)\FileCopyPage.obj"
	-@erase "$(INTDIR)\FileCopyPage.sbr"
	-@erase "$(INTDIR)\FileDirectory.obj"
	-@erase "$(INTDIR)\FileDirectory.sbr"
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\getopt.sbr"
	-@erase "$(INTDIR)\getopt1.obj"
	-@erase "$(INTDIR)\getopt1.sbr"
	-@erase "$(INTDIR)\GetTPMPage.obj"
	-@erase "$(INTDIR)\GetTPMPage.sbr"
	-@erase "$(INTDIR)\KeyValFile.obj"
	-@erase "$(INTDIR)\KeyValFile.sbr"
	-@erase "$(INTDIR)\ListChoiceDlg.obj"
	-@erase "$(INTDIR)\ListChoiceDlg.sbr"
	-@erase "$(INTDIR)\Log.obj"
	-@erase "$(INTDIR)\Log.sbr"
	-@erase "$(INTDIR)\PackagesPage.obj"
	-@erase "$(INTDIR)\PackagesPage.sbr"
	-@erase "$(INTDIR)\RebootPage.obj"
	-@erase "$(INTDIR)\RebootPage.sbr"
	-@erase "$(INTDIR)\Registry.obj"
	-@erase "$(INTDIR)\Registry.sbr"
	-@erase "$(INTDIR)\ReviewPage.obj"
	-@erase "$(INTDIR)\ReviewPage.sbr"
	-@erase "$(INTDIR)\RootPage.obj"
	-@erase "$(INTDIR)\RootPage.sbr"
	-@erase "$(INTDIR)\ShellLinkData.obj"
	-@erase "$(INTDIR)\ShellLinkData.sbr"
	-@erase "$(INTDIR)\SourcePage.obj"
	-@erase "$(INTDIR)\SourcePage.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\texinstall.obj"
	-@erase "$(INTDIR)\texinstall.sbr"
	-@erase "$(INTDIR)\TeXSetup.obj"
	-@erase "$(INTDIR)\TeXSetup.pch"
	-@erase "$(INTDIR)\TeXSetup.res"
	-@erase "$(INTDIR)\TeXSetup.sbr"
	-@erase "$(INTDIR)\TeXSetupCommandLineInfo.obj"
	-@erase "$(INTDIR)\TeXSetupCommandLineInfo.sbr"
	-@erase "$(INTDIR)\TeXSetupWizard.obj"
	-@erase "$(INTDIR)\TeXSetupWizard.sbr"
	-@erase "$(INTDIR)\TPM.obj"
	-@erase "$(INTDIR)\TPM.sbr"
	-@erase "$(INTDIR)\TPMspecial.obj"
	-@erase "$(INTDIR)\TPMspecial.sbr"
	-@erase "$(INTDIR)\TransBmp.obj"
	-@erase "$(INTDIR)\TransBmp.sbr"
	-@erase "$(INTDIR)\TreePackage.obj"
	-@erase "$(INTDIR)\TreePackage.sbr"
	-@erase "$(INTDIR)\UninstallPage.obj"
	-@erase "$(INTDIR)\UninstallPage.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\WelcomePage.obj"
	-@erase "$(INTDIR)\WelcomePage.sbr"
	-@erase "$(INTDIR)\Win32Util.obj"
	-@erase "$(INTDIR)\Win32Util.sbr"
	-@erase "$(OUTDIR)\TeXSetup.bsc"
	-@erase "$(OUTDIR)\TeXSetup.exe"
	-@erase "$(OUTDIR)\TeXSetup.ilk"
	-@erase "$(OUTDIR)\TeXSetup.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /GX /ZI /Od /I "c:/program files/Microsoft SDK/Include" /I "../../libs/geturl" /I "../../libs/expat/lib" /I "../unzip/windll" /I "../unzip" /I "../../texk/kpathsea" /I "c:/program files/microsoft platform sdk/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDLL" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\TeXSetup.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TeXSetup.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ConfigPage.sbr" \
	"$(INTDIR)\Environment.sbr" \
	"$(INTDIR)\ErrorPage.sbr" \
	"$(INTDIR)\FileCopyPage.sbr" \
	"$(INTDIR)\FileDirectory.sbr" \
	"$(INTDIR)\getopt.sbr" \
	"$(INTDIR)\getopt1.sbr" \
	"$(INTDIR)\GetTPMPage.sbr" \
	"$(INTDIR)\KeyValFile.sbr" \
	"$(INTDIR)\ListChoiceDlg.sbr" \
	"$(INTDIR)\Log.sbr" \
	"$(INTDIR)\PackagesPage.sbr" \
	"$(INTDIR)\RebootPage.sbr" \
	"$(INTDIR)\Registry.sbr" \
	"$(INTDIR)\ReviewPage.sbr" \
	"$(INTDIR)\RootPage.sbr" \
	"$(INTDIR)\ShellLinkData.sbr" \
	"$(INTDIR)\SourcePage.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\texinstall.sbr" \
	"$(INTDIR)\TeXSetup.sbr" \
	"$(INTDIR)\TeXSetupCommandLineInfo.sbr" \
	"$(INTDIR)\TeXSetupWizard.sbr" \
	"$(INTDIR)\TPM.sbr" \
	"$(INTDIR)\TPMspecial.sbr" \
	"$(INTDIR)\TransBmp.sbr" \
	"$(INTDIR)\TreePackage.sbr" \
	"$(INTDIR)\UninstallPage.sbr" \
	"$(INTDIR)\WelcomePage.sbr" \
	"$(INTDIR)\Win32Util.sbr"

"$(OUTDIR)\TeXSetup.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=../../libs/expat/lib/Debug/expat.lib ../unzip/Debug/unzip.lib ../../libs/geturl_static/Debug/geturl_static.lib advapi32.lib version.lib delayimp.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\TeXSetup.pdb" /debug /machine:I386 /out:"$(OUTDIR)\TeXSetup.exe" /pdbtype:sept /delayload:wininet.dll /delayload:wsock32.dll /delayload:advapi32.dll 
LINK32_OBJS= \
	"$(INTDIR)\ConfigPage.obj" \
	"$(INTDIR)\Environment.obj" \
	"$(INTDIR)\ErrorPage.obj" \
	"$(INTDIR)\FileCopyPage.obj" \
	"$(INTDIR)\FileDirectory.obj" \
	"$(INTDIR)\getopt.obj" \
	"$(INTDIR)\getopt1.obj" \
	"$(INTDIR)\GetTPMPage.obj" \
	"$(INTDIR)\KeyValFile.obj" \
	"$(INTDIR)\ListChoiceDlg.obj" \
	"$(INTDIR)\Log.obj" \
	"$(INTDIR)\PackagesPage.obj" \
	"$(INTDIR)\RebootPage.obj" \
	"$(INTDIR)\Registry.obj" \
	"$(INTDIR)\ReviewPage.obj" \
	"$(INTDIR)\RootPage.obj" \
	"$(INTDIR)\ShellLinkData.obj" \
	"$(INTDIR)\SourcePage.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\texinstall.obj" \
	"$(INTDIR)\TeXSetup.obj" \
	"$(INTDIR)\TeXSetupCommandLineInfo.obj" \
	"$(INTDIR)\TeXSetupWizard.obj" \
	"$(INTDIR)\TPM.obj" \
	"$(INTDIR)\TPMspecial.obj" \
	"$(INTDIR)\TransBmp.obj" \
	"$(INTDIR)\TreePackage.obj" \
	"$(INTDIR)\UninstallPage.obj" \
	"$(INTDIR)\WelcomePage.obj" \
	"$(INTDIR)\Win32Util.obj" \
	"$(INTDIR)\TeXSetup.res" \
	"..\unzip\Debug\unzip.lib" \
	"..\..\libs\expat\lib\Debug\expat.lib" \
	"..\..\libs\geturl_static\Debug\geturl_static.lib"

"$(OUTDIR)\TeXSetup.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("TeXSetup.dep")
!INCLUDE "TeXSetup.dep"
!ELSE 
!MESSAGE Warning: cannot find "TeXSetup.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "TeXSetup - Win32 Release" || "$(CFG)" == "TeXSetup - Win32 Debug"
SOURCE=.\ConfigPage.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\ConfigPage.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\ConfigPage.obj"	"$(INTDIR)\ConfigPage.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Environment.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\Environment.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\Environment.obj"	"$(INTDIR)\Environment.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ErrorPage.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\ErrorPage.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\ErrorPage.obj"	"$(INTDIR)\ErrorPage.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\FileCopyPage.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\FileCopyPage.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\FileCopyPage.obj"	"$(INTDIR)\FileCopyPage.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\FileDirectory.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\FileDirectory.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\FileDirectory.obj"	"$(INTDIR)\FileDirectory.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\..\texk\kpathsea\getopt.c

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\getopt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\getopt.obj"	"$(INTDIR)\getopt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\texk\kpathsea\getopt1.c

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\getopt1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\getopt1.obj"	"$(INTDIR)\getopt1.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\GetTPMPage.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\GetTPMPage.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\GetTPMPage.obj"	"$(INTDIR)\GetTPMPage.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\KeyValFile.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\KeyValFile.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\KeyValFile.obj"	"$(INTDIR)\KeyValFile.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ListChoiceDlg.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\ListChoiceDlg.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\ListChoiceDlg.obj"	"$(INTDIR)\ListChoiceDlg.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Log.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\Log.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\Log.obj"	"$(INTDIR)\Log.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\PackagesPage.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\PackagesPage.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\PackagesPage.obj"	"$(INTDIR)\PackagesPage.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\RebootPage.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\RebootPage.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\RebootPage.obj"	"$(INTDIR)\RebootPage.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Registry.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\Registry.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\Registry.obj"	"$(INTDIR)\Registry.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ReviewPage.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\ReviewPage.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\ReviewPage.obj"	"$(INTDIR)\ReviewPage.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\RootPage.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\RootPage.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\RootPage.obj"	"$(INTDIR)\RootPage.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ShellLinkData.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\ShellLinkData.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\ShellLinkData.obj"	"$(INTDIR)\ShellLinkData.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\SourcePage.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\SourcePage.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\SourcePage.obj"	"$(INTDIR)\SourcePage.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "c:/program files/Microsoft SDK/Include" /I "../../libs/geturl" /I "../../libs/expat/lib" /I "../unzip/windll" /I "../unzip" /I "../../texk/kpathsea" /I "c:/program files/microsoft platform sdk/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\TeXSetup.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\TeXSetup.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /GX /ZI /Od /I "c:/program files/Microsoft SDK/Include" /I "../../libs/geturl" /I "../../libs/expat/lib" /I "../unzip/windll" /I "../unzip" /I "../../texk/kpathsea" /I "c:/program files/microsoft platform sdk/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\TeXSetup.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\TeXSetup.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\texinstall.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\texinstall.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\texinstall.obj"	"$(INTDIR)\texinstall.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\TeXSetup.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "c:/program files/Microsoft SDK/Include" /I "../../libs/geturl" /I "../../libs/expat/lib" /I "../unzip/windll" /I "../unzip" /I "../../texk/kpathsea" /I "c:/program files/microsoft platform sdk/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\TeXSetup.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /GX /ZI /Od /I "c:/program files/Microsoft SDK/Include" /I "../../libs/geturl" /I "../../libs/expat/lib" /I "../unzip/windll" /I "../unzip" /I "../../texk/kpathsea" /I "c:/program files/microsoft platform sdk/include" /I "../../kpathsea" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDLL" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\TeXSetup.obj"	"$(INTDIR)\TeXSetup.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\TeXSetup.rc

"$(INTDIR)\TeXSetup.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\TeXSetupCommandLineInfo.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\TeXSetupCommandLineInfo.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\TeXSetupCommandLineInfo.obj"	"$(INTDIR)\TeXSetupCommandLineInfo.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\TeXSetupWizard.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\TeXSetupWizard.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\TeXSetupWizard.obj"	"$(INTDIR)\TeXSetupWizard.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\TPM.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\TPM.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\TPM.obj"	"$(INTDIR)\TPM.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\TPMspecial.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\TPMspecial.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\TPMspecial.obj"	"$(INTDIR)\TPMspecial.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\TransBmp.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\TransBmp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\TransBmp.obj"	"$(INTDIR)\TransBmp.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\TreePackage.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\TreePackage.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\TreePackage.obj"	"$(INTDIR)\TreePackage.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\UninstallPage.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\UninstallPage.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\UninstallPage.obj"	"$(INTDIR)\UninstallPage.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\WelcomePage.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\WelcomePage.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\WelcomePage.obj"	"$(INTDIR)\WelcomePage.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Win32Util.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"


"$(INTDIR)\Win32Util.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"


"$(INTDIR)\Win32Util.obj"	"$(INTDIR)\Win32Util.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

!IF  "$(CFG)" == "TeXSetup - Win32 Release"

!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"

!ENDIF 

!IF  "$(CFG)" == "TeXSetup - Win32 Release"

"unzip - Win32 Release" : 
   cd "\Source\fptex\win32\unzip"
   $(MAKE) /$(MAKEFLAGS) /F .\unzip.mak CFG="unzip - Win32 Release" 
   cd "..\TeXSetup"

"unzip - Win32 ReleaseCLEAN" : 
   cd "\Source\fptex\win32\unzip"
   $(MAKE) /$(MAKEFLAGS) /F .\unzip.mak CFG="unzip - Win32 Release" RECURSE=1 CLEAN 
   cd "..\TeXSetup"

!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"

"unzip - Win32 Debug" : 
   cd "\Source\fptex\win32\unzip"
   $(MAKE) /$(MAKEFLAGS) /F .\unzip.mak CFG="unzip - Win32 Debug" 
   cd "..\TeXSetup"

"unzip - Win32 DebugCLEAN" : 
   cd "\Source\fptex\win32\unzip"
   $(MAKE) /$(MAKEFLAGS) /F .\unzip.mak CFG="unzip - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\TeXSetup"

!ENDIF 

!IF  "$(CFG)" == "TeXSetup - Win32 Release"

"expat - Win32 Release" : 
   cd "\Source\fptex\libs\expat\lib"
   $(MAKE) /$(MAKEFLAGS) /F .\expat.mak CFG="expat - Win32 Release" 
   cd "..\..\..\win32\TeXSetup"

"expat - Win32 ReleaseCLEAN" : 
   cd "\Source\fptex\libs\expat\lib"
   $(MAKE) /$(MAKEFLAGS) /F .\expat.mak CFG="expat - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\win32\TeXSetup"

!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"

"expat - Win32 Debug" : 
   cd "\Source\fptex\libs\expat\lib"
   $(MAKE) /$(MAKEFLAGS) /F .\expat.mak CFG="expat - Win32 Debug" 
   cd "..\..\..\win32\TeXSetup"

"expat - Win32 DebugCLEAN" : 
   cd "\Source\fptex\libs\expat\lib"
   $(MAKE) /$(MAKEFLAGS) /F .\expat.mak CFG="expat - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\win32\TeXSetup"

!ENDIF 

!IF  "$(CFG)" == "TeXSetup - Win32 Release"

"geturl_static - Win32 Release" : 
   cd "\Source\fptex\libs\geturl_static"
   $(MAKE) /$(MAKEFLAGS) /F .\geturl_static.mak CFG="geturl_static - Win32 Release" 
   cd "..\..\win32\TeXSetup"

"geturl_static - Win32 ReleaseCLEAN" : 
   cd "\Source\fptex\libs\geturl_static"
   $(MAKE) /$(MAKEFLAGS) /F .\geturl_static.mak CFG="geturl_static - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\win32\TeXSetup"

!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"

"geturl_static - Win32 Debug" : 
   cd "\Source\fptex\libs\geturl_static"
   $(MAKE) /$(MAKEFLAGS) /F .\geturl_static.mak CFG="geturl_static - Win32 Debug" 
   cd "..\..\win32\TeXSetup"

"geturl_static - Win32 DebugCLEAN" : 
   cd "\Source\fptex\libs\geturl_static"
   $(MAKE) /$(MAKEFLAGS) /F .\geturl_static.mak CFG="geturl_static - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\win32\TeXSetup"

!ENDIF 


!ENDIF 

