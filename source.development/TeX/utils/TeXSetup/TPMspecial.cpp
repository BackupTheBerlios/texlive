#include "Win32Util.h"
#include "Registry.h"
#include "FileDirectory.h"
#include "KeyValFile.h"
#include "TPM.h"
#include "TeXInstall.h"

#if 0
bool TPM::InstallWinEdt()
{
    bool bRet = IsSelected();
    
    if (bRet) {
        CString sFile;
        // First the program
        //	if (GrabInternetFile(m_saRemoteFiles[0], sFile)) {
        if (GetSpecialFile(m_saRemoteFiles[0], sFile)) {
            bRet = bRet && RunProcess(sFile, false, true, true); // no logging, fails under W98, wait, show
#if 0
            CString sTempWinEdt = ConcatPath(g_sTempPath, "WinEdt");
            UnzipFile(sFile, sTempWinEdt);
            DeleteFile(sFile);
            // Install Shield is 16bit app ...
            system((LPCTSTR)ConcatPath(sTempWinEdt, "setup.exe"));
            // Find a window from class InstallShield_Win and wait on it
            while (FindWindow("InstallShield_Win", NULL) != NULL) {
                Sleep(1000);
            }
#endif
        }
    }
#if 0
    //
    CString sWinEdtDir;
    CRegistry theRegistry;
    if (theRegistry.Connect(HKEY_CURRENT_USER) == FALSE) {
        theLog("Registry: can't connect to HKEY_CURRENT_USER (Error %d)\n",
               GetLastError());
        bRet = false;
        goto exit;
    }
    if (theRegistry.Open("Software\\WinEdt") == FALSE) {
        theLog("Registry: can't open key 'Software\\WinEdt' (Error %d)\n",
               GetLastError());
        bRet = false;
        goto exit;
    }
    if (theRegistry.GetValue("Install Root", sWinEdtDir) == FALSE) {
        theLog("Registry: can't get 'Install Root' value (Error %d)\n",
               GetLastError());
        bRet = false;
        goto exit;
    }
    theRegistry.Close();
    //	if (GrabInternetFile(m_saRemoteFiles[1], sFile)) {
    if (GetSpecialFile(m_saRemoteFiles[1], sFile)) {
        bRet = bRet && UnzipFile(sFile, sWinEdtDir);
        DeleteFile(sFile);
    }
    //	if (GrabInternetFile(m_saRemoteFiles[2], sFile)) {
    if (GetSpecialFile(m_saRemoteFiles[2], sFile)) {
        bRet = bRet &&  UnzipFile(sFile, sWinEdtDir);
        DeleteFile(sFile);
    }
#endif

    bRet = bRet && InstallTPM();

    m_bInstalled = true;

    if (bRet) {
        theLog("Package %s has been installed successfully.\n", m_sKey);
    }
    else {
        theLog("!!!Package %s has not been installed successfully.\n", m_sKey);
    }

 exit: ;
    return m_bInstalled;
}

