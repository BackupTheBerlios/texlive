// FileDirectory.cpp: implementation of the CFileDirectory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "texsetup.h"
#include "FileDirectory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileDirectory::CFileDirectory()
{

}

CFileDirectory::~CFileDirectory()
{
    Close();
}

void CFileDirectory::Close( void )
{
    m_Name.Empty();
}

BOOL CFileDirectory::Open( const CString& directory_name )
{

    m_Name = directory_name;

    if ( m_Name.GetLength() > 0 ) {
        if ( m_Name.GetAt( m_Name.GetLength() - 1 ) != TEXT( '/' ) &&
             m_Name.GetAt( m_Name.GetLength() - 1 ) != TEXT( '\\' ) ) {
            m_Name += TEXT( "/" );
        }

        return( TRUE );
    }

    return( FALSE ); // User passed us an empty string
}

BOOL CFileDirectory::Read( CStringArray& filenames )
{

    filenames.RemoveAll();

    HANDLE file_find_handle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA find_data;

    ZeroMemory( &find_data, sizeof( find_data ) );

    file_find_handle = ::FindFirstFile( m_Name + TEXT( "*.*" ), &find_data );
    if ( file_find_handle != INVALID_HANDLE_VALUE ) {
        CString complete_filename;
        if ( find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
            // Skip this file
        }
        else {
            complete_filename  = m_Name;
            complete_filename += find_data.cFileName;
            filenames.Add( complete_filename );
        }

        while( ::FindNextFile( file_find_handle, &find_data ) != FALSE ) {
            if ( find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
                // Skip this file
            }
            else {
                complete_filename  = m_Name;
                complete_filename += find_data.cFileName;

                filenames.Add( complete_filename );
            }
        }

        if ( ::FindClose( file_find_handle ) == FALSE ) {
        }

        file_find_handle = INVALID_HANDLE_VALUE;

        return( TRUE );
    }

    return( FALSE );
}

BOOL CFileDirectory::Read( FILE_ACTION_FUNCTION action_to_take, void * action_parameter )
{

    HANDLE file_find_handle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA find_data;

    ZeroMemory( &find_data, sizeof( find_data ) );
    file_find_handle = ::FindFirstFile( m_Name + TEXT( "*.*" ), &find_data );

    // The user passed us a couple of pointers, don't trust them
    CString complete_filename;
    try {
        if ( file_find_handle != INVALID_HANDLE_VALUE ) {
            if ( find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
                // Skip this file
            }
            else {
                complete_filename  = m_Name;
                complete_filename += find_data.cFileName;

                if ( action_to_take( action_parameter, complete_filename ) == FALSE ) {
                    if ( ::FindClose( file_find_handle ) == FALSE ) {
                    }

                    file_find_handle = INVALID_HANDLE_VALUE;

                    return( TRUE );
                }
            }

            while( ::FindNextFile( file_find_handle, &find_data ) != FALSE ) {
                if ( find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
                    // Skip this file
                }
                else {
                    complete_filename  = m_Name;
                    complete_filename += find_data.cFileName;

                    if ( action_to_take( action_parameter, complete_filename ) == FALSE ) {
                        if ( ::FindClose( file_find_handle ) == FALSE ) {
                        }
                        file_find_handle = INVALID_HANDLE_VALUE;
                        return( TRUE );
                    }
                }
            }

            if ( ::FindClose( file_find_handle ) == FALSE ) {
                Win32Error("FindClose()");
            }

            file_find_handle = INVALID_HANDLE_VALUE;
            return( TRUE );
        }
    }
    catch( ... ) {
        // Do Nothing
        AfxMessageBox("Error while reading " + complete_filename);
    }

    return( FALSE );
}

typedef struct _recursive_directory_read_parameters
{
    HANDLE find_file_handle;
    WIN32_FIND_DATA find_data;
    CString complete_filename;
    CString directory_to_open;
    CString new_directory_name;
    CString directory_name_ending_in_a_slash;
}
RECURSIVE_DIRECTORY_READ_PARAMETERS;

