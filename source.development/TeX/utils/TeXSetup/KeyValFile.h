// KeyValFile.h: interface for the KeyValFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KEYVALFILE_H__DE689DE2_BA3F_4012_9DF3_4D713CB3E646__INCLUDED_)
#define AFX_KEYVALFILE_H__DE689DE2_BA3F_4012_9DF3_4D713CB3E646__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class KeyValFile  
{
public:
    enum eAction {
        BEFORE,
        AFTER,
        REPLACE,
        REPLACEALL
    };

private:
    CStringArray saContent;	// The file in memory
    CString sName;
    BOOL bOk;
    BOOL bModified;
    BOOL IsComment(const CString &);

public:
    KeyValFile();
    virtual ~KeyValFile();
    BOOL Open(const CString &filename);
    BOOL Lookup(const CString &sPattern, CString &sValue);
    int Lookup(const CString &sPattern, BOOL bSkipComments = TRUE);
    BOOL RawReplace(const CString &sOldString, const CString &sNewString, 
                    BOOL bSkipComment = FALSE, int nExpectedPos = -1);
    BOOL Replace(const CString &sVariable, const CString &sValue);
    BOOL Commit();
    BOOL Close();
    BOOL Comment(const CString &sVariable);
    BOOL Uncomment(const CString &sVariable);
};

#endif // !defined(AFX_KEYVALFILE_H__DE689DE2_BA3F_4012_9DF3_4D713CB3E646__INCLUDED_)

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

