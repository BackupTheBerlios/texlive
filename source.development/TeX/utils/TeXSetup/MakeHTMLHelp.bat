@echo off

REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by TeXLive.HHP. >"hlp\HTMLDefines.h"
echo. >>"hlp\HTMLDefines.h"
echo // Commands (ID_* and IDM_*) >>"hlp\HTMLDefines.h"
makehm /h ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\HTMLDefines.h"
echo. >>"hlp\HTMLDefines.h"
echo // Prompts (IDP_*) >>"hlp\HTMLDefines.h"
makehm /h IDP_,HIDP_,0x30000 resource.h >>"hlp\HTMLDefines.h"
echo. >>"hlp\HTMLDefines.h"
echo // Resources (IDR_*) >>"hlp\HTMLDefines.h"
makehm /h IDR_,HIDR_,0x20000 resource.h >>"hlp\HTMLDefines.h"
echo. >>"hlp\HTMLDefines.h"
echo // Dialogs (IDD_*) >>"hlp\HTMLDefines.h"
makehm /h IDD_,HIDD_,0x20000 resource.h >>"hlp\HTMLDefines.h"
echo. >>"hlp\HTMLDefines.h"
echo // Frame Controls (IDW_*) >>"hlp\HTMLDefines.h"
makehm /h /a afxhh.h IDW_,HIDW_,0x50000 resource.h >>"hlp\HTMLDefines.h"

REM -- Make help for Project TeXLive

echo Building Win32 HTML Help files
hhc "hlp\texlive.hhp"
if not exist "hlp\texlive.chm" goto :Error 
if exist Debug\nul copy "hlp\texlive.chm" Debug
if exist Release\nul copy "hlp\texlive.chm" Release
goto :done

:Error 
echo texlive.hhp(1) : error: 
echo.

:done 
echo.

