// TPM.cpp: implementation of the TPM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "texsetup.h"
#include "TPM.h"
#include "TeXInstall.h"
#include "expat.h"
#include "Win32Util.h"
extern "C" {
#include "windll.h"
}
#include "Registry.h"

#include <ctype.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

TPMSpecialDesc g_fnTPMSpecial[] = {
#if 0
    { "support/gnu-utils",	TPM::InstallParent,	TPM::ConfigureGnuUtils },
    { "support/gs-free",	TPM::Install,		NULL },
    { "support/gs-nonfree",	TPM::Install,		NULL },
    { "support/imagick",	TPM::InstallParent,	TPM::ConfigureIMagick },
    { "support/ntemacs",	TPM::InstallParent,	TPM::ConfigureNTEmacs },
    { "support/netpbm",		TPM::InstallParent,	TPM::ConfigureNetPBM },
    { "support/pfe",		TPM::InstallParent,	TPM::ConfigurePFE },
    { "support/perl",		TPM::InstallParent,	TPM::ConfigurePerl },
    { "support/texshell",	TPM::InstallParent,	TPM::ConfigureTeXShell },
    { "support/texniccenter",	TPM::Install,		NULL },
    { "support/winedt",		TPM::InstallWinEdt,	NULL },
    { "support/winshell",	TPM::InstallWinShell,	NULL },
    { "support/xemacs",		TPM::InstallParent,	TPM::ConfigureXEmacs },
#endif
    { "collection/tex-basic",	NULL,			TPM::ConfigureTeXBasic },
    { "binary/bin-tex4htk",	NULL,			TPM::ConfigureTeX4ht },
    { "binary/bin-dvipdfm",	NULL,			TPM::ConfigureDvipdfm },
    { "binary/bin-dvipsk",	NULL,			TPM::ConfigureDvips },
    { "binary/bin-pdftex",	NULL,			TPM::ConfigurePdftex },
    { NULL, NULL, NULL }
};
  
struct PROCESS_ARGS {
    XML_Parser parser;
    int *retPtr;
};

static TPM::TPMField currentField, currentReqType;
static CString *currentCData, currentArch, 
    currentReqName, currentReqSubdir;
static int currentDepth;
static CStringArray *currentSACData;

static
void reportError(XML_Parser parser, const XML_Char *filename)
{
    if (g_bQuickInstall) return;

    CString sMsg;
    XML_Error code = XML_GetErrorCode(parser);
    const XML_Char *message = XML_ErrorString(code);
    if (message) {
        sMsg.FormatMessage("%1!s!:%2!d!:%3!ld!: %4!s!",
                           filename,
                           XML_GetErrorLineNumber(parser),
                           XML_GetErrorColumnNumber(parser),
                           message);
        ::MessageBox(NULL, sMsg, "XML Parsing", MB_OK);
    }
    else {
        sMsg.FormatMessage("%1!s!: (unknown message %2!d!)", filename, code);
        ::MessageBox(NULL, sMsg, "XML Parsing", MB_OK);
    }    
}

static
void ParseTPM(const void *data,
	      size_t size,
	      const XML_Char *filename,
	      void *args)
{
    XML_Parser parser = ((PROCESS_ARGS *)args)->parser;
    int *retPtr = ((PROCESS_ARGS *)args)->retPtr;
    currentDepth = 0;
    if (!XML_Parse(parser, reinterpret_cast<const char *>(data), size, 1)) {
        reportError(parser, filename);
        *retPtr = 0;
    }
    else {
        *retPtr = 1;
    }
}

void startElement(void *userData, const char *name, const char **atts)
{
    TPM *tpm = reinterpret_cast<TPM *>(userData);
    tpm->startElement(name, atts);
}

