// EnvVar.h: interface for the EnvVar class.
//
//////////////////////////////////////////////////////////////////////

#include <afxtempl.h>

#if !defined(AFX_ENVVAR_H__DFD3283C_7E46_414D_929A_5B102BF2F978__INCLUDED_)
#define AFX_ENVVAR_H__DFD3283C_7E46_414D_929A_5B102BF2F978__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

using namespace std;

class Environment
{
private:
    CTypedPtrMap<CMapStringToPtr, CString, CString *> m_mEnv;
    bool m_bIsModified;

public:
    BOOL GetValue(const CString &sVariable, CString &sValue);
    BOOL SetValue(const CString &sVariable, const CString &sValue);
    void CleanupPath(CString &sPath);
    Environment();
    virtual ~Environment();
    BOOL Commit();
    BOOL CommitW9x(BOOL bAdd);
    void DeleteFromPath(CString &sPath, CString &sElt);
    BOOL Restore(char *vars[]);
    BOOL IsModified() { return m_bIsModified; };
};

#endif // !defined(AFX_ENVVAR_H__DFD3283C_7E46_414D_929A_5B102BF2F978__INCLUDED_)
/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

