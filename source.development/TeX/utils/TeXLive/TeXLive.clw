; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CTeXLiveDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "TeXLive.h"

ClassCount=4
Class1=CTeXLiveApp
Class2=CTeXLiveDlg
Class3=CAboutDlg

ResourceCount=7
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_TEXLIVE_DIALOG
Resource4=IDD_DIALOG_LIST_CHOICE
Resource5=IDD_ABOUTBOX (English (U.S.))
Class4=CActionProgressDlg
Resource6=IDD_ACTION_PROGRESS_DIALOG
Resource7=IDD_TEXLIVE_DIALOG (English (U.S.))

[CLS:CTeXLiveApp]
Type=0
HeaderFile=TeXLive.h
ImplementationFile=TeXLive.cpp
Filter=N
LastObject=CTeXLiveApp

[CLS:CTeXLiveDlg]
Type=0
HeaderFile=TeXLiveDlg.h
ImplementationFile=TeXLiveDlg.cpp
Filter=D
LastObject=CTeXLiveDlg
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=TeXLiveDlg.h
ImplementationFile=TeXLiveDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg


[DLG:IDD_TEXLIVE_DIALOG]
Type=1
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Class=CTeXLiveDlg

[DLG:IDD_TEXLIVE_DIALOG (English (U.S.))]
Type=1
Class=CTeXLiveDlg
ControlCount=16
Control1=IDC_RUN_TEXSETUP,button,1342251008
Control2=IDC_TEXLIVE_MAINTENANCE,button,1342242816
Control3=IDC_TEXLIVE_UNINSTALL,button,1342242816
Control4=IDC_RUN_TEX_CDROM,button,1342254848
Control5=IDC_CDROM_CLEANUP,button,1342242816
Control6=IDC_UPGRADE_DLLS,button,1342242816
Control7=IDC_RUN_TEXLIVE_DOC,button,1342254848
Control8=IDC_GOTO_TUG,button,1342254848
Control9=IDC_GOTO_FPTEX,button,1342254848
Control10=IDCLOSE,button,1342242817
Control11=IDC_STATIC_INSTALLATION,static,1342308865
Control12=IDC_STATIC_DOCUMENTATION,static,1342308865
Control13=IDC_BROWSE_CDROM,button,1208037120
Control14=ID_HELP,button,1208025088
Control15=IDC_STATIC,static,1342179342
Control16=IDC_STATIC_WELCOME_TEXLIVE,static,1342308865

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[CLS:CActionProgressDlg]
Type=0
HeaderFile=ActionProgressDlg.h
ImplementationFile=ActionProgressDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_STATIC_ACTION

[DLG:IDD_ACTION_PROGRESS_DIALOG]
Type=1
Class=CActionProgressDlg
ControlCount=3
Control1=IDOK,button,1073807361
Control2=IDCANCEL,button,1073807360
Control3=IDC_STATIC_ACTION,static,1342308865

[DLG:IDD_DIALOG_LIST_CHOICE]
Type=1
Class=?
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC_DESCRIPTION,static,1342308352
Control4=IDC_COMBO_LIST_CHOICE,combobox,1344340289
Control5=IDC_LISTCHOICE_RADIO1,button,1342177289
Control6=IDC_LISTCHOICE_RADIO2,button,1342177289
Control7=IDC_LISTCHOICE_RADIO3,button,1342177289