void PASCAL __read_recursively( const CString& directory_name, CStringArray& filenames )
{

    RECURSIVE_DIRECTORY_READ_PARAMETERS * parameters_p = reinterpret_cast< RECURSIVE_DIRECTORY_READ_PARAMETERS * >( NULL );

    try {
        parameters_p = new RECURSIVE_DIRECTORY_READ_PARAMETERS;
    }
    catch( ... ) {
        parameters_p = reinterpret_cast< RECURSIVE_DIRECTORY_READ_PARAMETERS * >( NULL );
    }

    if ( parameters_p == NULL ) {
        return;
    }

    ZeroMemory( &parameters_p->find_data, sizeof( parameters_p->find_data ) );
    parameters_p->directory_to_open = directory_name;
    if ( parameters_p->directory_to_open.GetLength() == 0 ) {
        return;
    }

    if ( parameters_p->directory_to_open.GetAt( parameters_p->directory_to_open.GetLength() - 1 ) 
         != TEXT( '/' ) &&
         parameters_p->directory_to_open.GetAt( parameters_p->directory_to_open.GetLength() - 1 ) 
         != TEXT( '\\' ) ) {
        parameters_p->directory_to_open += TEXT( "/" );
    }

    parameters_p->directory_name_ending_in_a_slash = parameters_p->directory_to_open;
    parameters_p->directory_to_open += TEXT( "*.*" );
    parameters_p->find_file_handle = FindFirstFile( parameters_p->directory_to_open, 
                                                    &parameters_p->find_data );

    if ( parameters_p->find_file_handle != INVALID_HANDLE_VALUE ) {
        if ( parameters_p->find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
            if ( _tcscmp( parameters_p->find_data.cFileName, TEXT( "."  ) ) == 0 ||
                 _tcscmp( parameters_p->find_data.cFileName, TEXT( ".." ) ) == 0 ) {
                // Do nothing, to do something would cause infinite recursion and that is a bad thing
            }
            else {
                parameters_p->new_directory_name  = parameters_p->directory_name_ending_in_a_slash;
                parameters_p->new_directory_name += parameters_p->find_data.cFileName;
                __read_recursively( parameters_p->new_directory_name, filenames );
            }
        }
        else {
            parameters_p->complete_filename  = parameters_p->directory_name_ending_in_a_slash;
            parameters_p->complete_filename += parameters_p->find_data.cFileName;
            filenames.Add( parameters_p->complete_filename );
        }

        while( FindNextFile( parameters_p->find_file_handle, &parameters_p->find_data ) != FALSE ) {
            if ( parameters_p->find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
                if ( _tcscmp( parameters_p->find_data.cFileName, TEXT( "."	) ) == 0 ||
                     _tcscmp( parameters_p->find_data.cFileName, TEXT( ".." ) ) == 0 ) {
                    // Do nothing, to do something would cause infinite recursion and that is a bad thing
                }
                else {
                    parameters_p->new_directory_name	 = parameters_p->directory_name_ending_in_a_slash;
                    parameters_p->new_directory_name += parameters_p->find_data.cFileName;
                    __read_recursively( parameters_p->new_directory_name, filenames );
                }
            }
            else {
                parameters_p->complete_filename  = parameters_p->directory_name_ending_in_a_slash;
                parameters_p->complete_filename += parameters_p->find_data.cFileName;
                filenames.Add( parameters_p->complete_filename );
            }
        }

        if ( FindClose( parameters_p->find_file_handle ) == FALSE ) {
            Win32Error("FindClose()");
        }

        parameters_p->find_file_handle = INVALID_HANDLE_VALUE;
    }

    delete parameters_p;
    parameters_p = reinterpret_cast< RECURSIVE_DIRECTORY_READ_PARAMETERS * >( NULL );
}