bool TPM::InstallWinShell()
{
    bool bRet = IsSelected();
    if (bRet) {
        CString sFile = ConcatPath(g_sLocalSourceDir, m_saRemoteFiles[0]);
        if (FileExists(sFile)) {
            bRet = bRet && RunProcess(sFile, false, true, true); // no logging, fails under W98, wait, show
        }
        // No bug fix for release 2.2
#if 0
        // Apply the bug fix zip file
        CString sWinShell;
        CRegistry theRegistry;
        if (theRegistry.Connect(HKEY_LOCAL_MACHINE) == FALSE) {
            theLog("Registry: can't connect to HKEY_LOCAL_MACHINE (Error %d)\n",
                   GetLastError());
            return false;
        }
        if (theRegistry.Open(TEXT("SOFTWARE\\Classes\\tex_auto_file\\DefaultIcon"),
                             (CRegistry::CreatePermissions)CRegistry::permissionRead) == FALSE ) {
            theLog("Registry: can't open WinShell.Document\\DefaultIcon key (Error %d)\n",
                   GetLastError());
            theRegistry.Close();
            return false;
        }
        theRegistry.GetValue("", sWinShell);
        // it might be quoted
        sWinShell.Remove(TEXT('\"'));
        ::GetParentDirectory(sWinShell);
        theRegistry.Close();
        theLog("WinShell has been located in %s and the bug fixes will be applied.\n", (LPCTSTR)sWinShell);
        
        for (int i = 1; i < m_saRemoteFiles.GetSize(); i++) {
            CString sUrl, sTmpBugFix, sBugFix = m_saRemoteFiles[i];
            bool bRemoveTemp = false;
            
            sUrl = ConcatPath(g_sLocalSourceDir, sBugFix);
            if (FileExists(sUrl)) {
                sBugFix = sUrl;
                theLog("Special file %s has been found on local directory.\n", (LPCTSTR)sBugFix);
            }
            else {
                // Try with the remote directory
                CString sUrl = ConcatPath(g_sRemoteSourceDir, sBugFix, '/');
                if (g_uiNetMethod == NetIOUnavail) {
                    theLog("Error: No net access allowed, cannot grab %s from the Internet!!!\n", (LPCTSTR)sBugFix);
                    bRet = false;
                }
                else if (GrabInternetFile(sUrl, sTmpBugFix)) {
                    theLog("Special file %s has been grabbed from the Internet.\n", (LPCTSTR)sBugFix);
                    bRemoveTemp = true;
                }
                else {
                    theLog("Error: Failed to grab %s from the Internet!!!\n", (LPCTSTR)sUrl);
                    bRet = false;
                }
            }

            bRet = bRet && UnzipFile(sTmpBugFix, sWinShell);
            m_bInstalled = bRet;
            
            if (bRemoveTemp)
                DeleteFile(sBugFix);
        }
#endif
    }

    bRet = bRet && InstallTPM();
    m_bInstalled = true;

    if (bRet) {
        theLog("Package %s has been installed successfully.\n", m_sKey);
    }
    else {
        theLog("!!!Package %s has not been installed successfully.\n", m_sKey);
    }

    return bRet;
}

bool TPM::ConfigurePFE()
{
    bool bRet = IsInstalled();

    if (IsInstalled()) {
        bRet = CreateShellObjects(g_PFELinks, g_nPFELinks, g_sInstallRootDir);
        if (bRet) {
            theLog("Shell objects for %s have been created successfully.\n", m_sKey);
        }
        else {
            theLog("!!!Shell objects for %s have not been created successfully.\n", m_sKey);
        }
    }
    return bRet;
}

bool TPM::ConfigureNTEmacs()
{
    bool bRet = IsInstalled();

    if (IsInstalled()) {
        // Run addpm <location>
        CString sNTEmacs = g_sInstallRootDir;
        GetParentDirectory(sNTEmacs);
        sNTEmacs = ConcatPath(sNTEmacs, "NTEmacs");
        CString sAddpm = ConcatPath(sNTEmacs, "bin\\addpm.exe"); 
        CString sCmd = GetSafePathName(sAddpm) + " " + GetSafePathName(sNTEmacs);
        bRet = bRet && RunProcess(sCmd, false, false, true); // no logging, fails under W98, no wait, hide

        // Copy the GnuServ .exe files to some location in the PATH
        CString sGnuFile, sDestFile;
        sGnuFile = ConcatPath(sNTEmacs, "bin\\gnuclient.exe");
        sDestFile = ConcatPath(g_sInstallRootDir, "bin\\win32\\gnuclient.exe");
        SafeCopyFile(sGnuFile, sDestFile, false, true); // no backup, log
        sGnuFile = ConcatPath(sNTEmacs, "bin\\gnuclientw.exe");
        sDestFile = ConcatPath(g_sInstallRootDir, "bin\\win32\\gnuclientw.exe");
        SafeCopyFile(sGnuFile, sDestFile, false, true); // no backup, log
        sGnuFile = ConcatPath(sNTEmacs, "bin\\gnuserv.exe");
        sDestFile = ConcatPath(g_sInstallRootDir, "bin\\win32\\gnuserv.exe");
        SafeCopyFile(sGnuFile, sDestFile, false, true); // no backup, log
    }
    return bRet;
}

