// ShellLinkData.cpp: Shell objects for fpTeX
// Taken from MikTeX.

#include "stdafx.h"
#include "stdafx.h"

#include "TeXSetup.h"
#include "ShellLinkData.h"

const ShellLinkData g_ShellLinks[] = {

    {
        0,
        IDS_LOCAL_GUIDE,
        0,
    },

    {
        0,
        IDS_LATEX2E_REFERENCE,
        0,
    },

    {
        0,
        IDS_DVI_VIEWER,
        "bin\\win32\\windvi.exe",
        LD_USESHOWCMD,
        0,
        0,
        0,
        0,
        0,
        SW_SHOWNORMAL,
        0,
    },

    {
        0,
        IDS_UNINSTALL,
        "@@texsetup@@",
        LD_USEARGS | LD_USESHOWCMD | LD_USEICON,
        0,
        "--uninstall",
        "@@texsetup@@",
        1,
        0,
        SW_SHOWNORMAL,
        0,
    },

    {
        IDS_MAINTENANCE,
        IDS_ADD_PACKAGE,
        "@@texsetup@@",
        LD_USEARGS | LD_USESHOWCMD,
        0,
        "--add-package --local-source-dir=@@sourcedir@@",
        0,
        0,
        0,
        SW_SHOWNORMAL,
        0,
    },

    {
        IDS_MAINTENANCE,
        IDS_REFRESH_FILENAME_DATABASE,
        "bin\\win32\\mktexlsr.exe",
        LD_USEARGS | LD_USESHOWCMD,
        0,
        "",
        0,
        0,
        0,
        SW_SHOWNORMAL,
        0,
    },

    {
        IDS_MAINTENANCE,
        IDS_CREATE_LATEX_FORMAT_FILE,
        "bin\\win32\\fmtutil.exe",
        LD_USEARGS | LD_USESHOWCMD,
        0,
        "--byfmt=latex --dolinks",
        0,
        0,
        0,
        SW_SHOWNORMAL,
        0,
    },

    {
        IDS_MAINTENANCE,
        IDS_CREATE_MISSING_FORMAT_FILES,
        "bin\\win32\\fmtutil.exe",
        LD_USEARGS | LD_USESHOWCMD,
        0,
        "--missing --dolinks",
        0,
        0,
        0,
        SW_SHOWNORMAL,
        0,
    },

    {
        IDS_MAINTENANCE,
        IDS_CREATE_ALL_FORMAT_FILES,
        "bin\\win32\\fmtutil.exe",
        LD_USEARGS | LD_USESHOWCMD,
        0,
        "--all --dolinks",
        0,
        0,
        0,
        SW_SHOWNORMAL,
        0,
    },

    {
        IDS_HELP,
        IDS_LOCAL_GUIDE,
        "texmf\\doc\\tldoc\\english\\live.html",
        LD_USESHOWCMD,
        0,
        0,
        0,
        0,
        0,
        SW_SHOWNORMAL,
        0,
    },

    //   {
    //     IDS_HELP,
    //     IDS_FAQ,
    //     "temxf\\doc\\html\\fptex\\fptex#faq.html",
    //     LD_USESHOWCMD,
    //     0,
    //     0,
    //     0,
    //     0,
    //     0,
    //     SW_SHOWNORMAL,
    //     0,
    //   },

    {
        IDS_HELP,
        IDS_REMOTE_GUIDE,
        "http://www.fptex.org/wwwfptex.html",
        LD_USESHOWCMD,
        0,
        0,
        0,
        0,
        0,
        SW_SHOWNORMAL,
        0,
    },

    {
        IDS_HELP,
        IDS_TUG_GUIDE,
        "http://www.tug.org/texlive/",
        LD_USESHOWCMD,
        0,
        0,
        0,
        0,
        0,
        SW_SHOWNORMAL,
        0,
    },

    {
        IDS_HELP,
        IDS_LATEX2E_REFERENCE,
        "bin\\win32\\latex2e.hlp",
        LD_USESHOWCMD,
        0,
        0,
        0,
        0,
        0,
        SW_SHOWNORMAL,
        0,
    },

    //   {
    //     IDS_HELP,
    //     IDS_RELEASE_NOTES,
    //     "doc\\miktex\\RelNotes.txt",
    //     LD_USESHOWCMD,
    //     0,
    //     0,
    //     0,
    //     0,
    //     0,
    //     SW_SHOWNORMAL,
    //     0,
    //   },

};

size_t g_nShellLinks = sizeof(g_ShellLinks) / sizeof(g_ShellLinks[0]);

#if 0

const ShellLinkData g_TeXShellLinks[] = {


    {
        0,
        IDS_TEXSHELL_HELP,
        0,
    },
    {
        0,
        IDS_TEXSHELL_EDITOR,
        "bin\\win32\\TeXShell.exe",
        LD_USESHOWCMD,
        0,
        0,
        0,
        0,
        0,
        SW_SHOWNORMAL,
        0,
    },
    {
        IDS_HELP,
        IDS_TEXSHELL_HELP,
        "bin\\win32\\TeXShell.hlp",
        LD_USESHOWCMD,
        0,
        0,
        0,
        0,
        0,
        SW_SHOWNORMAL,
        0,
    },
};

size_t g_nTeXShellLinks = sizeof(g_TeXShellLinks) / sizeof(g_TeXShellLinks[0]);

const ShellLinkData g_PFELinks[] = {

    {
        0,
        IDS_PFE_HELP,
        0,
    },
    {
        0,
        IDS_PFE_EDITOR,
        "bin\\win32\\pfe32.exe",
        LD_USESHOWCMD,
        0,
        0,
        0,
        0,
        0,
        SW_SHOWNORMAL,
        0,
    },
    {
        IDS_HELP,
        IDS_PFE_HELP,
        "texmf\\doc\\pfe\\pfe4tex.txt",
        LD_USESHOWCMD,
        0,
        0,
        0,
        0,
        0,
        SW_SHOWNORMAL,
        0,
    },

};

size_t g_nPFELinks = sizeof(g_PFELinks) / sizeof(g_PFELinks[0]);
#endif

const ShellLinkData g_XEmacsShellLinks[] = {

    {
        0,
        IDS_XEMACS_HELP,
        0,
    },
    {
        0,
        IDS_XEMACS_EDITOR,
        "bin\\win32\\xemacs.exe",
        LD_USESHOWCMD,
        0,
        0,
        0,
        0,
        0,
        SW_SHOWNORMAL,
        0,
    },
    {
        IDS_HELP,
        IDS_XEMACS_HELP,
        "http://www.xemacs.org/",
        LD_USESHOWCMD,
        0,
        0,
        0,
        0,
        0,
        SW_SHOWNORMAL,
        0,
    },

};

size_t g_nXEmacsShellLinks = sizeof(g_XEmacsShellLinks) / sizeof(g_XEmacsShellLinks[0]);


/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