BOOL PASCAL __read_recursively( const CString& directory_name, FILE_ACTION_FUNCTION action_to_take, void * action_parameter )
{

    RECURSIVE_DIRECTORY_READ_PARAMETERS * parameters_p = reinterpret_cast< RECURSIVE_DIRECTORY_READ_PARAMETERS * >( NULL );

    try {
        parameters_p = new RECURSIVE_DIRECTORY_READ_PARAMETERS;
    }
    catch( ... ) {
        parameters_p = reinterpret_cast< RECURSIVE_DIRECTORY_READ_PARAMETERS * >( NULL );
    }

    if ( parameters_p == NULL ) {
        return( FALSE );
    }

    ZeroMemory( &parameters_p->find_data, sizeof( parameters_p->find_data ) );
    parameters_p->directory_to_open = directory_name;
    if ( parameters_p->directory_to_open.GetLength() == 0 ) {
        return( FALSE );
    }

    if ( parameters_p->directory_to_open.GetAt( parameters_p->directory_to_open.GetLength() - 1 ) 
         != TEXT( '/' ) &&
         parameters_p->directory_to_open.GetAt( parameters_p->directory_to_open.GetLength() - 1 ) 
         != TEXT( '\\' ) ) {
        parameters_p->directory_to_open += TEXT( "/" );
    }

    parameters_p->directory_name_ending_in_a_slash = parameters_p->directory_to_open;
    parameters_p->directory_to_open += TEXT( "*.*" );
    parameters_p->find_file_handle = FindFirstFile( parameters_p->directory_to_open, 
                                                    &parameters_p->find_data );

    if ( parameters_p->find_file_handle != INVALID_HANDLE_VALUE ) {
        if ( parameters_p->find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
            if ( _tcscmp( parameters_p->find_data.cFileName, TEXT( "."  ) ) == 0 ||
                 _tcscmp( parameters_p->find_data.cFileName, TEXT( ".." ) ) == 0 ) {
                // Do nothing, to do something would cause infinite recursion and that is a bad thing
            }
            else {
                parameters_p->new_directory_name  = parameters_p->directory_name_ending_in_a_slash;
                parameters_p->new_directory_name += parameters_p->find_data.cFileName;
                if ( __read_recursively( parameters_p->new_directory_name, 
                                         action_to_take, action_parameter ) == FALSE ) {
                    if ( FindClose( parameters_p->find_file_handle ) == FALSE ) {
                        Win32Error("FindClose()");
                    }

                    delete parameters_p;
                    parameters_p = reinterpret_cast< RECURSIVE_DIRECTORY_READ_PARAMETERS * >( NULL );
                    return( FALSE );
                }
            }
        }
        else {
            parameters_p->complete_filename  = parameters_p->directory_name_ending_in_a_slash;
            parameters_p->complete_filename += parameters_p->find_data.cFileName;

            try {
                if ( action_to_take( action_parameter, parameters_p->complete_filename ) == FALSE ) {
                    if ( FindClose( parameters_p->find_file_handle ) == FALSE ) {
                    }

                    delete parameters_p;
                    parameters_p = reinterpret_cast< RECURSIVE_DIRECTORY_READ_PARAMETERS * >( NULL );

                    return( FALSE );
                }
            }
            catch( ... ) {
            }
        }

        while( FindNextFile( parameters_p->find_file_handle, &parameters_p->find_data ) != FALSE ) {
            if ( parameters_p->find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
                if ( _tcscmp( parameters_p->find_data.cFileName, TEXT( "."	) ) == 0 ||
                     _tcscmp( parameters_p->find_data.cFileName, TEXT( ".." ) ) == 0 ) {
                    // Do nothing, to do something would cause infinite recursion and that is a bad thing
                }
                else {
                    parameters_p->new_directory_name	 = parameters_p->directory_name_ending_in_a_slash;
                    parameters_p->new_directory_name += parameters_p->find_data.cFileName;

                    if ( __read_recursively( parameters_p->new_directory_name, action_to_take, action_parameter ) == FALSE ) {
                        if ( FindClose( parameters_p->find_file_handle ) == FALSE ) {
                            Win32Error("FindClose()");
                        }

                        delete parameters_p;
                        parameters_p = reinterpret_cast< RECURSIVE_DIRECTORY_READ_PARAMETERS * >( NULL );
                        return( FALSE );
                    }
                }
            }
            else {
                parameters_p->complete_filename  = parameters_p->directory_name_ending_in_a_slash;
                parameters_p->complete_filename += parameters_p->find_data.cFileName;

                try {
                    if ( action_to_take( action_parameter, parameters_p->complete_filename ) == FALSE ) {
                        if ( FindClose( parameters_p->find_file_handle ) == FALSE ) {
                        }

                        delete parameters_p;
                        parameters_p = reinterpret_cast< RECURSIVE_DIRECTORY_READ_PARAMETERS * >( NULL );

                        return( FALSE );
                    }
                }
                catch( ... ) {
                }
            }
        }

        if ( FindClose( parameters_p->find_file_handle ) == FALSE ) {
            Win32Error("FindClose()");
        }

        parameters_p->find_file_handle = INVALID_HANDLE_VALUE;
    }

    delete parameters_p;
    parameters_p = reinterpret_cast< RECURSIVE_DIRECTORY_READ_PARAMETERS * >( NULL );
    return( TRUE );
}