void TPM::startElement(const char *name, const char **atts)
{
    int i;
    // Prepare to add new CData Cstring
    currentDepth++;

    if (_stricmp(name, "rdf:RDF") == 0) {
        currentField = TPM::RDF_RDF;
        currentCData = 0;
        currentSACData = 0;
    }
    else if (_stricmp(name, "rdf:Description") == 0) {
        currentField = TPM::RDF_DESCRIPTION;
        currentCData = 0;
        currentSACData = 0;
    }
    else if (_stricmp(name, "TPM:Name") == 0) {
        currentField = TPM::NAME;
        currentCData = &m_sName;
        currentSACData = 0;
    }
    else if (_stricmp(name, "TPM:Version") == 0) {
        currentField = TPM::VERSION;
        currentCData = &m_sVersion;
        currentSACData = 0;
    }
    else if (_stricmp(name, "TPM:Date") == 0) {
        currentField = TPM::DATE;
        currentCData = &m_sDate;
        currentSACData = 0;
    }
    else if (_stricmp(name, "TPM:Creator") == 0) {
        currentField = TPM::CREATOR;
        currentCData = &m_sCreator;
        currentSACData = 0;
    }
    else if (_stricmp(name, "TPM:Title") == 0) {
        currentField = TPM::TITLE;
        currentCData = &m_sTitle;
        currentSACData = 0;
    }
    else if (_stricmp(name, "TPM:Description") == 0) {
        currentField = TPM::DESCRIPTION;
        currentCData = &m_sDescription;
        currentSACData = 0;
    }
    else if (_stricmp(name, "TPM:Flags") == 0) {
        currentField = currentReqType = TPM::FLAGS;
        for (i = 0; atts[i] != NULL; i += 2) {
            if (atts[i] != 0 
                && _stricmp(atts[i], "default") == 0 
                && atts[i+1] != 0) {
		m_bDefault = (strchr("1yY", *atts[i+1]) != NULL);
            }
            else if (atts[i] != 0
                     && _stricmp(atts[i], "format") == 0
                     && atts[i+1] != 0) {
                m_sFormat = atts[i+1];
            }
            else if (atts[i] != 0
                     && _stricmp(atts[i], "remote") == 0
                     && atts[i+1] != 0) {
                m_bOnCD = (strchr("1yY", *atts[i+1]) == NULL);
            }
            else if (atts[i] != 0
                     && _stricmp(atts[i], "selectable") == 0
                     && atts[i+1] != 0) {
                m_bSelectable = (strchr("0nN", *atts[i+1]) == NULL);
            }
            else if (atts[i] != 0
                     && _stricmp(atts[i], "treeroot") == 0
                     && atts[i+1] != 0) {
               if (strchr("1yY", *atts[i+1]) != NULL && g_tpmRoot == 0)
                   g_tpmRoot = this;
            }
        }
    }
    else if (_stricmp(name, "TPM:Requires") == 0) {
        currentField = currentReqType = TPM::REQUIRES;
        currentCData = 0;
        currentSACData = &m_saTPMRequires;
    }
    else if (_stricmp(name, "TPM:RequiredBy") == 0) {
        currentField = currentReqType = TPM::REQUIREDBY;
        currentCData = 0;
        currentSACData = &m_saTPMRequiredBy;
    }
    else if (_stricmp(name, "TPM:SourceFiles") == 0) {
        currentField = TPM::SOURCEFILES;
        if (atts[0] != 0 
            && _stricmp(atts[0], "size") == 0 
            && atts[1] != 0) {
            m_dwSourceSize = atoi(atts[1]);
        }
        currentCData = 0;
        currentSACData = &m_saSourceFiles;
    }
    else if (_stricmp(name, "TPM:DocFiles") == 0) {
        currentField = TPM::DOCFILES;
        if (atts[0] != 0 
            && _stricmp(atts[0], "size") == 0 
            && atts[1] != 0) {
            m_dwDocSize = atoi(atts[1]);
        }
        currentCData = 0;
        currentSACData = &m_saDocFiles;
    }
    else if (_stricmp(name, "TPM:RunFiles") == 0) {
        currentField = TPM::RUNFILES;
        if (atts[0] != 0 
            && _stricmp(atts[0], "size") == 0 
            && atts[1] != 0) {
            m_dwRunSize = atoi(atts[1]);
        }
        currentCData = 0;
        currentSACData = &m_saRunFiles;
    }
    else if (_stricmp(name, "TPM:BinFiles") == 0) {
        currentField = TPM::BINFILES;
        currentArch.Empty();
        for (i = 0; atts[i] != NULL; i += 2) {
            if (atts[i] != 0 
                && _stricmp(atts[i], "size") == 0 
                && atts[i+1] != 0) {
		m_dwBinSize = atoi(atts[i+1]);
            }
            else if (atts[i] != 0
                     && _stricmp(atts[i], "arch") == 0
                     && atts[i+1] != 0) {
                currentArch = atts[i+1];
            }
        }
        currentCData = 0;
        currentSACData = &m_saBinFiles;
    }
    else if (_stricmp(name, "TPM:RemoteFiles") == 0) {
        currentField = TPM::REMOTEFILES;
        if (_stricmp(atts[0], "size") == 0) {
            m_dwRemoteSize = atoi(atts[1]);
        }
        currentCData = 0;
        currentSACData = &m_saRemoteFiles;
    }
    else if (_stricmp(name, "TPM:Package") == 0) {
        currentField = TPM::PACKAGE;
        currentReqName.Empty();
        if (atts[0] != 0 
            && _stricmp(atts[0], "name") == 0 
            && atts[1] != 0) {
            switch (currentReqType) {
            case TPM::REQUIRES:
                m_saTPMRequires.Add(ConcatPath("package", atts[1], '/'));
                break;
            case TPM::REQUIREDBY:
                m_saTPMRequiredBy.Add(ConcatPath("package", atts[1], '/'));
                break;
            }
        }
        currentCData = 0;
    }
    else if (_stricmp(name, "TPM:Collection") == 0) {
        currentField = TPM::COLLECTION;
        if (atts[0] != 0 
            && _stricmp(atts[0], "name") == 0 
            && atts[1] != 0) {
            switch (currentReqType) {
            case TPM::REQUIRES:
                m_saTPMRequires.Add(ConcatPath("collection", atts[1], '/'));
                break;
            case TPM::REQUIREDBY:
                m_saTPMRequiredBy.Add(ConcatPath("collection", atts[1], '/'));
                break;
            }
        }
        currentCData = 0;
    }
    else if (_stricmp(name, "TPM:Support") == 0) {
        currentField = TPM::SUPPORT;
        if (atts[0] != 0 
            && _stricmp(atts[0], "name") == 0 
            && atts[1] != 0) {
            switch (currentReqType) {
            case TPM::REQUIRES:
                m_saTPMRequires.Add(ConcatPath("support", atts[1], '/'));
                break;
            case TPM::REQUIREDBY:
                m_saTPMRequiredBy.Add(ConcatPath("support", atts[1], '/'));
                break;
            }
        }
        currentCData = 0;
    }
    else if (_stricmp(name, "TPM:Binary") == 0) {
        currentField = TPM::SUPPORT;
        if (atts[0] != 0 
            && _stricmp(atts[0], "name") == 0 
            && atts[1] != 0) {
            switch (currentReqType) {
            case TPM::REQUIRES:
                m_saTPMRequires.Add(ConcatPath("binary", atts[1], '/'));
                break;
            case TPM::REQUIREDBY:
                m_saTPMRequiredBy.Add(ConcatPath("binary", atts[1], '/'));
                break;
            }
        }
        currentCData = 0;
    }
    else if (_stricmp(name, "TPM:Source") == 0) {
        currentField = TPM::SUPPORT;
        if (atts[0] != 0 
            && _stricmp(atts[0], "name") == 0 
            && atts[1] != 0) {
            switch (currentReqType) {
            case TPM::REQUIRES:
                m_saTPMRequires.Add(ConcatPath("source", atts[1], '/'));
                break;
            case TPM::REQUIREDBY:
                m_saTPMRequiredBy.Add(ConcatPath("source", atts[1], '/'));
                break;
            }
        }
        currentCData = 0;
    }
    else if (_stricmp(name, "TPM:Installation") == 0) {
        currentField = TPM::INSTALLATION;
        currentCData = 0;
        currentSACData = 0;
    }
    else if (_stricmp(name, "TPM:Execute") == 0) {
        currentField = currentReqType = TPM::EXECUTE;
        // FIXME: only one parameter, only one function
        int iFunction = -1, iParam = -1;
        bool bMixed = false;
        for (i = 0; atts[i] != NULL; i += 2) {
            if (atts[i] != 0 
                && _stricmp(atts[i], "function") == 0) {
                iFunction = i+1;
            }
            else if (atts[i] != 0
                     && _stricmp(atts[i], "parameter") == 0
                     && atts[i+1] != 0) {
                iParam = i+1;
            }
            else if (atts[i] != 0
                     && _stricmp(atts[i], "mode") == 0
                     && atts[i+1] != 0
                     && _stricmp(atts[i+1], "mixed") == 0
                     ) {
                bMixed = true;
            }
        }
        // strictly greater than 0, because odd number
        // also: we do not have parameterless functions
        if (iFunction > 0 && iParam > 0) {
            if (_stricmp(atts[iFunction], "BuildFormat") == 0) {
                AddFormatToBuild(this, atts[iParam]);
            }
            else if (_stricmp(atts[iFunction], "BuildLanguageDat") == 0) {
                AddLanguageDat(this, atts[iParam]);
            }
            else if (_stricmp(atts[iFunction], "addMap") == 0) {
                AddMapFile((bMixed ? g_vUpdMixedMap : g_vUpdMap), this, atts[iParam]);
            }
            else if (_stricmp(atts[iFunction], "addMixedMap") == 0) {
                AddMapFile(g_vUpdMixedMap, this, atts[iParam]);
            }
            else if (_stricmp(atts[iFunction], "addDvipsTeXMap") == 0) {
                AddMapFile(g_vDvipsMap, this, atts[iParam]);
            }
            else if (_stricmp(atts[iFunction], "addPdfTeXMap") == 0) {
                AddMapFile(g_vPdfTeXMap, this, atts[iParam]);
            }
            else if (_stricmp(atts[iFunction], "addDvipdfmMap") == 0) {
                AddMapFile(g_vDvipdfmMap, this, atts[iParam]);
            }
        }
        currentCData = 0;
        currentSACData = 0;
    }
    else {
        currentField = TPM::UNKNOWN;
        currentCData = 0;
        currentSACData = 0;
    }
}

