// FileDirectory.h: interface for the CFileDirectory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEDIRECTORY_H__9634181C_DBB1_4FA7_A027_274E13F67D95__INCLUDED_)
#define AFX_FILEDIRECTORY_H__9634181C_DBB1_4FA7_A027_274E13F67D95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afx.h>
#include <afxcoll.h>

#include "Win32Util.h"

typedef BOOL (* FILE_ACTION_FUNCTION)( void * parameter, const CString& filename );

class CFileDirectory : public CObject  
{
private:
    // Don't allow canonical behavior (i.e. don't allow this class
    // to be passed by value)
    CFileDirectory( const CFileDirectory& ) {};
    CFileDirectory& operator=( const CFileDirectory& ) { return( *this ); };

protected:
    CString m_Name;

public:

    // Construction
    CFileDirectory();
    // Desctruction
    virtual ~CFileDirectory();

    // Methods
    virtual void Close( void );
    virtual BOOL Open( const CString& directory_name );
    virtual BOOL Read( CStringArray& filenames );
    virtual BOOL Read( FILE_ACTION_FUNCTION action_to_take, void * action_parameter );
    virtual BOOL ReadRecursively( CStringArray& filenames );
    virtual BOOL ReadRecursively( FILE_ACTION_FUNCTION action_to_take, void * action_parameter );

    // Operators
};

#endif // !defined(AFX_FILEDIRECTORY_H__9634181C_DBB1_4FA7_A027_274E13F67D95__INCLUDED_)
/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