BOOL CFileDirectory::ReadRecursively( CStringArray& filenames )
{

    HANDLE file_find_handle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA find_data;

    ZeroMemory( &find_data, sizeof( find_data ) );
    file_find_handle = ::FindFirstFile( m_Name + TEXT( "*.*" ), &find_data );

    if ( file_find_handle != INVALID_HANDLE_VALUE ) {
        CString complete_filename;
        CString new_directory_name;

        if ( find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
            if ( ::_tcscmp( find_data.cFileName, TEXT( "."	 ) ) == 0 ||
                 ::_tcscmp( find_data.cFileName, TEXT( ".." ) ) == 0 ) {
                // Avoid infinite recursion here.
            }
            else {
                new_directory_name = m_Name;
                new_directory_name += find_data.cFileName;
                ::__read_recursively( new_directory_name, filenames );
            }
        }
        else {
            complete_filename  = m_Name;
            complete_filename += find_data.cFileName;
            filenames.Add( complete_filename );
        }

        while( ::FindNextFile( file_find_handle, &find_data ) != FALSE ) {
            if ( find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
                if ( ::_tcscmp( find_data.cFileName, TEXT( "."  ) ) == 0 ||
                     ::_tcscmp( find_data.cFileName, TEXT( ".." ) ) == 0 ) {
                    // Avoid infinite recursion here.
                }
                else {
                    new_directory_name = m_Name;
                    new_directory_name += find_data.cFileName;
                    ::__read_recursively( new_directory_name, filenames );
                }
            }
            else {
                complete_filename  = m_Name;
                complete_filename += find_data.cFileName;
                filenames.Add( complete_filename );
            }
        }

        if ( ::FindClose( file_find_handle ) == FALSE ) {
            Win32Error("FindClose()");
        }

        file_find_handle = INVALID_HANDLE_VALUE;
        return( TRUE );
    }

    return( FALSE );
}

BOOL CFileDirectory::ReadRecursively( FILE_ACTION_FUNCTION action_to_take, void * action_parameter )
{

    HANDLE file_find_handle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA find_data;

    ZeroMemory( &find_data, sizeof( find_data ) );
    file_find_handle = ::FindFirstFile( m_Name + TEXT( "*.*" ), &find_data );

    if ( file_find_handle != INVALID_HANDLE_VALUE ) {
        CString complete_filename;
        CString new_directory_name;
        if ( find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
            if ( ::_tcscmp( find_data.cFileName, TEXT( "."	 ) ) == 0 ||
                 ::_tcscmp( find_data.cFileName, TEXT( ".." ) ) == 0 ) {
                // Avoid infinite recursion here.
            }
            else {
                new_directory_name = m_Name;
                new_directory_name += find_data.cFileName;

                if ( ::__read_recursively( new_directory_name, action_to_take, action_parameter ) == FALSE ) {
                    if ( ::FindClose( file_find_handle ) == FALSE ) {
                        Win32Error("FindClose()");
                    }

                    file_find_handle = INVALID_HANDLE_VALUE;
                    return( TRUE );
                }
            }
        }
        else {
            complete_filename  = m_Name;
            complete_filename += find_data.cFileName;

            // We were passed pointers by the caller, don't trust them

            try {
                if ( action_to_take( action_parameter, complete_filename ) == FALSE ) {
                    if ( ::FindClose( file_find_handle ) == FALSE ) {
                    }

                    file_find_handle = INVALID_HANDLE_VALUE;
                    return( TRUE );
                }
            }
            catch( ... ) {
            }
        }

        while( ::FindNextFile( file_find_handle, &find_data ) != FALSE ) {
            if ( find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
                if ( ::_tcscmp( find_data.cFileName, TEXT( "."  ) ) == 0 ||
                     ::_tcscmp( find_data.cFileName, TEXT( ".." ) ) == 0 ) {
                    // Avoid infinite recursion here.
                }
                else {
                    new_directory_name = m_Name;
                    new_directory_name += find_data.cFileName;

                    if ( ::__read_recursively( new_directory_name, action_to_take, action_parameter ) == FALSE ) {
                        if ( ::FindClose( file_find_handle ) == FALSE ) {
                            Win32Error("FindClose()");
                        }
                        file_find_handle = INVALID_HANDLE_VALUE;
                        return( TRUE );
                    }
                }
            }
            else {
                complete_filename  = m_Name;
                complete_filename += find_data.cFileName;

                // We were passed a couple of pointers by the caller, don't trust them

                try {
                    if ( action_to_take( action_parameter, complete_filename ) == FALSE ) {
                        if ( ::FindClose( file_find_handle ) == FALSE ) {
                            Win32Error("FindClose()");
                        }
                        file_find_handle = INVALID_HANDLE_VALUE;
                        return( TRUE );
                    }
                }
                catch( ... ) {
                }
            }
        }

        if ( ::FindClose( file_find_handle ) == FALSE ) {
            Win32Error("FindClose()");
        }

        file_find_handle = INVALID_HANDLE_VALUE;
        return( TRUE );
    }

    return( FALSE );
}

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

