// KeyValFile.cpp: implementation of the KeyValFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TeXSetup.h"
#include "KeyValFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KeyValFile::KeyValFile()
{
    bOk = TRUE;
    bModified = FALSE;
}

KeyValFile::~KeyValFile()
{
    saContent.RemoveAll();
}

BOOL KeyValFile::Open(const CString &filename) 
{
    sName = filename;
    bOk = TRUE;
    FILE *f = fopen((LPCTSTR)filename, "r");
    if (f == NULL) {
        return bOk = FALSE;
    }
    char buf[1024];
    while (fgets(buf, sizeof(buf), f) != NULL) {
        saContent.Add(buf);
    }
    if (feof(f)) {
        fclose(f);
        return bOk = TRUE;
    }
    else {
        fclose(f);
        return bOk = FALSE;
    }
}

static const char *lpszCommentChars = "%#";
static const char *lpszWhiteSpaceChars = " \t\n\r";

BOOL KeyValFile::IsComment(const CString &sLine)
{
    if (sLine.IsEmpty())
        return TRUE;

    CString sTmp = sLine;
    sTmp.TrimLeft();
    return (strchr(lpszCommentChars, sTmp[0]) != 0);
}

// This is flakey. We need to specify more precisely 
// which pattern we are looking for.
int KeyValFile::Lookup(const CString &sPattern, BOOL bSkipComments)
{
    for (int i = 0; i < saContent.GetSize(); i++) {
        CString sLine = saContent.GetAt(i);
        sLine.TrimLeft();
        // Skip comments
        if (bSkipComments && IsComment(sLine)) 
            continue;
        sLine.TrimLeft(lpszCommentChars);
        sLine.TrimLeft(lpszWhiteSpaceChars);
        int nPos = sLine.Find((LPCTSTR)sPattern);
        if (nPos == -1 || nPos > 0)
            continue;
        CString sTmp = sLine.Mid(nPos + sPattern.GetLength());
        sTmp.TrimLeft();
        if (sTmp[0] == '=') {
            return i;
        }
    }
    return -1;
}

BOOL KeyValFile::Lookup(const CString &sPattern, CString &sValue)
{
    int nPos;
    sValue.Empty();
    for (int i = 0; i < saContent.GetSize(); i++) {
        CString sLine = saContent.GetAt(i);
	sLine.TrimLeft();
        // Skip comments
        if (IsComment(sLine)) 
            continue;
        if (strncmp((LPCTSTR)sLine, (LPCTSTR)sPattern, strlen((LPCTSTR)sPattern)) == 0
            && (nPos = sLine.Find('=')) > -1) {
            sValue = sLine.Mid(nPos + 1);
            sValue.TrimLeft();
            sValue.TrimRight();
            return TRUE;
        }
    }
    return FALSE;
}

BOOL KeyValFile::RawReplace(const CString &sOldString, const CString &sNewString, BOOL bSkipComment, int nExpectedPos)
{
    BOOL bRet = FALSE;

    if (sOldString.IsEmpty()) {
        if (!sNewString.IsEmpty()) {
            saContent.Add(sNewString);
        }
        bModified = bRet = TRUE;
    }
    else {
        CString sLine;
        int i = 0, nPos;
        while (i < saContent.GetSize()) {
            sLine = saContent.GetAt(i);
            if (bSkipComment && IsComment(sLine)) {
                i++; continue;
            }
            if ((nPos = sLine.Find(sOldString)) == -1) {
                i++; continue;
            }
            if (nExpectedPos > -1 && nPos != nExpectedPos) {
                i++; continue;
            }
            if (sNewString.IsEmpty()) {
                saContent.RemoveAt(i);
            }
            else {
                sLine.Replace(sOldString, sNewString);
                saContent.SetAt(i, sLine);
                i++;
            }
            bModified = bRet = TRUE;
        }
    }
    return bRet;
}

BOOL KeyValFile::Replace(const CString &sVariable, const CString &sValue)
{
    CString sTmp = sValue;
    int nIndex = Lookup(sVariable);
    if (nIndex < 0)
        return FALSE;
    // FIXME: Weird!
    // No '\\' in texmf.cnf !
    sTmp.Replace('\\', '/');
    CString sNew = sVariable;
    sNew = sNew + " = ";
    sNew = sNew + sTmp;
    saContent.SetAt(nIndex, sNew);
    bModified = TRUE;
    return TRUE;
}

BOOL KeyValFile::Commit()
{
    if (! bOk)
        return FALSE;
    if (! bModified)
        return TRUE;

    FILE *f = fopen(sName, "w");
    if (f == NULL)
        return FALSE;
    for (int i = 0; i < saContent.GetSize(); i++) {
        const char *line = (LPCTSTR)saContent.GetAt(i);
        fputs(line, f);
        if (line[strlen(line) - 1] != '\n')
            fputc('\n', f);
    }
    fclose(f);
    return TRUE;
}
	
BOOL KeyValFile::Close()
{
    return TRUE;
}
	
BOOL KeyValFile::Comment(const CString &sVariable)
{
    int nIndex = Lookup(sVariable);
    if (nIndex < 0)
        return FALSE;
    CString sNew("% ");
    sNew = sNew + saContent.GetAt(nIndex);
    saContent.SetAt(nIndex, sNew);
    bModified = TRUE;
    return TRUE;
}
	
BOOL KeyValFile::Uncomment(const CString &sVariable)
{
    int nIndex = Lookup(sVariable, FALSE);
    if (nIndex < 0)
        return FALSE;
    CString sLine = saContent.GetAt(nIndex);
    sLine.TrimLeft(lpszCommentChars);
    sLine.TrimLeft(lpszWhiteSpaceChars);
    saContent.SetAt(nIndex, sLine);
    bModified = TRUE;
    return TRUE;
}

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