bool TPM::ConfigurePerl()
{
    bool bRet = IsInstalled();

    if (IsInstalled()) {
        CString sPerlDir = g_sInstallRootDir;
        GetParentDirectory(sPerlDir);
        sPerlDir = ConcatPath(sPerlDir, "Perl\\bin") 
            + ";" + ConcatPath(sPerlDir, "Tcl\\bin");
        CString sPath;
        theEnvironment.GetValue("PATH", sPath);
        if (sPath.IsEmpty()) {
            theEnvironment.SetValue("PATH", "%PATH%;" + GetSafePathName(sPerlDir, false));
        }
        else {
            theEnvironment.SetValue("PATH", (sPath + ";") + GetSafePathName(sPerlDir, false));
        }
    }

    return bRet;
}

bool TPM::ConfigureNetPBM()
{
    bool bRet = IsInstalled();

    if (IsInstalled()) {
        CString sNetPBMDir = g_sInstallRootDir;
        GetParentDirectory(sNetPBMDir);
        sNetPBMDir = ConcatPath(sNetPBMDir, "netpbm\\bin");
        CString sPath;
        theEnvironment.GetValue("PATH", sPath);
        if (sPath.IsEmpty()) {
            theEnvironment.SetValue("PATH", "%PATH%;" + GetSafePathName(sNetPBMDir, false));
        }
        else {
            theEnvironment.SetValue("PATH", (sPath + ";") + GetSafePathName(sNetPBMDir, false));
        }
    }
    return bRet;
}

bool TPM::ConfigureIMagick()
{
    bool bRet = IsInstalled();

    if (IsInstalled()) {
        CString sIMagickDir = g_sInstallRootDir;
        GetParentDirectory(sIMagickDir);
        sIMagickDir = ConcatPath(sIMagickDir, "ImageMagick");
        CString sPath;
        theEnvironment.GetValue("PATH", sPath);
        if (sPath.IsEmpty()) {
            theEnvironment.SetValue("PATH", "%PATH%;" + GetSafePathName(sIMagickDir, false));
        }
        else {
            theEnvironment.SetValue("PATH", (sPath + ";") + GetSafePathName(sIMagickDir, false));
        }
        theEnvironment.SetValue("MAGICK_HOME", (LPCTSTR)sIMagickDir);
    }
    return bRet;
}

bool TPM::ConfigureGnuUtils()
{
    bool bRet = IsInstalled();

    if (IsInstalled()) {
        CString sGnuUtilsDir = g_sInstallRootDir;
        GetParentDirectory(sGnuUtilsDir);
        sGnuUtilsDir = ConcatPath(sGnuUtilsDir, "Gnu\\bin");
        CString sPath;
        theEnvironment.GetValue("PATH", sPath);
        if (sPath.IsEmpty()) {
            theEnvironment.SetValue("PATH", "%PATH%;" + GetSafePathName(sGnuUtilsDir, false));
        }
        else {
            theEnvironment.SetValue("PATH", (sPath + ";") + GetSafePathName(sGnuUtilsDir, false));
        }
    }
    return bRet;
}

bool TPM::ConfigureTeXShell()
{
    bool bRet = IsInstalled();
    if (IsInstalled()) {
        bRet = bRet && CreateShellObjects(g_TeXShellLinks, g_nTeXShellLinks, g_sInstallRootDir);
    }
    return bRet;
}

bool TPM::ConfigureXEmacs()
{
    bool bRet = IsInstalled();

    if (IsInstalled()) {
    }
    return bRet;
}

#endif

static bool NeedToEditMapFileList(TPMSet &vMap)
{
    bool bRet = false;

    // Check if we need to change the current list of map files
    // First if we are doing a full installation
    if (g_eInstallType == INSTALL_FULL) {
        bRet = true;
    }
    // Second, compare the list of installed package with the
    // new packages.
    else if (g_eInstallType == INSTALL_MAINTENANCE) {
        POSITION pos;
        CString sKey, sTPMKey;
        TPM *tpm = NULL, *tpmSource = NULL, *tpmLocal = NULL;
        pos = vMap.GetStartPosition();
        while (pos) {
            vMap.GetNextAssoc(pos, sKey, tpm);
            sTPMKey = tpm->GetKey();
            if (g_vSourceTPM.Lookup(sTPMKey, tpmSource)
                && tpmSource->IsInstalled()) {
                // This one has been installed
                // need to rewrite because if the package is newer
                // the list might have changed
                bRet = true;
                break;
            }
        }
    }
    return bRet;
}

