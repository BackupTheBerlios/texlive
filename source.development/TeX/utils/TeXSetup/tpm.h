// TeXPackage.h: interface for the TeXPackage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXPACKAGE_H__5458BDDD_D730_47D9_B41C_2716F4D49FD1__INCLUDED_)
#define AFX_TEXPACKAGE_H__5458BDDD_D730_47D9_B41C_2716F4D49FD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

/* 
   Currently, there is a one to one correspondence
   between features provided and package names.
   If it were to change, then we would need to adapt
   the data structures.
*/

class CTreePackage;

class TPM {

    friend CTreePackage;

protected:
//     static int WINAPI DoProgress(LPCSTR filename, unsigned long size);
//     static int WINAPI DoReplace (char *filename);
//     static int WINAPI DoPrint (LPSTR buf, unsigned long size);

//     bool UnzipFile(const CString &zipName, const CString &destdir, const CStringArray *sa = NULL);
    //    bool GrabInternetFile(const CString &url, CString &filename);
    bool GetSpecialFile(const CString &sFile, CString &sTmp);
    bool ProcessSpecialFile(const CString &sFile);

    bool InstallFromCdrom();
    bool InstallFromZip();
    bool InstallFromInternet();
    bool InstallLocalZipFile(const CString &sZipFile, bool bCommon = true);
    bool InstallRemoteFiles();
    bool InstallTPM();

    bool m_bSelected;
    bool m_bInstalled;
    bool m_bSelectable;
    bool m_bDownloaded;

    CString m_sName;
    CString m_sKey;
    CString m_sDate;
    CString m_sVersion;
    CString m_sCreator;
    CString m_sTitle;
    CString m_sDescription;

    bool m_bDefault;            // Is it part of the default setup ?
    bool m_bOnCD;               // default to true, but else net is required
    CString m_sFormat;          // Potentially, format to build

    CStringArray m_saRunFiles;
    // If we were to support multiple architectures
    //  CMap<CString, CString&, CStringArray&, CStringArray&> m_saBinFiles;
    CStringArray m_saBinFiles;
    CStringArray m_saDocFiles;
    CStringArray m_saSourceFiles;
    CStringArray m_saRemoteFiles;

    DWORD m_dwRunSize;
    DWORD m_dwSourceSize;
    DWORD m_dwDocSize;
    DWORD m_dwBinSize;
    DWORD m_dwRemoteSize;
  
    CString m_sProvides; // FIXME : the current implementation does not allow
    // to have several packages providing the same feature.

    // CArray<TPM *, TPM *> m_vTPMRequires;
    // CArray<TPM *, TPM *> m_vTPMRequiredBy;
    // If we resolve the requirements at use time
    // it maybe simpler and more flexible : all the TPM
    // descriptions can be stored in a single map with
    // {binary,collection,package}/name as key.
    CStringArray m_saTPMRequires;
    CStringArray m_saTPMRequiredBy;
  
public:

    enum TPMField {
        RDF_RDF,
        RDF_DESCRIPTION,
        NAME,
        DATE,
        VERSION,
        CREATOR,
        TITLE,
        DESCRIPTION,
        FLAGS,
        SOURCEFILES,
        DOCFILES,
        RUNFILES,
        BINFILES,
        REMOTEFILES,
        PROVIDES,
        REQUIRES,
        REQUIREDBY,
        PACKAGE,
        COLLECTION,
        SUPPORT,
        BINARY,
        INSTALLATION,
        EXECUTE,
        UNKNOWN
    };

    TPM();
    virtual ~TPM();

    virtual bool Init(const CString & filename);
    virtual bool Download(CString &sDest = CString(""));
    virtual bool Install();
    virtual bool InstallParent();
    virtual bool UnInstall();
    virtual bool Update();

    virtual const CString& GetName() const { return m_sName; }
    virtual const CString& GetKey() const { return m_sKey; }
    virtual const CString& GetVersion() const { return m_sVersion; }
    virtual const CString& GetTitle() const { return m_sTitle; }
    virtual const CString& GetDescription() const { return m_sDescription; }
    virtual void SetName(const CString &sName) { m_sName = sName; }
    virtual void SetKey(const CString &sKey) { m_sKey = sKey; }
    virtual unsigned long GetSize(bool rounded, unsigned long dwClusterSize = 4096, 
                                  bool recursive = false, bool relative = false,
                                  int depth = 0) const;
    virtual const CString &GetDate() const { return m_sDate; }
    virtual const CStringArray &GetRequired() const { return m_saTPMRequires; }
    virtual const CStringArray &GetRequiredBy() const { return m_saTPMRequiredBy; }
    virtual void AddRequires(const CString &sPackage) { m_saTPMRequires.Add(sPackage); }

    virtual void Select();
    virtual void DeSelect();
    virtual bool IsSelected() const { return m_bSelected; }
    virtual bool IsInstalled() const { return m_bInstalled; }
    virtual bool IsDownloaded() const { return m_bDownloaded; }

    virtual bool IsDefault() const { return m_bDefault; }
    virtual bool IsOnCD() const { return m_bOnCD; }
    virtual bool IsToBeInstalled() const;
    virtual bool IsUpToDate() const;
    virtual bool HasFormatToBuild() const { return m_sFormat.IsEmpty() == 0; }
    virtual CString &GetFormatToBuild() { return m_sFormat; }
    virtual int HasFilesToInstall();

    void startElement(const char *name, const char **atts);
    void endElement(const char *name);
    void AddRequiredBy();
  
    /*
      Specific Install() and Configure() instructions.
      A good OO design would require to derive new
      class from the current one and use virtual functions.
      However, we still will have only one instance of each of 
      the more specific packages, so there is no gain in doing this.
    */
    bool InstallWinEdt();
    bool InstallWinShell();
    bool InstallGs();

    bool ConfigureDefault();
    
    bool ConfigureTeXBasic();
    bool ConfigureTeX4ht();
    bool ConfigureDvipdfm();
    bool ConfigureDvips();
    bool ConfigurePdftex();
    bool ConfigurePSUtils();
    bool ConfigureGnuUtils();
    
    //    bool ConfigureFrench();
    bool ConfigureGsFree();
    bool ConfigureGsNonFree();
    bool ConfigureIMagick();
    bool ConfigureNTEmacs();
    bool ConfigureNetPBM();
    bool ConfigurePFE();
    bool ConfigurePerl();
    bool ConfigureTeXShell();
    bool ConfigureTeXnicCenter();
    bool ConfigureWinEdt();
    bool ConfigureWinShell();
    bool ConfigureXEmacs();
};

// A map of <CString, TPM *> pairs
typedef CTypedPtrMap<CMapStringToPtr, CString, TPM *> TPMSet;

// The set of all TPM packages
extern TPMSet g_vSourceTPM;     // Those to be installed
extern TPMSet g_InstalledTPM;   // Those already installed if any

// Some packages need special Install and or Configure
// instuctions

// Some typedefs to point to Install and Configure special instructions
typedef bool (TPM::*pfTpmInstall)();
typedef bool (TPM::*pfTpmConfigure)();

typedef struct {
    char *lpctstrTpmName;
    pfTpmInstall pfInstall;
    pfTpmConfigure pfConfigure;
} TPMSpecialDesc;

// An array describing which packages have specific instructions
extern TPMSpecialDesc g_fnTPMSpecial[];

#endif // !defined(AFX_TEXPACKAGE_H__5458BDDD_D730_47D9_B41C_2716F4D49FD1__INCLUDED_)

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

