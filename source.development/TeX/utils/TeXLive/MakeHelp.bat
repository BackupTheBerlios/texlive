@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by TEXLIVE.HPJ. >"hlp\TeXLive.hm"
echo. >>"hlp\TeXLive.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\TeXLive.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\TeXLive.hm"
echo. >>"hlp\TeXLive.hm"
echo // Prompts (IDP_*) >>"hlp\TeXLive.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\TeXLive.hm"
echo. >>"hlp\TeXLive.hm"
echo // Resources (IDR_*) >>"hlp\TeXLive.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\TeXLive.hm"
echo. >>"hlp\TeXLive.hm"
echo // Dialogs (IDD_*) >>"hlp\TeXLive.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\TeXLive.hm"
echo. >>"hlp\TeXLive.hm"
echo // Frame Controls (IDW_*) >>"hlp\TeXLive.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\TeXLive.hm"
REM -- Make help for Project TEXLIVE


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\TeXLive.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\TeXLive.hlp" goto :Error
if not exist "hlp\TeXLive.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\TeXLive.hlp" Debug
if exist Debug\nul copy "hlp\TeXLive.cnt" Debug
if exist Release\nul copy "hlp\TeXLive.hlp" Release
if exist Release\nul copy "hlp\TeXLive.cnt" Release
echo.
goto :done

:Error
echo hlp\TeXLive.hpj(1) : error: Problem encountered creating help file

:done
echo.