bool TPM::ConfigureDvipdfm()
{
    bool bRet = IsInstalled();
    CString src, dst;
    dst = ConcatPath(g_sVarTexmf, "dvipdfm\\config\\config");
    if (IsInstalled()) {
        src = ConcatPath(g_sTexmfMain, "dvipdfm\\config\\config-win32", '\\');
        bRet = bRet && SafeCopyFile(src, dst, true, true);
    }
    if (NeedToEditMapFileList(g_vDvipdfmMap)) {
        theLog("Dvipdfm configuration: selecting installed map files.\n");
        // Configuring map files
        KeyValFile kConfig;
        if (kConfig.Open(dst)) {
            CString sMapLine;
            kConfig.RawReplace("f ", "", true, 0);
            // 
            POSITION pos;
            CString sKey, sTPMKey;
            TPM *tpm, *tpmSource, *tpmLocal;
            pos = g_vDvipdfmMap.GetStartPosition();
            while (pos) {
                g_vDvipdfmMap.GetNextAssoc(pos, sKey, tpm);
                // Weird ! we need to be sure we are talking to the ones 
                // in the g_sSourceTPM array
                sTPMKey = tpm->GetKey();
                if ((g_vSourceTPM.Lookup(sTPMKey, tpmSource) && tpmSource->IsInstalled())
                    || (g_vInstalledTPM.Lookup(sTPMKey, tpmLocal))) {
                    sMapLine = "f " + sKey;
                    kConfig.RawReplace("", sMapLine , true);
                    theLog("Adding `%s'\n", sMapLine);
                }
            }
            kConfig.Commit();
            kConfig.Close();
            theLog("Closing %s.\n", dst);
        }
        else {
            theLog("!!!Failed to configure map files for %s.\n", m_sKey);
        }
    }
    return bRet;
}

bool TPM::ConfigureDvips()
{
    bool bRet = IsInstalled();
    CString src, dst;
    src = ConcatPath(g_sTexmfMain, "dvips\\config\\config.ps");
    dst = ConcatPath(g_sVarTexmf, "dvips\\config\\config.ps");
    if (IsInstalled()) {
        bRet = bRet && SafeCopyFile(src, dst, true, true);
    }
    if (NeedToEditMapFileList(g_vDvipsMap)) {
        theLog("Dvipsk configuration: selecting installed map files.\n");
        // Configuring map files
        KeyValFile kConfig;
        if (kConfig.Open(dst)) {
            CString sMapLine;
            kConfig.RawReplace("p +", "", true, 0);
            // 
            POSITION pos;
            CString sKey, sTPMKey;
            TPM *tpm, *tpmSource, *tpmLocal;
            pos = g_vDvipsMap.GetStartPosition();
            while (pos) {
                g_vDvipsMap.GetNextAssoc(pos, sKey, tpm);
                // Weird ! we need to be sure we are talking to the ones 
                // in the g_sSourceTPM array
                sTPMKey = tpm->GetKey();
                if ((g_vSourceTPM.Lookup(sTPMKey, tpmSource) && tpmSource->IsInstalled())
                    || (g_vInstalledTPM.Lookup(sTPMKey, tpmLocal))) {
                    sMapLine = "p +" + sKey;
                    kConfig.RawReplace("", sMapLine , true);
                    theLog("Adding `%s'\n", sMapLine);
                }
            }
            kConfig.Commit();
            kConfig.Close();
            theLog("Closing %s.\n", dst);
        }
        else {
            theLog("!!!Failed to configure map files for %s.\n", m_sKey);
        }
    }
    return bRet;
}

