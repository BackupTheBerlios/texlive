; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CWelcomePage
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "TeXSetup.h"
LastPage=0

ClassCount=24
Class1=CCustomDirsPage
Class2=CPackagesPage
Class3=CFileCopyPage
Class4=CFinishPage
Class5=CPreviewWnd
Class6=CListChoiceDlg
Class7=CReviewPage
Class8=CRootPage
Class9=CSetupStylePage
Class10=CTeXSetupApp
Class11=CAboutDlg
Class12=CTeXSetupDlg
Class13=CTeXSetupWizard
Class14=CWelcomePage

ResourceCount=20
Resource1=IDD_PROPPAGE_CONFIG (English (U.S.))
Resource2=IDD_DIALOG_TPM_PROGRESS
Resource3=IDD_PROPPAGE_REBOOT (English (U.S.))
Resource4=IDD_ABOUTBOX (English (U.S.))
Resource5=IDD_PROPPAGE_FINISH (English (U.S.))
Resource6=IDD_TEXSETUP_DIALOG (English (U.S.))
Resource7=IDD_DIALOG1 (English (U.S.))
Resource8=IDD_PROPPAGE_PACKAGES (English (U.S.))
Resource9=IDD_PROPPAGE_ERROR (English (U.S.))
Resource10=IDD_PROPPAGE_SETUP_STYLE (English (U.S.))
Resource11=IDD_PROPPAGE_SUPPLEMENT (English (U.S.))
Resource12=IDD_PROPPAGE_GETTPM (English (U.S.))
Class15=CSupplementPage
Class16=CErrorPage
Resource13=IDD_PROPPAGE_SOURCE (English (U.S.))
Class17=CRebootPage
Class18=CTreePackage
Resource14=IDD_PROPPAGE_UNINSTALL (English (U.S.))
Class19=CConfigPage
Resource15=IDD_PROPPAGE_CUSTOM_DIRS (English (U.S.))
Class20=CUninstallPage
Resource16=IDD_PROPPAGE_WELCOME (English (U.S.))
Class21=CSourcePage
Resource17=IDD_PROPPAGE_ROOT (English (U.S.))
Class22=CTpmProgress
Resource18=IDD_PROPPAGE_FILE_COPY (English (U.S.))
Class23=CGetTPMPage
Class24=CTransBmp
Resource19=IDD_PROPPAGE_REVIEW (English (U.S.))
Resource20=IDD_DIALOG_LIST_CHOICE

