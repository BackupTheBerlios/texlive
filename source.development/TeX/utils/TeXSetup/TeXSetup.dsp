# Microsoft Developer Studio Project File - Name="TeXSetup" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=TeXSetup - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TeXSetup.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TeXSetup - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "c:/program files/Microsoft SDK/Include" /I "../../libs/geturl" /I "../../libs/expat/lib" /I "../unzip/windll" /I "../unzip" /I "../../texk/kpathsea" /I "c:/program files/microsoft platform sdk/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ../../libs/expat/lib/Release/expat.lib ../unzip/Release/unzip.lib DelayImp.lib ../../libs/geturl_static/Release/geturl_static.lib version.lib advapi32.lib /nologo /subsystem:windows /machine:I386 /delayload:wininet.dll /delayload:wsock32.dll /delayload:advapi32.dll
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /GX /ZI /Od /I "c:/program files/Microsoft SDK/Include" /I "../../libs/geturl" /I "../../libs/expat/lib" /I "../unzip/windll" /I "../unzip" /I "../../texk/kpathsea" /I "c:/program files/microsoft platform sdk/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDLL" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ../../libs/expat/lib/Debug/expat.lib ../unzip/Debug/unzip.lib ../../libs/geturl_static/Debug/geturl_static.lib advapi32.lib version.lib delayimp.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /delayload:wininet.dll /delayload:wsock32.dll /delayload:advapi32.dll
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "TeXSetup - Win32 Release"
# Name "TeXSetup - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ConfigPage.cpp
# End Source File
# Begin Source File

SOURCE=.\Environment.cpp
# End Source File
# Begin Source File

SOURCE=.\ErrorPage.cpp
# End Source File
# Begin Source File

SOURCE=.\FileCopyPage.cpp
# End Source File
# Begin Source File

SOURCE=.\FileDirectory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\texk\kpathsea\getopt.c
# End Source File
# Begin Source File

SOURCE=..\..\texk\kpathsea\getopt1.c
# End Source File
# Begin Source File

SOURCE=.\GetTPMPage.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyValFile.cpp
# End Source File
# Begin Source File

SOURCE=.\ListChoiceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\PackagesPage.cpp
# End Source File
# Begin Source File

SOURCE=.\RebootPage.cpp
# End Source File
# Begin Source File

SOURCE=.\Registry.cpp
# End Source File
# Begin Source File

SOURCE=.\ReviewPage.cpp
# End Source File
# Begin Source File

SOURCE=.\RootPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellLinkData.cpp
# End Source File
# Begin Source File

SOURCE=.\SourcePage.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\texinstall.cpp
# End Source File
# Begin Source File

SOURCE=.\TeXSetup.cpp

!IF  "$(CFG)" == "TeXSetup - Win32 Release"

!ELSEIF  "$(CFG)" == "TeXSetup - Win32 Debug"

# ADD CPP /I "../../kpathsea"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TeXSetup.rc
# End Source File
# Begin Source File

SOURCE=.\TeXSetupCommandLineInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\TeXSetupWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\TPM.cpp
# End Source File
# Begin Source File

SOURCE=.\TPMspecial.cpp
# End Source File
# Begin Source File

SOURCE=.\TransBmp.cpp
# End Source File
# Begin Source File

SOURCE=.\TreePackage.cpp
# End Source File
# Begin Source File

SOURCE=.\UninstallPage.cpp
# End Source File
# Begin Source File

SOURCE=.\WelcomePage.cpp
# End Source File
# Begin Source File

SOURCE=.\Win32Util.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ConfigPage.h
# End Source File
# Begin Source File

SOURCE=.\ErrorPage.h
# End Source File
# Begin Source File

SOURCE=.\FileCopyPage.h
# End Source File
# Begin Source File

SOURCE=.\FileDirectory.h
# End Source File
# Begin Source File

SOURCE=..\..\kpathsea\getopt.h
# End Source File
# Begin Source File

SOURCE=.\GetTPMPage.h
# End Source File
# Begin Source File

SOURCE=.\KeyValFile.h
# End Source File
# Begin Source File

SOURCE=.\LimitSingleInstance.h
# End Source File
# Begin Source File

SOURCE=.\ListChoiceDlg.h
# End Source File
# Begin Source File

SOURCE=.\Log.h
# End Source File
# Begin Source File

SOURCE=.\PackagesPage.h
# End Source File
# Begin Source File

SOURCE=.\RebootPage.h
# End Source File
# Begin Source File

SOURCE=.\Registry.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ReviewPage.h
# End Source File
# Begin Source File

SOURCE=.\RootPage.h
# End Source File
# Begin Source File

SOURCE=.\SourcePage.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\texinstall.h
# End Source File
# Begin Source File

SOURCE=.\TeXSetup.h
# End Source File
# Begin Source File

SOURCE=.\TeXSetupCommandLineInfo.h
# End Source File
# Begin Source File

SOURCE=.\TeXSetupDlg.h
# End Source File
# Begin Source File

SOURCE=.\TeXSetupWizard.h
# End Source File
# Begin Source File

SOURCE=.\TPM.h
# End Source File
# Begin Source File

SOURCE=.\TPMspecial.h
# End Source File
# Begin Source File

SOURCE=.\TransBmp.h
# End Source File
# Begin Source File

SOURCE=.\TreePackage.h
# End Source File
# Begin Source File

SOURCE=.\UninstallPage.h
# End Source File
# Begin Source File

SOURCE=.\WelcomePage.h
# End Source File
# Begin Source File

SOURCE=.\Win32Util.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm256.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon3.ico
# End Source File
# Begin Source File

SOURCE=.\res\internet.bmp
# End Source File
# Begin Source File

SOURCE=".\res\lion-mini.bmp"
# End Source File
# Begin Source File

SOURCE=.\res\MainFrame.ico
# End Source File
# Begin Source File

SOURCE=.\res\Setup.ico
# End Source File
# Begin Source File

SOURCE=.\res\texsetup.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TeXSetup.ico
# End Source File
# Begin Source File

SOURCE=.\res\TeXSetup.rc2
# End Source File
# Begin Source File

SOURCE=.\res\tree1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tree3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tree4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tree_box.bmp
# End Source File
# Begin Source File

SOURCE=.\res\uninstall.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