bool TPM::ConfigurePdftex()
{
    bool bRet = IsInstalled();
    CString src, dst;
    src = ConcatPath(g_sTexmfMain, "pdftex\\config\\pdftex.cfg");
    dst = ConcatPath(g_sVarTexmf, "pdftex\\config\\pdftex.cfg");
    if (IsInstalled()) {
        bRet = bRet && SafeCopyFile(src, dst, true, true);
    }
    if (NeedToEditMapFileList(g_vPdfTeXMap)) {
        theLog("PdfTeX configuration: selecting installed map files.\n");
        // Configuring map files
        KeyValFile kConfig;
        if (kConfig.Open(dst)) {
            CString sMapLine;
            // Remove all map lines
            kConfig.RawReplace("map +", "", true, 0);
            // 
            POSITION pos;
            CString sKey, sTPMKey;
            TPM *tpm, *tpmSource, *tpmLocal;
            pos = g_vPdfTeXMap.GetStartPosition();
            while (pos) {
                g_vPdfTeXMap.GetNextAssoc(pos, sKey, tpm);
                // Weird ! we need to be sure we are talking to the ones 
                // in the g_sSourceTPM array
                sTPMKey = tpm->GetKey();
                if ((g_vSourceTPM.Lookup(sTPMKey, tpmSource) && tpmSource->IsInstalled())
                    || (g_vInstalledTPM.Lookup(sTPMKey, tpmLocal))) {
                    sMapLine = "map +" + sKey;
                    kConfig.RawReplace("", sMapLine , true);
                    theLog("Adding `%s'\n", sMapLine);
                }
            }
            kConfig.Commit();
            kConfig.Close();
            theLog("Closing %s.\n", dst);
        }
        else {
            theLog("!!!Failed to configure map files for %s.\n", m_sKey);
        }
    }
    return bRet;
}

bool TPM::ConfigureTeX4ht()
{
    bool bRet = IsInstalled();
    if (IsInstalled()) {
        // Special case for tex4ht.env
        // No need to edit it after that. I won't bother if people prefer
        // to use IMagick.
        CString src, dst;
        CString sTeX4htCfg("tex4ht-netpbm-win32.env");
        TPM *tpmIMagick = PackageLookup("support/imagick");
        if (tpmIMagick && tpmIMagick->IsSelected()) 
            sTeX4htCfg = "tex4ht-imagick-win32.env";

        // It should have been copied there.
        src = ConcatPath(g_sTexmfMain, ConcatPath("tex4ht\\base", sTeX4htCfg), '\\');
        dst = ConcatPath(g_sVarTexmf, "tex4ht\\base\\tex4ht.env");
        bRet = bRet && SafeCopyFile(src, dst, true, true);
        if (FileExists(dst)){
            // Be safe in locating convert.exe !
            if (sTeX4htCfg.Find("imagick") > -1) {
                CString sIMagickDir = g_sInstallRootDir;
                GetParentDirectory(sIMagickDir);
                ConcatPath(sIMagickDir, "ImageMagick");	  
                KeyValFile kTeX4ht;
                kTeX4ht.Open((LPCTSTR)dst);
                bRet = bRet && kTeX4ht.RawReplace("@imbindir@", sIMagickDir);
                bRet = bRet && kTeX4ht.Commit();
                kTeX4ht.Close();
            }
        }
    }
    return bRet;
}