[CLS:CCustomDirsPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=CustomDirsPage.h
ImplementationFile=CustomDirsPage.cpp
Filter=D
VirtualFilter=idWC
LastObject=CCustomDirsPage

[CLS:CPackagesPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=PackagesPage.h
ImplementationFile=PackagesPage.cpp
Filter=D
VirtualFilter=idWC
LastObject=IDC_CHECK_SOURCE

[CLS:CFileCopyPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=FileCopyPage.h
ImplementationFile=FileCopyPage.cpp
Filter=D
VirtualFilter=idWC
LastObject=IDC_STATIC_PERCENT_UNIT

[CLS:CFinishPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=FinishPage.h
ImplementationFile=FinishPage.cpp
Filter=D
VirtualFilter=idWC
LastObject=IDC_CHECK1

[CLS:CPreviewWnd]
Type=0
BaseClass=CWnd
HeaderFile=PreviewWnd.h
ImplementationFile=PreviewWnd.cpp

[CLS:CReviewPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=ReviewPage.h
ImplementationFile=ReviewPage.cpp
Filter=D
VirtualFilter=idWC
LastObject=CReviewPage

[CLS:CRootPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=RootPage.h
ImplementationFile=RootPage.cpp
Filter=D
VirtualFilter=idWC
LastObject=IDC_ROOTDIR

[CLS:CSetupStylePage]
Type=0
BaseClass=CPropertyPage
HeaderFile=SetupStylePage.h
ImplementationFile=SetupStylePage.cpp
Filter=D
VirtualFilter=idWC
LastObject=CSetupStylePage

[CLS:CTeXSetupApp]
Type=0
BaseClass=CWinApp
HeaderFile=TeXSetup.h
ImplementationFile=TeXSetup.cpp
LastObject=CTeXSetupApp
Filter=N
VirtualFilter=AC

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=TeXSetupDlg.cpp
ImplementationFile=TeXSetupDlg.cpp
LastObject=CAboutDlg

[CLS:CTeXSetupDlg]
Type=0
BaseClass=CDialog
HeaderFile=TeXSetupDlg.h
ImplementationFile=TeXSetupDlg.cpp
LastObject=IDCANCEL

[CLS:CTeXSetupWizard]
Type=0
BaseClass=CPropertySheet
HeaderFile=TeXSetupWizard.h
ImplementationFile=TeXSetupWizard.cpp
Filter=W
VirtualFilter=hWC
LastObject=CTeXSetupWizard

[DLG:IDD_PROPPAGE_PACKAGES]
Type=1
Class=CPackagesPage

[DLG:IDD_PROPPAGE_FILE_COPY]
Type=1
Class=CFileCopyPage

[DLG:CG_IDD_PROGRESS]
Type=1
Class=CProgressDlg

[DLG:IDD_PROPPAGE_REVIEW]
Type=1
Class=CReviewPage

[DLG:IDD_PROPPAGE_ROOT]
Type=1
Class=CRootPage

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[DLG:IDD_PROPPAGE_WELCOME]
Type=1
Class=CWelcomePage

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_TEXSETUP_DIALOG (English (U.S.))]
Type=1
Class=?
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG1 (English (U.S.))]
Type=1
Class=?
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816

[DLG:IDD_PROPPAGE_WELCOME (English (U.S.))]
Type=1
Class=CWelcomePage
ControlCount=7
Control1=IDC_EDIT_LICENSE,edit,1350568004
Control2=IDC_CHECK_QUICKINSTALL,button,1342242819
Control3=IDC_STATIC_EXPLANATION,static,1342308352
Control4=IDC_STATIC,static,1342177294
Control5=IDC_STATIC,static,1342181383
Control6=IDC_STATIC_HEADER,static,1342308352
Control7=IDC_CHECK_ADMINISTRATOR,button,1342242819

[DLG:IDD_PROPPAGE_ROOT (English (U.S.))]
Type=1
Class=CRootPage
ControlCount=22
Control1=IDC_ROOTDIR,edit,1350631552
Control2=IDC_BROWSE,button,1342242816
Control3=IDC_STATIC_HEADER,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342181383
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC_SPACE_AVAILABLE,static,1342308354
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC_DEFAULT_SIZE,static,1342308354
Control11=IDC_STATIC,static,1342308352
Control12=IDC_LOCAL_TEXMF,edit,1350631552
Control13=IDC_EXTRA_TEXMF,edit,1350631552
Control14=IDC_HOME_TEXMF,edit,1350631552
Control15=IDC_VAR_TEXMF,edit,1350631552
Control16=IDC_VAR_FONTS,edit,1350631552
Control17=IDC_STATIC,static,1342308352
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308352
Control21=IDC_STATIC,static,1342308352
Control22=IDC_CHECK_CUSTOM_DIRS,button,1342242819

[DLG:IDD_PROPPAGE_SETUP_STYLE (English (U.S.))]
Type=1
Class=CSetupStylePage
ControlCount=19
Control1=IDC_BASIC_INSTALL,button,1342373897
Control2=IDC_REC_INSTALL,button,1342242825
Control3=IDC_FULL_INSTALL,button,1342242825
Control4=IDC_CUSTOM_INSTALL,button,1342242825
Control5=IDC_CDROM_INSTALL,button,1342242825
Control6=IDC_CHECK_DOC,button,1342373891
Control7=IDC_CHECK_SOURCE,button,1342242819
Control8=IDC_CHECK_ADMINISTRATOR,button,1342242819
Control9=IDC_STATIC_HEADER,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342181383
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC_SPACE_AVAILABLE,static,1342308354
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC_TOTAL_SIZE,static,1342308354
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC_ADMIN,static,1342308352

[DLG:IDD_PROPPAGE_CUSTOM_DIRS (English (U.S.))]
Type=1
Class=CCustomDirsPage
ControlCount=15
Control1=IDC_LOCAL_TEXMF,edit,1350631552
Control2=IDC_EXTRA_TEXMF,edit,1350631552
Control3=IDC_HOME_TEXMF,edit,1350631552
Control4=IDC_VAR_TEXMF,edit,1350631552
Control5=IDC_VAR_FONTS,edit,1350631552
Control6=IDC_BROWSE,button,1342177280
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342181383
Control14=IDC_STATIC_HEADER,static,1342308352
Control15=IDC_STATIC,static,1342181383

[DLG:IDD_PROPPAGE_PACKAGES (English (U.S.))]
Type=1
Class=CPackagesPage
ControlCount=17
Control1=IDC_TREE_PACKAGES,SysTreeView32,1350635559
Control2=IDC_STATIC_TOTAL_SIZE,static,1342308354
Control3=IDC_EDIT_DESCRIPTION,edit,1350699076
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC_PACKAGE_SIZE,static,1342308354
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342181383
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC_HEADER,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC_SPACE_AVAILABLE,static,1342308354
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_CHECK_DOC,button,1342373923
Control17=IDC_CHECK_SOURCE,button,1342242851

[DLG:IDD_PROPPAGE_REVIEW (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDC_EDIT_SUMMARY,edit,1352665284
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342181383
Control4=IDC_STATIC_HEADER,static,1342308352

[DLG:IDD_PROPPAGE_FINISH (English (U.S.))]
Type=1
Class=CFinishPage
ControlCount=7
Control1=IDC_STATIC,static,1342181383
Control2=IDC_STATIC,static,1342177294
Control3=IDC_STATIC,static,1342181383
Control4=IDC_STATIC_HEADER,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_CHECK1,button,1342242819
Control7=IDC_STATIC,static,1342308352

[DLG:IDD_PROPPAGE_FILE_COPY (English (U.S.))]
Type=1
Class=CFileCopyPage
ControlCount=14
Control1=CG_IDC_PROGDLG_PROGRESS_PACKAGE,msctls_progress32,1342177281
Control2=IDC_STATIC_PERCENT_PACKAGE,static,1342308352
Control3=CG_IDC_PROGDLG_PROGRESS_TOTAL,msctls_progress32,1342177281
Control4=IDC_STATIC_PERCENT_TOTAL,static,1342308352
Control5=IDC_STATIC,static,1342181383
Control6=IDC_STATIC_HEADER,static,1342308352
Control7=IDC_STATIC,static,1342181383
Control8=IDC_PROMPT,static,1342308352
Control9=IDC_STATIC_TOTAL,static,1342308352
Control10=IDC_STATIC_PACKAGE,static,1342308352
Control11=IDC_STATIC_INSTALLING,static,1342308352
Control12=IDC_STATIC_INSTALLING_PACKAGE,static,1342308352
Control13=IDC_STATIC_PERCENT_UNIT,static,1342308352
Control14=IDC_STATIC_PERCENT_UNIT2,static,1342308352

[CLS:CWelcomePage]
Type=0
HeaderFile=welcomepage.h
ImplementationFile=welcomepage.cpp
BaseClass=CPropertyPage
LastObject=IDC_CHECK_ADMINISTRATOR
Filter=D
VirtualFilter=idWC

[DLG:IDD_PROPPAGE_ERROR (English (U.S.))]
Type=1
Class=CErrorPage
ControlCount=9
Control1=IDC_STATIC,static,1342181383
Control2=IDC_STATIC,static,1342308352
Control3=IDC_EDIT_ERROR,edit,1352665092
Control4=IDC_PROMPT,static,1342308352
Control5=IDC_LOGFILE,static,1342308480
Control6=IDC_CHECK_VIEW_LOGFILE,button,1342242819
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC_LOGNAME,static,1342308352
Control9=IDC_STATIC_HEADER,static,1342308352

[DLG:IDD_PROPPAGE_SUPPLEMENT (English (U.S.))]
Type=1
Class=CSupplementPage
ControlCount=29
Control1=IDC_CHECK_GS550,button,1476591619
Control2=IDC_CHECK_NTEMACS,button,1476460547
Control3=IDC_CHECK_TEXSHELL,button,1476460547
Control4=IDC_CHECK_NETPBM,button,1476460547
Control5=IDC_CHECK_PSUTILS,button,1476460547
Control6=IDC_CHECK_WINEDT,button,1476460547
Control7=IDC_CHECK_IMAGICK,button,1476460547
Control8=IDC_CHECK_GS60,button,1476460547
Control9=IDC_CHECK_FRENCH,button,1476460547
Control10=IDC_CHECK_TEXNICCENTER,button,1476460547
Control11=IDC_STATIC,static,1342181383
Control12=IDC_PROMPT,static,1476526080
Control13=IDC_STATIC_HEADER,static,1342308352
Control14=IDC_STATIC_LOCAL,static,1476526080
Control15=IDC_STATIC_NET_DOWNLOAD,static,1476526080
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC_PACKAGE_SIZE,static,1342308354
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC_TOTAL_SIZE,static,1342308354
Control21=IDC_STATIC,static,1342308352
Control22=IDC_STATIC,static,1342308352
Control23=IDC_STATIC_SPACE_AVAILABLE,static,1342308354
Control24=IDC_STATIC,static,1342308352
Control25=IDC_CHECK_PFE,button,1476460547
Control26=IDC_CHECK_WINSHELL,button,1476460547
Control27=IDC_CHECK_PERL2,button,1476460547
Control28=IDC_CHECK_XEMACS,button,1476460547
Control29=IDC_CHECK_LATEX2HTML,button,1476460547

[CLS:CSupplementPage]
Type=0
HeaderFile=SupplementPage.h
ImplementationFile=SupplementPage.cpp
BaseClass=CDialog
Filter=D
LastObject=CSupplementPage
VirtualFilter=dWC

[CLS:CErrorPage]
Type=0
HeaderFile=ErrorPage.h
ImplementationFile=ErrorPage.cpp
BaseClass=CDialog
Filter=W
VirtualFilter=dWC
LastObject=CErrorPage

[CLS:CRebootPage]
Type=0
HeaderFile=RebootPage.h
ImplementationFile=RebootPage.cpp
BaseClass=CDialog
Filter=D
LastObject=CRebootPage
VirtualFilter=dWC

[DLG:IDD_PROPPAGE_REBOOT (English (U.S.))]
Type=1
Class=CRebootPage
ControlCount=10
Control1=IDC_CHECK_DOC,button,1342242819
Control2=IDC_CHECK_LOG,button,1342242819
Control3=IDC_CHECK_REBOOT,button,1476460547
Control4=IDC_STATIC_MESSAGE,static,1342308352
Control5=IDC_STATIC_REBOOT,static,1476526080
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC_HEADER,static,1342308352
Control8=IDC_STATIC,static,1342181383
Control9=IDC_STATIC,static,1342177294
Control10=IDC_STATIC,static,1342181383

[CLS:CTreePackage]
Type=0
HeaderFile=TreePackage.h
ImplementationFile=TreePackage.cpp
BaseClass=CTreeCtrl
Filter=W
VirtualFilter=GWC
LastObject=CTreePackage

[DLG:IDD_PROPPAGE_CONFIG (English (U.S.))]
Type=1
Class=CConfigPage
ControlCount=14
Control1=IDC_STATIC_SUPP,static,1073872896
Control2=IDC_STATIC_PACKAGE,static,1342308352
Control3=IDC_STATIC_LSR,static,1073872896
Control4=IDC_STATIC_FORMAT,static,1073872896
Control5=IDC_STATIC_ENVIRONMENT,static,1073872896
Control6=IDC_STATIC_SHELL,static,1073872896
Control7=IDC_OK_SUPP,static,1073741838
Control8=IDC_OK_LSR,static,1073741838
Control9=IDC_OK_FORMAT,static,1073741838
Control10=IDC_OK_ENVIRONMENT,static,1073741838
Control11=IDC_OK_SHELL,static,1073741838
Control12=IDC_STATIC_HEADER,static,1342308352
Control13=IDC_PROMPT,static,1342308352
Control14=IDC_STATIC,static,1342181383

[CLS:CConfigPage]
Type=0
HeaderFile=ConfigPage.h
ImplementationFile=ConfigPage.cpp
BaseClass=CPropertyPage
Filter=D
VirtualFilter=idWC
LastObject=CConfigPage

[DLG:IDD_PROPPAGE_UNINSTALL (English (U.S.))]
Type=1
Class=CUninstallPage
ControlCount=14
Control1=IDC_STATIC_HEADER,static,1342308352
Control2=IDC_STATIC_MENU,static,1073872896
Control3=IDC_STATIC_PACKAGE,static,1342308352
Control4=IDC_STATIC_REGISTRY,static,1073872896
Control5=IDC_STATIC_FILES,static,1073872896
Control6=IDC_OK_MENU,static,1073741838
Control7=IDC_OK_REGISTRY,static,1073741838
Control8=IDC_OK_FILES,static,1073741838
Control9=IDC_PROMPT,static,1342308352
Control10=IDC_CHECK_LOG,button,1476460547
Control11=IDC_STATIC_RMDIR,static,1342308352
Control12=IDC_STATIC,static,1342181383
Control13=IDC_STATIC_ENVIRONMENT,static,1073872896
Control14=IDC_OK_ENVIRONMENT,static,1073741838

[CLS:CUninstallPage]
Type=0
HeaderFile=UninstallPage.h
ImplementationFile=UninstallPage.cpp
BaseClass=CPropertyPage
Filter=D
VirtualFilter=idWC
LastObject=CUninstallPage

[DLG:IDD_PROPPAGE_SOURCE (English (U.S.))]
Type=1
Class=CSourcePage
ControlCount=22
Control1=IDC_STATIC_HEADER,static,1342308352
Control2=IDC_STATIC,static,1342181383
Control3=IDC_STATIC,static,1342308352
Control4=IDC_RADIO_SOURCE_CDROM,button,1342177289
Control5=IDC_RADIO_SOURCE_ZIP,button,1342185481
Control6=IDC_RADIO_SOURCE_INTERNET,button,1342177289
Control7=IDC_STATIC,static,1342308864
Control8=IDC_BROWSE_LOCAL,button,1342242816
Control9=IDC_EDIT_SOURCE_DIR,edit,1350631552
Control10=IDC_STATIC,static,1342308864
Control11=IDC_BROWSE_REMOTE,button,1342242816
Control12=IDC_EDIT_REMOTE_DIR,edit,1350631552
Control13=IDC_CHECK_NETDOWNLOAD,button,1342242819
Control14=IDC_INTERNET_CONNECTION_IE5,button,1342177289
Control15=IDC_INTERNET_CONNECTION_DIRECT,button,1342177289
Control16=IDC_INTERNET_CONNECTION_PROXY,button,1207959561
Control17=IDC_EDIT_PROXY_ADDRESS,edit,1216413824
Control18=IDC_STATIC,static,1208091136
Control19=IDC_EDIT_PROXY_PORT,edit,1216413824
Control20=IDC_STATIC,static,1342181383
Control21=IDC_STATIC,static,1208091136
Control22=IDC_STATIC,static,1342181383

[DLG:IDD_DIALOG_TPM_PROGRESS]
Type=1
Class=CTpmProgress
ControlCount=4
Control1=IDCANCEL,button,1342242816
Control2=IDC_PROGRESS_TPM,msctls_progress32,1342177281
Control3=IDC_STATIC_TPM_NAME,static,1342308352
Control4=IDOK,button,1073807360

[CLS:CTpmProgress]
Type=0
HeaderFile=TpmProgress.h
ImplementationFile=TpmProgress.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CTpmProgress

[CLS:CSourcePage]
Type=0
HeaderFile=sourcepage.h
ImplementationFile=sourcepage.cpp
BaseClass=CPropertyPage
LastObject=CSourcePage

[DLG:IDD_PROPPAGE_GETTPM (English (U.S.))]
Type=1
Class=CGetTPMPage
ControlCount=12
Control1=CG_IDC_PROGDLG_PROGRESS_DOWNLOAD,msctls_progress32,1342177281
Control2=IDC_STATIC_PERCENT_DOWNLOAD,static,1342308352
Control3=CG_IDC_PROGDLG_PROGRESS_READ,msctls_progress32,1342177281
Control4=IDC_STATIC,static,1342181383
Control5=IDC_STATIC_HEADER,static,1342308352
Control6=IDC_STATIC,static,1342181383
Control7=IDC_PROMPT,static,1342308352
Control8=IDC_STATIC_READING,static,1342308352
Control9=IDC_STATIC_DOWNLOADING,static,1342308352
Control10=IDC_STATIC_PERCENT_UNIT,static,1073872896
Control11=IDC_STATIC_TPMZIP,static,1342308352
Control12=IDC_STATIC_TPMREAD,static,1342308352

[CLS:CGetTPMPage]
Type=0
HeaderFile=GetTPMPage.h
ImplementationFile=GetTPMPage.cpp
BaseClass=CPropertyPage
Filter=D
LastObject=CG_IDC_PROGDLG_PROGRESS_DOWNLOAD
VirtualFilter=dWC

[CLS:CTransBmp]
Type=0
HeaderFile=TransBmp.h
ImplementationFile=TransBmp.cpp
BaseClass=generic CWnd
Filter=W
LastObject=CTransBmp

[DLG:IDD_DIALOG_LIST_CHOICE]
Type=1
Class=CListChoiceDlg
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC_DESCRIPTION,static,1342308352
Control4=IDC_COMBO_LIST_CHOICE,combobox,1344340289
Control5=IDC_LISTCHOICE_RADIO1,button,1342177289
Control6=IDC_LISTCHOICE_RADIO2,button,1342177289
Control7=IDC_LISTCHOICE_RADIO3,button,1342177289

[CLS:CListChoiceDlg]
Type=0
HeaderFile=ListChoiceDlg.h
ImplementationFile=ListChoiceDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_LISTCHOICE_RADIO3
VirtualFilter=dWC

