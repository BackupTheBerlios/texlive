#if !defined(AFX_SHELLLINKDATA_H__13347E23_B86A_11D2_86A6_000000000000__INCLUDED_)
#define AFX_SHELLLINKDATA_H__13347E23_B86A_11D2_86A6_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct ShellLinkData  
{
public:

#define LD_USEDESC     0x00000001
#define LD_USEARGS     0x00000002
#define LD_USEICON     0x00000004
#define LD_USEWORKDIR  0x00000008
#define LD_USEHOTKEY   0x00000010
#define LD_USESHOWCMD  0x00000020

  UINT nSubFolderID;
  UINT nNameID;
  LPTSTR pszPathname;
  DWORD fdwFlags;
  LPTSTR pszDesc;
  LPTSTR pszArgs;
  LPTSTR pszIconPath;
  int nIconIndex;
  LPTSTR pszWorkingDir;
  int nShowCmd;
  WORD wHotkey;
};

extern const ShellLinkData g_ShellLinks[];
extern const ShellLinkData g_TeXShellLinks[];
extern const ShellLinkData g_PFELinks[];
extern const ShellLinkData g_NTEmacsLinks[];
extern const ShellLinkData g_XEmacsShellLinks[];
extern size_t g_nShellLinks;
extern size_t g_nTeXShellLinks;
extern size_t g_nPFELinks;
extern size_t g_nNTEmacsLinks;
extern size_t g_nXEmacsShellLinks;

#endif // !defined(AFX_SHELLLINKDATA_H__13347E23_B86A_11D2_86A6_000000000000__INCLUDED_)
/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

