// TeXSetupCommandLineInfo.h: interface for the CTeXSetupCommandLineInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXSETUPCOMMANDLINEINFO_H__724B515D_AA8D_400A_86BB_227DF7C0B783__INCLUDED_)
#define AFX_TEXSETUPCOMMANDLINEINFO_H__724B515D_AA8D_400A_86BB_227DF7C0B783__INCLUDED_

#include <afxwin.h>

#include "geturl.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTeXSetupCommandLineInfo : public CCommandLineInfo  
{
public:
    inline CTeXSetupCommandLineInfo () :
        m_bUninstall(false),
        m_bQuickInstall (false),
        m_bDryRun (false),
        m_bAutomaticReboot (false), 
        m_bWithSource (false),
        m_bWithDoc(true),
        m_bXEmTeX(false),
        m_bAddPackage(false),
        m_bMaintenance(false),
        m_uiNetMethod(NetIOUnavail),
        m_sRemoteSourceDir("ftp://ftp.dante.de/pub/fptex/current/")
    { }
    virtual ~CTeXSetupCommandLineInfo();
public:
    bool m_bUninstall;
    bool m_bQuickInstall;
    bool m_bDryRun;
    bool m_bAutomaticReboot;
    bool m_bWithSource;
    bool m_bWithDoc;
    bool m_bAddPackage;
    bool m_bMaintenance;
    bool m_bXEmTeX;
    NetIOType m_uiNetMethod;
    CString m_sInstallRootDir;
    CString m_sTexmfMainDir;
    CString m_sTexmfLocalDir;
    CString m_sTexmfExtraDir;
    CString m_sTexmfHomeDir;
    CString m_sVarTexmfDir;
    CString m_sVarTexFontsDir;
    CString m_sLocalSourceDir;
    CString m_sRemoteSourceDir;
    CString m_sFolderName;
    CString m_sPackage;
    CString m_sScheme;
};

extern CTeXSetupCommandLineInfo cmdinfo;

#endif // !defined(AFX_TEXSETUPCOMMANDLINEINFO_H__724B515D_AA8D_400A_86BB_227DF7C0B783__INCLUDED_)

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