bool TPM::ConfigureTeXBasic()
{
    static const char *g_confFiles[] = {
        "web2c\\texmf.cnf",
        "web2c\\mktex.cnf",
        "web2c\\fmtutil.cnf",
        "web2c\\updmap.cfg"
    };

    bool bRet = IsInstalled();

    if (IsInstalled()) {
        ASSERT(! g_sInstallRootDir.IsEmpty());
        ASSERT(! g_sTexmfMain.IsEmpty());
    
        CString src, dst, sTmp;
        CString sRootDir = g_sInstallRootDir;
        
        // Copy the configuration files
        for (int i = 0; i < sizeof(g_confFiles)/sizeof(g_confFiles[0]); i++) {
            src = ConcatPath(g_sTexmfMain, g_confFiles[i]);
            dst = ConcatPath(g_sVarTexmf, g_confFiles[i]);
            if (FileExists(src)){
                SafeCopyFile(src, dst, true, true);
            }
        }

        if (! g_bDryRun) {
            // and edit $VARTEXMF/web2c/texmf.cnf
            KeyValFile kTexmfCnf;
            kTexmfCnf.Open(ConcatPath(g_sVarTexmf, "web2c\\texmf.cnf"));
      
            sRootDir.Replace('\\', '/');
            if (sRootDir.Right(1) != "/")
                sRootDir += "/";
      
            sTmp = g_sTexmfMain;
            sTmp.Replace('\\', '/');
            if (sTmp.Find((LPCTSTR)sRootDir) == 0) {
                sTmp.Replace((LPCTSTR)sRootDir, "$SELFAUTOPARENT/");
            }
            kTexmfCnf.Replace("TEXMFMAIN", sTmp);
      
            if (g_sTexmfLocal.IsEmpty()) {
                kTexmfCnf.Comment("TEXMFLOCAL");
            }
            else {
                kTexmfCnf.Uncomment("TEXMFLOCAL");
                sTmp = g_sTexmfLocal;
                sTmp.Replace('\\', '/');
                if (sTmp.Find((LPCTSTR)sRootDir) == 0) {
                    sTmp.Replace((LPCTSTR)sRootDir, "$SELFAUTOPARENT/");
                }
                kTexmfCnf.Replace("TEXMFLOCAL", sTmp);
                if (! ::DirectoryExists(g_sTexmfLocal))
                    ::CreateDirectoryPath(g_sTexmfLocal);
            }
      
            if (g_sTexmfHome.IsEmpty()) {
                kTexmfCnf.Comment("HOMETEXMF");
            }
            else {
                CString sHome;
                kTexmfCnf.Uncomment("HOMETEXMF");
                sTmp = g_sTexmfHome;
                sTmp.Replace('\\', '/');
                if (FindHomeDir(sHome)) {
                    if (sHome.Right(1) != "/" && sHome.Right(1) != "\\")
                        sHome += "/";
                    sHome.Replace('\\', '/');
                    if (sTmp.Find((LPCTSTR)sHome) == 0) {
                        sTmp.Replace((LPCTSTR)sHome, "$HOME/");
                    }
                }
                kTexmfCnf.Replace("HOMETEXMF", sTmp);
                if (! ::DirectoryExists(g_sTexmfHome))
                    ::CreateDirectoryPath(g_sTexmfHome);
            }
      
            if (g_sTexmfExtra.IsEmpty()) {
                kTexmfCnf.Comment("TEXMFEXTRA");
            }
            else {
                kTexmfCnf.Uncomment("TEXMFEXTRA");
                sTmp = g_sTexmfExtra;
                sTmp.Replace('\\', '/');
                if (sTmp.Find((LPCTSTR)sRootDir) == 0) {
                    sTmp.Replace((LPCTSTR)sRootDir, "$SELFAUTOPARENT/");
                }
                kTexmfCnf.Replace("TEXMFEXTRA", sTmp);
	
                if (! ::DirectoryExists(g_sTexmfExtra))
                    ::CreateDirectoryPath(g_sTexmfExtra);
            }
      
            if (g_sVarTexmf.IsEmpty()) {
                kTexmfCnf.Comment("VARTEXMF");
            }
            else {
                kTexmfCnf.Uncomment("VARTEXMF");
                sTmp = g_sVarTexmf;
                sTmp.Replace('\\', '/');
                if (sTmp.Find((LPCTSTR)sRootDir) == 0) {
                    sTmp.Replace((LPCTSTR)sRootDir, "$SELFAUTOPARENT/");
                }
                kTexmfCnf.Replace("VARTEXMF", sTmp);
            }
      
            if (g_sVarTexFonts.IsEmpty()) {
                kTexmfCnf.Comment("VARTEXFONTS");
            }
            else {
                kTexmfCnf.Uncomment("VARTEXFONTS");
                sTmp = g_sVarTexFonts;
                sTmp.Replace('\\', '/');
                if (sTmp.Find((LPCTSTR)g_sVarTexmf) == 0) {
                    sTmp.Replace((LPCTSTR)g_sVarTexmf, "$VARTEXMF");
                }
                kTexmfCnf.Replace("VARTEXFONTS", sTmp);
                if (! ::DirectoryExists(g_sVarTexFonts))
                    ::CreateDirectoryPath(g_sVarTexFonts);
            }
      
            CString sTexmf = "{";
            if (g_sTexmfHome.IsEmpty() == FALSE) {
                sTexmf += "$HOMETEXMF,";
            }
            if (g_sVarTexmf.IsEmpty() == FALSE) {
                sTexmf += "!!$VARTEXMF,";
            }
            if (g_sTexmfLocal.IsEmpty() == FALSE) {
                sTexmf += "!!$TEXMFLOCAL,";
            }
            if (g_sTexmfMain.IsEmpty() == FALSE) {
                sTexmf += "!!$TEXMFMAIN,";
            }
            if (g_sTexmfExtra.IsEmpty() == FALSE) {
                sTexmf += "!!$TEXMFEXTRA,";
            }
            if (sTexmf.GetAt(sTexmf.GetLength() - 1) == ',') {
                sTexmf.SetAt(sTexmf.GetLength() - 1, '}');
            }
            else {
                sTexmf += "}";
            }
            kTexmfCnf.Replace("TEXMF", sTexmf);
      
            CString sTexmfDBs = "$TEXMF";
            if (_strnicmp((LPCTSTR)g_sVarTexFonts, 
                          "$VARTEXMF", strlen("$VARTEXMF")) != 0
                && _strnicmp((LPCTSTR)g_sVarTexFonts, 
                             g_sVarTexmf, g_sVarTexmf.GetLength()) != 0) {
                sTexmfDBs += ";$VARTEXFONTS";
            }
            kTexmfCnf.Replace("TEXMFDBS", sTexmfDBs);
      
            kTexmfCnf.Comment("TEMP");
      
            kTexmfCnf.Commit();
            kTexmfCnf.Close();
        }
    }
    // Rebuild updmap.cfg
    {
        CString dst = ConcatPath(g_sVarTexmf, "web2c\\updmap.cfg");
        if (NeedToEditMapFileList(g_vUpdMap) || NeedToEditMapFileList(g_vUpdMixedMap)) {
            theLog("Updmap configuration: selecting installed map files.\n");
            // Configuring map files
            KeyValFile kConfig;
            if (kConfig.Open(dst)) {
                CString sMapLine;
                // Remove all map lines
                kConfig.RawReplace("Map", "", true, 0);
                kConfig.RawReplace("MixedMap", "", true, 0);
                // 
                POSITION pos;
                CString sKey, sTPMKey;
                TPM *tpm, *tpmSource, *tpmLocal;
                pos = g_vUpdMap.GetStartPosition();
                while (pos) {
                    g_vUpdMap.GetNextAssoc(pos, sKey, tpm);
                    // Weird ! we need to be sure we are talking to the ones 
                    // in the g_sSourceTPM array
                    sTPMKey = tpm->GetKey();
                    if ((g_vSourceTPM.Lookup(sTPMKey, tpmSource) && tpmSource->IsInstalled())
                        || (g_vInstalledTPM.Lookup(sTPMKey, tpmLocal))) {
                        sMapLine = "Map " + sKey;
                        kConfig.RawReplace("", sMapLine , true);
                        theLog("Adding `%s'\n", sMapLine);
                    }
                }

                pos = g_vUpdMixedMap.GetStartPosition();
                while (pos) {
                    g_vUpdMixedMap.GetNextAssoc(pos, sKey, tpm);
                    // Weird ! we need to be sure we are talking to the ones 
                    // in the g_sSourceTPM array
                    sTPMKey = tpm->GetKey();
                    if ((g_vSourceTPM.Lookup(sTPMKey, tpmSource) && tpmSource->IsInstalled())
                        || (g_vInstalledTPM.Lookup(sTPMKey, tpmLocal))) {
                        sMapLine = "MixedMap " + sKey;
                        kConfig.RawReplace("", sMapLine , true);
                        theLog("Adding `%s'\n", sMapLine);
                    }
                }
                kConfig.Commit();
                kConfig.Close();
                theLog("Closing %s.\n", dst);
            }
            else {
                theLog("!!!Failed to configure map files for %s.\n", m_sKey);
            }
        }
    }
    
    return bRet;
}
/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