void endElement(void *userData, const char *name)
{
    TPM *tpm = reinterpret_cast<TPM *>(userData);
    tpm->endElement(name);
}

void TPM::endElement(const char *name)
{
    currentDepth--;

    currentField = TPM::UNKNOWN;
}

void cdataHandler(void *userData, const XML_Char *s, int len)
{
    CString sData(s, len);
    sData.TrimLeft();
    sData.TrimRight();

    if (sData.GetLength() == 0) return;

    if (currentCData) {
        if (! currentCData->IsEmpty())
            *currentCData += " ";
        *currentCData += sData;
    }

    if (currentSACData) {
    
        switch (currentField) {
      
        case TPM::SOURCEFILES:
            currentSACData->Add(sData);
            break;
        case TPM::DOCFILES:
            currentSACData->Add(sData);
            break;
        case TPM::RUNFILES:
            // Remove the tpm file itself
            if (sData.Right(4) != ".tpm")
                currentSACData->Add(sData);
            break;
        case TPM::BINFILES:
            // If we were to support multiple architectures
            // tpm->m_saBinFiles[currentArch].Append(saCData);
            if (currentArch == "win32")
                currentSACData->Add(sData);
            break;
        case TPM::REMOTEFILES:
            currentSACData->Add(sData);
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TPM::TPM ()
{
    // FIXME : should not be really needed !
    m_bSelected = false;
    m_bInstalled = false;
    m_bSelectable = true;
    m_dwBinSize = m_dwRemoteSize = 0;
    m_dwRunSize = m_dwDocSize = m_dwSourceSize = 0;
    m_bDefault = false;
    m_bOnCD = true;
    m_bDownloaded = false;
}

bool TPM::Init(const CString &filename)
{
    PROCESS_ARGS args;
    int ret;
    XML_Parser parser = XML_ParserCreate(NULL);
    int depth = 0;
    XML_SetUserData(parser, &depth);
    XML_SetElementHandler(parser, ::startElement, ::endElement);
    XML_SetCharacterDataHandler(parser, cdataHandler);
    XML_SetUserData(parser, reinterpret_cast<void *>(this));
    args.parser = parser;
    args.retPtr = &ret;
    ::FileMapAndProcess(filename, ParseTPM, &args);
    XML_ParserFree(parser);

#if 0
    FILE *f = fopen("c:/tmp/foo.log", "a+");
    fprintf(f,"TPM file %s\nBinFiles:\n", m_sName);
    for (int i = 0; i < m_saBinFiles.GetSize(); i++) {
        fprintf(f,"%s\n", m_saBinFiles.GetAt(i));
    }
    fprintf(f,"RunFiles:\n", m_sName);
    for (i = 0; i < m_saRunFiles.GetSize(); i++) {
        fprintf(f,"%s\n", m_saRunFiles.GetAt(i));
    }
    fprintf(f,"TPM file %s\nDocFiles:\n", m_sName);
    for (i = 0; i < m_saDocFiles.GetSize(); i++) {
        fprintf(f,"%s\n", m_saDocFiles.GetAt(i));
    }
    fprintf(f,"TPM file %s\nSourceFiles:\n", m_sName);
    for (i = 0; i < m_saSourceFiles.GetSize(); i++) {
        fprintf(f,"%s\n", m_saSourceFiles.GetAt(i));
    }
    fprintf(f,"TPM file %s\nRemoteFiles:\n", m_sName);
    for (i = 0; i < m_saRemoteFiles.GetSize(); i++) {
        fprintf(f,"%s\n", m_saRemoteFiles.GetAt(i));
    }
    fclose(f);
#endif
    return (ret != 0);
}

TPM::~TPM()
{

}

void TPM::AddRequiredBy()
{
    CString sKey;
    TPM *tpm;
    for (int i=0; i < m_saTPMRequires.GetSize(); i++) {
        sKey = m_saTPMRequires.GetAt(i);
        if (g_vSourceTPM.Lookup(sKey, tpm)) {
            bool bFound = false;
            for (int j=0; !bFound && j < tpm->m_saTPMRequiredBy.GetSize(); j++)
                bFound = bFound || (tpm->m_saTPMRequiredBy[j] == m_sKey);
            if (!bFound) 
                tpm->m_saTPMRequiredBy.Add(m_sKey);
        }
    }
}

void TPM::Select()
{
    if (! m_bSelectable) {
        return;
    }
#if 0
    {
        FILE *f = fopen("c:/tmp/select.log", "a");
        fprintf(f, "Selecting %s\n", (LPCTSTR)GetName());
        fclose(f);
    }
#endif
    m_bSelected = true;
    for (int i = 0; i < m_saTPMRequires.GetSize(); i++) {
        TPM *tpmChild;
        CString sKey = m_saTPMRequires.GetAt(i);
        if (g_vSourceTPM.Lookup(sKey, tpmChild))
            if (g_eInstallType != INSTALL_MAINTENANCE || !tpmChild->IsUpToDate())
                tpmChild->Select();
    }
}

void TPM::DeSelect()
{
    int nRequired = 0;
    CString sKey;
    int i;
    TPM *tpmChild;

    for (i = 0; i < m_saTPMRequiredBy.GetSize(); i++) {
        sKey = m_saTPMRequiredBy.GetAt(i);
        if ((sKey != m_sKey) && g_vSourceTPM.Lookup(sKey, tpmChild)) {
            if (tpmChild->IsSelected())
                nRequired++;
        }
    }

    if (nRequired <= 1) {
	for (i = 0; i < m_saTPMRequires.GetSize(); i++) {
		sKey = m_saTPMRequires.GetAt(i);
		if (g_vSourceTPM.Lookup(sKey, tpmChild)) {
			tpmChild->DeSelect();
		}
	}
    }
	
    m_bSelected = (nRequired > 1);
}

bool TPM::InstallFromCdrom()
{
    int i;
    bool bRet = true;

    CSingleLock theLock(&g_csCritSec, FALSE);
    theStatus.nOpId = IDS_NOW_INSTALLING;
    theLock.Unlock();

    for (i = 0; i < m_saRunFiles.GetSize(); i++) {
        bRet = bRet && SafeCopyFile(ConcatPath(g_sLocalSourceDir, m_saRunFiles.GetAt(i)),
                                    ConcatPath(g_sInstallRootDir, m_saRunFiles.GetAt(i)));
    }
    for (i = 0; i < m_saBinFiles.GetSize(); i++) {
        bRet = bRet && SafeCopyFile(ConcatPath(g_sLocalSourceDir, m_saBinFiles.GetAt(i)),
                                    ConcatPath(g_sInstallRootDir, m_saBinFiles.GetAt(i)));
    }
    if (g_bInstallDocumentation) {
        for (i = 0; i < m_saDocFiles.GetSize(); i++) {
            bRet = bRet && SafeCopyFile(ConcatPath(g_sLocalSourceDir, m_saDocFiles.GetAt(i)),
                                        ConcatPath(g_sInstallRootDir, m_saDocFiles.GetAt(i)));
        }
    }
    if (g_bInstallSource) {
        for (i = 0; i < m_saSourceFiles.GetSize(); i++) {
            bRet = bRet && SafeCopyFile(ConcatPath(g_sLocalSourceDir, m_saSourceFiles.GetAt(i)),
                                        ConcatPath(g_sInstallRootDir, m_saSourceFiles.GetAt(i)));
        }
    }

    if (bRet) {
        theLog("All files from %s copied successfully.\n", m_sKey);
    }
    else {
        theLog("!!!Not all files from %s copied successfully.\n", m_sKey);
    }
    // Do not abort, go on.
    bRet = true;
    return bRet;
}

bool TPM::InstallFromZip()
{
    bool bRet = true;
    CString sZipFile;
    CString sType = m_sKey.Left(m_sKey.Find('/'));

    sZipFile = ConcatPath(g_sLocalSourceDir, m_sKey, '/');
    sZipFile = sZipFile + CString(".zip");
    

    if (FileExists(sZipFile)) {
        bRet = InstallLocalZipFile(sZipFile, true);
    }
    else {
        theLog("Warning: zip file not found (%s).\n", (LPCTSTR)sZipFile);
        bRet = false;
    }

    if (m_saBinFiles.GetSize() > 0) {
        sZipFile = ConcatPath(g_sLocalSourceDir, m_sKey + "-win32", '/');
        sZipFile = sZipFile + CString(".zip");
        if (FileExists(sZipFile)) {
            bRet = InstallLocalZipFile(sZipFile, false);
        }
        else {
            theLog("Warning: zip file not found (%s).\n", (LPCTSTR)sZipFile);
        }
    }

    return bRet;
}

bool TPM::Download(CString &sDest)
{
    bool bRet = true, bRet1 = true;
    
    if (HasFilesToInstall()) {
        CString sZipFile = ConcatPath(g_sRemoteSourceDir, m_sKey, '/');
        sZipFile = sZipFile + CString(".zip");
        
        sDest = ConcatPath(g_sLocalSourceDir, m_sKey, '/');
        sDest = sDest + CString(".zip");

        /* Grab the remote file to the local one */
        bRet = GrabInternetFile(sZipFile, sDest);
        
        if (bRet) {
            m_bDownloaded = true;
            theLog("File %s has been downloaded successfully to %s.\n", (LPCTSTR)sZipFile, (LPCTSTR)sDest);
        }
        else {
            theLog("Warning: Internet file not found (%s).\n", (LPCTSTR)sZipFile);
        }

        if (m_saBinFiles.GetSize() > 0) {
            sZipFile = ConcatPath(g_sRemoteSourceDir, m_sKey + "-win32", '/');
            sZipFile = sZipFile + CString(".zip");
            
            sDest = ConcatPath(g_sLocalSourceDir, m_sKey + "-win32", '/');
            sDest = sDest + CString(".zip");

            /* Grab the remote file to the local one */
            bRet1 = GrabInternetFile(sZipFile, sDest);
            
            if (bRet1) {
                m_bDownloaded = true;
                theLog("File %s has been downloaded successfully to %s.\n", (LPCTSTR)sZipFile, (LPCTSTR)sDest);
            }
            else {
                theLog("Warning: Internet file not found (%s).\n", (LPCTSTR)sZipFile);
            }
        }
    }

    return bRet && bRet1;
}

bool TPM::InstallFromInternet()
{
    bool bRet = true;
    CString sZipFile;
    CString sTmp;

    sZipFile = ConcatPath(g_sRemoteSourceDir, m_sKey, '/');
    sZipFile = sZipFile + CString(".zip");

    if (GrabInternetFile(sZipFile, sTmp)) {
        bRet = InstallLocalZipFile(sTmp, true);
        DeleteFile(sTmp);
    }
    else {
        theLog("Warning: Internet file not found (%s).\n", (LPCTSTR)sZipFile);
        bRet = false;
    }

    if (m_saBinFiles.GetSize() > 0) {

        sZipFile = ConcatPath(g_sRemoteSourceDir, m_sKey + "-win32", '/');
        sZipFile = sZipFile + CString(".zip");
        
        if (GrabInternetFile(sZipFile, sTmp)) {
            bRet = InstallLocalZipFile(sTmp, false);
            DeleteFile(sTmp);
        }
        else {
            theLog("Warning: Internet file not found (%s).\n", (LPCTSTR)sZipFile);
            bRet = false;
        }
    }

    return bRet;
}

bool TPM::InstallLocalZipFile(const CString &sZipFile, bool bCommon)
{
    CStringArray saFiles;
    bool bRet = true;
    int i;

    if (bCommon) {
        for (i = 0; i < m_saRunFiles.GetSize(); i++) {
            saFiles.Add(m_saRunFiles.GetAt(i));
        }
        if (g_bInstallDocumentation) {
            for (i = 0; i < m_saDocFiles.GetSize(); i++) {
                saFiles.Add(m_saDocFiles.GetAt(i));
            }
        }
        if (g_bInstallSource) {
            for (i = 0; i < m_saSourceFiles.GetSize(); i++) {
                saFiles.Add(m_saSourceFiles.GetAt(i));
            }
        }
    }
    else {
        for (i = 0; i < m_saBinFiles.GetSize(); i++) {
            saFiles.Add(m_saBinFiles.GetAt(i));
        }
    }

    bRet = UnzipFile(sZipFile, g_sInstallRootDir, &saFiles);

    if (bRet)
        theLog("File %s has been unzipped successfully.\n", sZipFile);
    else
        theLog("!!!File %s has not been unzipped successfully.\n", sZipFile);
    
    // Do not abort, go on.
    bRet = true;
    return bRet;

}

bool TPM::InstallRemoteFiles()
{
    bool bRet = true;

    if (m_saRemoteFiles.GetSize() > 0) {  
        for (int i = 0; i < m_saRemoteFiles.GetSize(); i++) {
            bRet = bRet && ProcessSpecialFile(m_saRemoteFiles[i]);
        }
    }

    return bRet;
}

int TPM::HasFilesToInstall()
{
    // The .tpm file is not counted in
    // So at least 1 file overall
    return (m_saSourceFiles.GetSize()
            + m_saDocFiles.GetSize()
            + m_saRunFiles.GetSize()
            + m_saBinFiles.GetSize() > 0);
}

bool TPM::Install ()
{
    bool bRet = false;

    if (!m_bSelected || m_bInstalled) return true;

    // First, copy files
    // Where from ???
    // either the cdrom, the local .zip file or the remote .zip file
    // this will just install normal files, not remote ones.
    if (HasFilesToInstall()) {
        switch (g_eSourceType) {
        case ::SOURCE_CDROM:
            if (InstallFromCdrom()) {
            }
            else {
                theLog("Error: fail to install %s from cdrom.\n", m_sKey);
                return false;
            }
            break;
        case ::SOURCE_ZIP:
            if (InstallFromZip()) {
            }
            else {
                theLog("Error: fail to install %s from zip file.\n", m_sKey);
                return false;
            }
            break;
        case ::SOURCE_INTERNET:
            if (IsDownloaded()) {
                if (InstallFromZip()) {
                }
                else {
                    theLog("Error: fail to install %s from the internet.\n", m_sKey);
                    return false;
                }
            }
            else {
                theLog("Error: %s has not been downloaded, so not installed.\n", m_sKey);
            }
            break;
        }
    }

    // Now remote files
    if (!InstallRemoteFiles()) {
        theLog("Error: fail to install remote files for %s.\n", m_sKey);
        return false;
    }
    
    // anyway we don't want to reinstall it right now ...
    m_bInstalled = true;
    if (bRet = InstallTPM()) {
        theLog("Package %s has been installed successfully.\n", m_sKey);
    }
    else {
        theLog("!!!Package %s has not been installed successfully.\n", m_sKey);
    }
    return bRet;
}

bool TPM::InstallTPM()
{
    // Do not forget to copy the tpm file !
    CString sSrcTpm = ConcatPath(g_sSourceTPMDir, m_sKey) + ".tpm";
    CString sDstTpm = ConcatPath(g_sLocalTPMDir, m_sKey) + ".tpm";
    return ::SafeCopyFile(sSrcTpm, sDstTpm);
}

bool TPM::InstallParent()
{
    // Some support packages should be installed in "c:\Program Files"
    // rather than in "c:\Program Files\TeXLive" ...
    CString sInstallRootDir = g_sInstallRootDir;
    GetParentDirectory(g_sInstallRootDir);
    bool bRet = TPM::Install();
    g_sInstallRootDir = sInstallRootDir;
    return bRet;
}

bool TPM::Update ()
{
    return true;
}

static void add_uniq(CString &sPath, CStringArray &sa)
{
    int iPathLen = sPath.GetLength();
    int i = 0;
    while (i < sa.GetSize() &&  iPathLen < sa.GetAt(i).GetLength()) i++;
    while (i < sa.GetSize() && iPathLen == sa.GetAt(i).GetLength()) {
        if (sPath == sa.GetAt(i)) return;
        i++;
    }
    sa.InsertAt(i, sPath);
}

bool TPM::UnInstall ()
{
    int i;
    bool bRet = true;
    CStringArray saDirectories, saParents, *psaDirs, *psaParents, *psaTmp;
    CString sPath;

    CSingleLock theLock(&g_csCritSec, FALSE);
    theStatus.nOpId = IDS_NOW_REMOVING;
    theStatus.tpm = this;
    theStatus.sName = (LPCTSTR)GetKey();
    theLock.Unlock();

    for (i = 0; i < m_saRunFiles.GetSize(); i++) {
        sPath = ConcatPath(g_sInstallRootDir, m_saRunFiles.GetAt(i));
        bRet = bRet && ::RemovePath(sPath);
        GetParentDirectory(sPath);
        add_uniq(sPath, saDirectories);
    }
    for (i = 0; i < m_saBinFiles.GetSize(); i++) {
        sPath = ConcatPath(g_sInstallRootDir, m_saBinFiles.GetAt(i));
        bRet = bRet && ::RemovePath(sPath);
        GetParentDirectory(sPath);
        add_uniq(sPath, saDirectories);
    }
    for (i = 0; i < m_saDocFiles.GetSize(); i++) {
        sPath = ConcatPath(g_sInstallRootDir, m_saDocFiles.GetAt(i));
        bRet = bRet && ::RemovePath(sPath);
        GetParentDirectory(sPath);
        add_uniq(sPath, saDirectories);
    }
    for (i = 0; i < m_saSourceFiles.GetSize(); i++) {
        sPath = ConcatPath(g_sInstallRootDir, m_saSourceFiles.GetAt(i));
        bRet = bRet && ::RemovePath(sPath);
        GetParentDirectory(sPath);
        add_uniq(sPath, saDirectories);
    }
    // The TPM file itself
    sPath = ConcatPath(g_sInstallRootDir, "texmf/tpm");
    sPath = ConcatPath(sPath, GetKey() + CString(".tpm"));
    bRet = bRet && ::RemovePath(sPath);
    GetParentDirectory(sPath);
    add_uniq(sPath, saDirectories);

    // We need to remove any empty directories
    // Do not bother to test if empty, just remove them.
    int iRootLength = g_sInstallRootDir.GetLength();
    psaDirs = &saDirectories;
    psaParents = &saParents;
    while (psaDirs->GetSize() > 0) {
        for (i = 0; i < psaDirs->GetSize(); i++) {
            sPath = psaDirs->GetAt(i);
            if (RemoveDirectory(sPath) != 0) {
                theLog("Removed directory %s\n", sPath);
                GetParentDirectory(sPath);
                if (iRootLength < sPath.GetLength()) {
                    add_uniq(sPath, *psaParents);
                }
            }
        }
        psaTmp = psaParents;
        psaParents = psaDirs;
        psaDirs = psaTmp;
        psaParents->RemoveAll();
    }

    if (bRet) {
        theLog("All files from %s removed successfully.\n", m_sKey);
    }
    else {
        theLog("!!!Not all files from %s removed successfully.\n", m_sKey);
    }
    // Do not abort, go on.
    return true;
}

bool TPM::GetSpecialFile(const CString & sFile, CString & sTmp)
{
    CString sUrl(sFile);

    if (_stricmp(sUrl.Left(7),"http://") != 0 
        && _stricmp(sUrl.Left(6),"ftp://") != 0) {
        sUrl = ConcatPath(g_sRemoteSourceDir, sFile, '/');
    }

    if (g_uiNetMethod == NetIOUnavail) {
        theLog("!!!No net access allowed, cannot grab %s from the Internet!!!\n", (LPCTSTR)sUrl);
        return false;
    }
    else if (! GrabInternetFile(sUrl, sTmp)) {
        theLog("Error: Failed to grab %s from the Internet!!!\n", (LPCTSTR)sUrl);
        return false;
    }
    theLog("Grabbed %s from the Internet, copy in %s\n", (LPCTSTR)sUrl, (LPCTSTR)sTmp);
    return true;
}

bool TPM::ProcessSpecialFile(const CString &sFile)
{
    CString sTmp;
    bool bRet = true;
    bool bRemoveTemp = false;

    // Special files should be looked for locally,
    // then on remote site if available.
    // Alternatively, if they specify an absolute url,
    // only one seek will occur : remote files are remote files.
    // Only files specified with a relative path will be looked
    // for locally and remotely.
  
    if (_stricmp(sFile.Left(7),"http://") == 0 
        || _stricmp(sFile.Left(6),"ftp://") == 0) {
        // Url, absolute
        if (g_uiNetMethod == NetIOUnavail) {
            theLog("Error: No net access allowed, cannot grab %s from the Internet!!!\n", (LPCTSTR)sFile);
            return false;
        }
        else if (GrabInternetFile(sFile, sTmp)) {
            theLog("Special file %s has been grabbed from the Internet, copy in %s.\n", 
                   (LPCTSTR)sFile, (LPCTSTR)sTmp);
            bRemoveTemp = true;
        }
        else {
            theLog("Error: Failed to grab %s from the Internet!!!\n", (LPCTSTR)sFile);
            return false;
        }
    }
    else {
        // Filename, relative
        CString sLocal = ConcatPath(g_sLocalSourceDir, sFile);
        if (FileExists(sLocal)) {
            sTmp = sLocal;
            theLog("Special file %s has been found on local directory.\n", (LPCTSTR)sTmp);
        }
        else {
            // Try with the remote directory
            CString sUrl = ConcatPath(g_sRemoteSourceDir, sFile, '/');
            if (g_uiNetMethod == NetIOUnavail) {
                theLog("Error: No net access allowed, cannot grab %s from the Internet!!!\n", (LPCTSTR)sFile);
                return false;
            }
            else if (GrabInternetFile(sUrl, sTmp)) {
                theLog("Special file %s has been grabbed from the Internet, copy in %s.\n", 
                       (LPCTSTR)sUrl, (LPCTSTR)sTmp);
                bRemoveTemp = true;
            }
            else {
                theLog("Error: Failed to grab %s from the Internet!!!\n", (LPCTSTR)sUrl);
                return false;
            }
        }
    }

    if (_stricmp(sTmp.Right(4), ".zip") == 0) {
        bRet = bRet && UnzipFile(sTmp, g_sInstallRootDir);
    }
    else if (_stricmp(sTmp.Right(4), ".exe") == 0) {
        bRet = bRet && RunProcess((LPCTSTR)sTmp, false, true); // no logging, fails under W98
    }
    else {
        // We don't know what to do with it, just copy it.
        bRet = bRet && SafeCopyFile(sTmp, ConcatPath(g_sInstallRootDir, sFile));
    }

    if (bRemoveTemp)
        DeleteFile(sTmp);

    return bRet;
}

// Size in  bytes of the package
// Default: dwClusterSize = 4096, recursive = false, relative = false, depth = 0
unsigned long TPM::GetSize(bool rounded, unsigned long dwClusterSize, 
                           bool recursive, bool relative, int depth) const
{
    unsigned long dwTpmRounded = 0;
    unsigned long dwFileNumber = 0;

    static TPMSet vTpmFlag;

#define ROUNDUP(size, number) (rounded ? ((size / dwClusterSize) + number) * dwClusterSize : size)

    if (!relative || (relative && m_bSelected)) {
        if (m_dwBinSize > 0) {
            dwTpmRounded += ROUNDUP(m_dwBinSize, m_saBinFiles.GetSize());
        }
        if (m_dwRunSize > 0) {
            dwTpmRounded += ROUNDUP(m_dwRunSize, m_saRunFiles.GetSize());
        }
        if (g_bInstallDocumentation && m_dwDocSize > 0) {
            dwTpmRounded += ROUNDUP(m_dwDocSize, m_saDocFiles.GetSize());
        }
        if (g_bInstallSource && m_dwSourceSize > 0) {
            dwTpmRounded += ROUNDUP(m_dwSourceSize, m_saSourceFiles.GetSize());
        }
        if (m_dwRemoteSize > 0) {
            // No way to know how much space it will take !
            dwTpmRounded += m_dwSourceSize;
        }
    }
#undef ROUNDUP

    // Beware : no package should be counted twice !
    if  (recursive) {
        CString sKey;
        TPM *tpmChild;
        vTpmFlag[m_sKey] = (TPM *)this;
        for (int i = 0; i < m_saTPMRequires.GetSize(); i++) {
            sKey = m_saTPMRequires.GetAt(i);
            if (! vTpmFlag.Lookup(sKey, tpmChild)
                && g_vSourceTPM.Lookup(sKey, tpmChild)) {
                dwTpmRounded += tpmChild->GetSize(rounded, dwClusterSize, recursive, relative, depth+1);
            }
        }
        // When back at depth 0, remove the keys for next time.
        if (depth == 0) {
            CString sKey;
            POSITION pos = vTpmFlag.GetStartPosition();
            while(pos) {
                vTpmFlag.GetNextAssoc(pos, sKey, tpmChild);
                vTpmFlag.RemoveKey(sKey);
            }
        }
    }

    return dwTpmRounded;

}

// Check if tpm has been installed
bool TPM::IsToBeInstalled() const
{
    if (g_eInstallType != INSTALL_MAINTENANCE)
        return true;
    TPM *tpmInstalled;
    return g_vInstalledTPM.Lookup(m_sKey, tpmInstalled);
}

// Compare date with the reference distribution
bool TPM::IsUpToDate() const
{
    // If not in maintenance mode, return
    // false by convention
    if (g_eInstallType != INSTALL_MAINTENANCE)
        return true;

    CTime ctTpmInstalled, ctTpmSource;
    TPM *tpmInstalled;

    if (!g_vInstalledTPM.Lookup(m_sKey, tpmInstalled)) {
        // not installed
        return false;
    }

    // If we can't parse the installed date, return false
    if (! wfc_parse_iso_8601_string((LPCTSTR)tpmInstalled->GetDate(), ctTpmInstalled))
        return false;
    // If we can't parse the source date, return true
    if (! wfc_parse_iso_8601_string((LPCTSTR)m_sDate, ctTpmSource))
        return true;

    return (ctTpmInstalled >= ctTpmSource);
}

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

