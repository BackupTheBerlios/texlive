// Registry.cpp: implementation of the CRegistry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TeXSetup.h"
#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC( CRegistry, CObject);

static LONG _recursively_delete_all_sub_keys(HKEY key_handle, LPCTSTR key_name)
{
    HKEY child_key_handle = NULL;
    LONG ret = 0;
    LPTSTR temporary_key_name = NULL;

    ret = RegOpenKeyEx(key_handle, key_name, NULL, KEY_ALL_ACCESS, &child_key_handle);

    if (ret != ERROR_SUCCESS) {
        return(ret);
    }

    try {
        temporary_key_name = new TCHAR[ MAX_PATH ];
    }
    catch(...) {
        temporary_key_name = NULL;
    }

    if (temporary_key_name == NULL) {
        return(ERROR_NOT_ENOUGH_MEMORY);
    }

    ret = RegEnumKey(child_key_handle, 0, temporary_key_name, MAX_PATH);
    while(ret == ERROR_SUCCESS) {
        _recursively_delete_all_sub_keys(child_key_handle, temporary_key_name);

        ret = RegEnumKey(child_key_handle, 0, temporary_key_name, MAX_PATH);
    }

    delete [] temporary_key_name;
    temporary_key_name = NULL;
    RegCloseKey(child_key_handle);
    ret = RegDeleteKey(key_handle, key_name);

    return(ret);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegistry::CRegistry()
{
    m_Initialize();
}

CRegistry::~CRegistry()
{

    if (m_RegistryHandle != (HKEY) NULL) {
        Close();
    }

    m_Initialize();
}

void CRegistry::m_Initialize(void)
{

    /*
    ** Make sure everything is zeroed out
    */

    m_ClassName.Empty();
    m_ComputerName.Empty();
    m_KeyName.Empty();
    m_RegistryName.Empty();

    m_KeyHandle                    = (HKEY) NULL;
    m_ErrorCode                    = 0L;
    m_NumberOfSubkeys              = 0;
    m_LongestSubkeyNameLength      = 0;
    m_LongestClassNameLength       = 0;
    m_NumberOfValues               = 0;
    m_LongestValueNameLength       = 0;
    m_LongestValueDataLength       = 0;
    m_SecurityDescriptorLength     = 0;
    m_LastWriteTime.dwLowDateTime  = 0;
    m_LastWriteTime.dwHighDateTime = 0;

    // Thanks go to Chad Christenson (chadc@cwcinc.com) for finding
    // the bug where m_RegistryHandle was never initialized

    m_RegistryHandle               = (HKEY) NULL;
}

BOOL CRegistry::Close(void)
{

    // Thanks go to Gareth Isaac (humbert@tcp.co.uk) for finding a resource leak here

    if (m_KeyHandle != (HKEY) NULL) {
        ::RegCloseKey(m_KeyHandle);
        m_KeyHandle = (HKEY) NULL;
    }

    if (m_RegistryHandle == (HKEY) NULL) {
        return(TRUE);
    }

    m_ErrorCode = ::RegCloseKey(m_RegistryHandle);

    if (m_ErrorCode == ERROR_SUCCESS) {
        m_RegistryHandle = (HKEY) NULL;
        m_Initialize();

        return(TRUE);
    }
    else {
        return(FALSE);
    }
}

BOOL CRegistry::Connect(HKEY key_to_open, LPCTSTR name_of_computer)
{

    // We were passed a pointer, do not trust it

    try {
        /*
        ** name_of_computer can be NULL
        */

        if (key_to_open == (HKEY) keyClassesRoot || key_to_open == (HKEY) keyCurrentUser) {
            if (name_of_computer == NULL) {
                m_RegistryHandle = key_to_open;
                m_ErrorCode      = ERROR_SUCCESS;
            }
            else {
                /*
                ** NT won't allow you to connect to these hives via RegConnectRegistry so we'll just skip that step
                */

                m_ErrorCode = ERROR_INVALID_HANDLE;
            }
        }
        else {
         
            // Thanks to Paul Ostrowski [postrowski@xantel.com] for finding UNICODE bug here
         
            // RegConnectRegistry is not const correct

            m_ErrorCode = ::RegConnectRegistry((LPTSTR) name_of_computer, key_to_open, &m_RegistryHandle);
        }

        if (m_ErrorCode == ERROR_SUCCESS) {
            if (name_of_computer == NULL) {
                TCHAR computer_name[ MAX_PATH ];
                DWORD size = MAX_PATH;

                if (::GetComputerName(computer_name, &size) == FALSE) {
                    m_ComputerName.Empty();
                }
                else {
                    m_ComputerName = computer_name;
                }
            }
            else {
                m_ComputerName = name_of_computer;
            }

            /*
            ** It would be nice to use a switch statement here but I get a "not integral" error!
            */

 if (key_to_open == HKEY_LOCAL_MACHINE) {
     m_RegistryName = TEXT("HKEY_LOCAL_MACHINE");
 }
 else if (key_to_open == HKEY_CLASSES_ROOT) {
     m_RegistryName = TEXT("HKEY_CLASSES_ROOT");
 }
 else if (key_to_open == HKEY_USERS) {
     m_RegistryName = TEXT("HKEY_USERS");
 }
 else if (key_to_open == HKEY_CURRENT_USER) {
     m_RegistryName = TEXT("HKEY_CURRENT_USER");
 }
 else if (key_to_open == HKEY_PERFORMANCE_DATA) {
     m_RegistryName = TEXT("HKEY_PERFORMANCE_DATA");
 }
#if (WINVER >= 0x400)
 else if (key_to_open == HKEY_CURRENT_CONFIG) {
     m_RegistryName = TEXT("HKEY_CURRENT_CONFIG");
 }
 else if (key_to_open == HKEY_DYN_DATA) {
     m_RegistryName = TEXT("HKEY_DYN_DATA");
 }
#endif
 else {
     m_RegistryName = TEXT("Unknown");
 }


 return(TRUE);
        }
        else {
            return(FALSE);
        }
    }
 catch(...) {
     m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
     return(FALSE);
 }
}

BOOL CRegistry::Create(LPCTSTR               name_of_subkey, 
                       LPCTSTR               name_of_class,
                       CreateOptions         options, 
                       CreatePermissions     permissions, 
                       LPSECURITY_ATTRIBUTES security_attributes_p,
                       CreationDisposition * disposition_p)
{
    ASSERT(name_of_subkey != NULL);

    if (name_of_subkey == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    // We were passed a pointer, do not trust it

    try {
        DWORD disposition = 0;

        if (name_of_class == NULL) {
            name_of_class = TEXT(""); // Paul Ostrowski [postrowski@xantel.com]
        }

        if (m_KeyHandle != (HKEY) NULL) {
            ::RegCloseKey(m_KeyHandle);
            m_KeyHandle = (HKEY) NULL;
        }

        m_ErrorCode = ::RegCreateKeyEx(m_RegistryHandle,
                                       name_of_subkey,
                                       (DWORD) 0,
                                       (LPTSTR) name_of_class, // Paul Ostrowski [postrowski@xantel.com]
                                       options,
                                       permissions,
                                       security_attributes_p,
                                       &m_KeyHandle,
                                       &disposition);

        if (m_ErrorCode == ERROR_SUCCESS) {
            if (disposition_p != NULL) {
                *disposition_p = (CreationDisposition) disposition;
            }

            m_KeyName = name_of_subkey;

            return(TRUE);
        }
        else {
            return(FALSE);
        }
    }
    catch(...) {
        m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
        return(FALSE);
    }
}

BOOL CRegistry::DeleteKey(LPCTSTR name_of_key_to_delete)
{

    ASSERT(name_of_key_to_delete != NULL);

    if (name_of_key_to_delete == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    // We were passed a pointer, do not trust it

    try {
        /*
        ** You can't delete a key given a full path. What you have to do is back up one level and then do a delete
        */

        CString full_key_name = name_of_key_to_delete;

        if (full_key_name.Find(TEXT('\\')) == (-1)) {
            /*
            ** User had not given us a full path so assume the name of the key he passed us
            ** is a key off of the current key
            */

            m_ErrorCode = ::_recursively_delete_all_sub_keys(m_KeyHandle, name_of_key_to_delete);
        }
        else {
            int last_back_slash_location = full_key_name.GetLength() - 1;

            /*
            ** We know this loop will succeed because a back slash was found in the above if statement
            */

            while(full_key_name[ last_back_slash_location ] != TEXT('\\')) {
                last_back_slash_location--;
            }

            CString currently_opened_key_name = m_KeyName;

            CString parent_key_name = full_key_name.Left(last_back_slash_location);
            CString child_key_name  = full_key_name.Right((full_key_name.GetLength() - last_back_slash_location) - 1);

            /*
            ** Now we open the parent key and delete the child
            */

            if (Open(parent_key_name) != FALSE) {
                m_ErrorCode = ::_recursively_delete_all_sub_keys(m_KeyHandle, child_key_name);
            }
            else {
                m_KeyName = currently_opened_key_name;
                return(FALSE);
            }
        }

        if (m_ErrorCode == ERROR_SUCCESS) {
            return(TRUE);
        }
        else {
            return(FALSE);
        }
    }
    catch(...) {
        m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
        return(FALSE);
    }
}                  

BOOL CRegistry::DeleteValue(LPCTSTR name_of_value_to_delete)
{

    /*
    ** name_of_value_to_delete can be NULL
    */

    // We were passed a pointer, do not trust it

    try {
        m_ErrorCode = ::RegDeleteValue(m_KeyHandle, (LPTSTR) name_of_value_to_delete);

        if (m_ErrorCode == ERROR_SUCCESS) {
            return(TRUE);
        }
        else {
            return(FALSE);
        }
    }
    catch(...) {
        m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
        return(FALSE);
    }
}

#if defined(_DEBUG)

void CRegistry::Dump(CDumpContext& dump_context) const
{
    CObject::Dump(dump_context);

    dump_context << TEXT("m_KeyHandle = "               ) << m_KeyHandle                << TEXT("\n");
    dump_context << TEXT("m_RegistryHandle = "          ) << m_RegistryHandle           << TEXT("\n");
    dump_context << TEXT("m_ClassName = \""             ) << m_ClassName                << TEXT("\"\n");
    dump_context << TEXT("m_ComputerName = \""          ) << m_ComputerName             << TEXT("\"\n");
    dump_context << TEXT("m_KeyName = \""               ) << m_KeyName                  << TEXT("\"\n");
    dump_context << TEXT("m_RegistryName = \""          ) << m_RegistryName             << TEXT("\"\n");
    dump_context << TEXT("m_NumberOfSubkeys = "         ) << m_NumberOfSubkeys          << TEXT("\n");
    dump_context << TEXT("m_NumberOfValues = "          ) << m_NumberOfValues           << TEXT("\n");
    dump_context << TEXT("m_LongestSubkeyNameLength = " ) << m_LongestSubkeyNameLength  << TEXT("\n");
    dump_context << TEXT("m_LongestClassNameLength = "  ) << m_LongestClassNameLength   << TEXT("\n");
    dump_context << TEXT("m_LongestValueNameLength = "  ) << m_LongestValueNameLength   << TEXT("\n");
    dump_context << TEXT("m_LongestValueDataLength = "  ) << m_LongestValueDataLength   << TEXT("\n");
    dump_context << TEXT("m_SecurityDescriptorLength = ") << m_SecurityDescriptorLength << TEXT("\n");

    // Mike Berger (mberger@smg.seagatesoftware.com) found a bug where there was no
    // << operator for FILETIME structure. Color me stupid for not catching it...

    dump_context << TEXT("m_LastWriteTime.dwLowDateTime  = ") << m_LastWriteTime.dwLowDateTime  << TEXT("\n");
    dump_context << TEXT("m_LastWriteTime.dwHighDateTime = ") << m_LastWriteTime.dwHighDateTime << TEXT("\n");
}

#endif // _DEBUG

BOOL CRegistry::EnumerateKeys(const DWORD subkey_index, CString& subkey_name, CString& class_name)
{

    TCHAR subkey_name_string[ 2048 ];
    TCHAR class_name_string[ 2048 ];

    DWORD size_of_subkey_name_string = DIMENSION_OF(subkey_name_string) - 1;
    DWORD size_of_class_name_string  = DIMENSION_OF(class_name_string ) - 1;

    ::ZeroMemory(subkey_name_string, sizeof(subkey_name_string));
    ::ZeroMemory(class_name_string,  sizeof(class_name_string ));

    m_ErrorCode = ::RegEnumKeyEx(m_KeyHandle, 
                                 subkey_index, 
                                 subkey_name_string, 
                                 &size_of_subkey_name_string,
                                 NULL,
                                 class_name_string,
                                 &size_of_class_name_string,
                                 &m_LastWriteTime);

    if (m_ErrorCode == ERROR_SUCCESS) {
        subkey_name = subkey_name_string;
        class_name  = class_name_string;

        return(TRUE);
    }
    else {
        return(FALSE);
    }
}

BOOL CRegistry::EnumerateValues(const DWORD    value_index, 
                                CString&       name_of_value, 
                                KeyValueTypes& type_code,
                                LPBYTE         data_buffer,
                                DWORD&         size_of_data_buffer)
{

    /*
    ** data_buffer and size_of_data_buffer can be NULL
    */

    DWORD temp_type_code = type_code;

    TCHAR temp_name[ 2048 ];

    ::ZeroMemory(temp_name, sizeof(temp_name));

    DWORD temp_name_size = DIMENSION_OF(temp_name);

    // We were passed a pointer, do not trust it

    try {
        m_ErrorCode = ::RegEnumValue(m_KeyHandle,
                                     value_index,
                                     temp_name,
                                     &temp_name_size,
                                     NULL,
                                     &temp_type_code,
                                     data_buffer,
                                     &size_of_data_buffer);

        if (m_ErrorCode == ERROR_SUCCESS) {
            type_code     = (KeyValueTypes) temp_type_code;
            name_of_value = temp_name;

            return(TRUE);
        }
        else {
            return(FALSE);
        }
    }
    catch(...) {
        m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
        return(FALSE);
    }
}

BOOL CRegistry::Flush(void)
{

    m_ErrorCode = ::RegFlushKey(m_KeyHandle);

    if (m_ErrorCode == ERROR_SUCCESS) {
        return(TRUE);
    }
    else {
        return(FALSE);
    }
}

BOOL CRegistry::GetBinaryValue(LPCTSTR name_of_value, CByteArray& return_array)
{

    ASSERT(name_of_value != NULL);

    if (name_of_value == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    // Thanks go to Chris Hines (ChrisHines@msn.com) for finding
    // a bug here. If you add entries to the key, then the
    // information retrieved via QueryInfo() may be invalid. This
    // will screw you here. So, we must make sure our information
    // is correct before we attempt to *use* the data.

    QueryInfo();

    DWORD size_of_buffer = m_LongestValueDataLength;

    LPBYTE memory_buffer = (LPBYTE) new BYTE[ size_of_buffer ];

    if (memory_buffer == NULL) {
        m_ErrorCode = ::GetLastError();
        return(FALSE);
    }

    BOOL ret = TRUE;

    KeyValueTypes type = typeBinary;

    if (QueryValue(name_of_value, type, memory_buffer, size_of_buffer) != FALSE) {
        /*
        ** We've got data so give it back to the caller
        */

        return_array.RemoveAll();

        DWORD index = 0;

        while(index < size_of_buffer) {
            return_array.Add(memory_buffer[ index ]);
            index++;
        }

        ret = TRUE;
    }
    else {
        ret = FALSE;
    }

    delete [] memory_buffer;

    return(ret);
}

void CRegistry::GetClassName(CString& class_name) const
{
    class_name = m_ClassName;
}

void CRegistry::GetComputerName(CString& computer_name) const
{
    computer_name = m_ComputerName;
}

BOOL CRegistry::GetDoubleWordValue(LPCTSTR name_of_value, DWORD& ret)
{

    ASSERT(name_of_value != NULL);

    if (name_of_value == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    DWORD size_of_buffer = sizeof(DWORD);

    KeyValueTypes type = typeDoubleWord;

    return(QueryValue(name_of_value, type, (LPBYTE) &ret, size_of_buffer));
}

BOOL CRegistry::GetErrorCode(void) const
{
    return(m_ErrorCode);
}

void CRegistry::GetKeyName(CString& key_name) const
{
    key_name = m_KeyName;
}

DWORD CRegistry::GetNumberOfSubkeys(void) const
{
    return(m_NumberOfSubkeys);
}

DWORD CRegistry::GetNumberOfValues(void) const
{
    return(m_NumberOfValues);
}

void CRegistry::GetRegistryName(CString& registry_name) const
{
    registry_name = m_RegistryName;
}

BOOL CRegistry::GetSecurity(const SECURITY_INFORMATION what_you_want_to_know,
                            PSECURITY_DESCRIPTOR       data_buffer,
                            DWORD&                     size_of_data_buffer)
{

    ASSERT(data_buffer != NULL);

    if (data_buffer == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    // We were passed a pointer, do not trust it

    try {
        m_ErrorCode = ::RegGetKeySecurity(m_KeyHandle,
                                          what_you_want_to_know,
                                          data_buffer,
                                          &size_of_data_buffer);

        if (m_ErrorCode == ERROR_SUCCESS) {
            return(TRUE);
        }
        else {
            return(FALSE);
        }
    }
    catch(...) {
        m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
        return(FALSE);
    }
}

BOOL CRegistry::GetStringValue(LPCTSTR name_of_value, CString& return_string)
{

    ASSERT(name_of_value != NULL);

    if (name_of_value == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    TCHAR temp_string[ 2048 ];
    DWORD size_of_buffer = 2048;

    ::ZeroMemory(temp_string, sizeof(temp_string));

    KeyValueTypes type = typeString;

    if (QueryValue(name_of_value, type, (LPBYTE) temp_string, size_of_buffer) != FALSE) {
        return_string = temp_string;
        return(TRUE);
    }
    else {
        return_string.Empty();
        return(FALSE);
    }
}

BOOL CRegistry::GetStringArrayValue(LPCTSTR name_of_value, CStringArray& return_array)
{

    ASSERT(name_of_value != NULL);

    if (name_of_value == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    // Thanks go to Chris Hines (ChrisHines@msn.com) for finding
    // a bug here. If you add entries to the key, then the
    // information retrieved via QueryInfo() may be invalid. This
    // will screw you here. So, we must make sure our information
    // is correct before we attempt to *use* the data.

    QueryInfo();

    DWORD size_of_buffer = m_LongestValueDataLength;

    LPBYTE memory_buffer = (LPBYTE) new BYTE[ size_of_buffer ];

    if (memory_buffer == NULL) {
        m_ErrorCode = ::GetLastError();
        return(FALSE);
    }

    BOOL ret = TRUE;

    KeyValueTypes type = typeMultipleString; // A double NULL terminated string

    if (QueryValue(name_of_value, type, memory_buffer, size_of_buffer) != FALSE) {
        /*
        ** We've got data so give it back to the caller
        */

        LPTSTR strings = (LPTSTR) memory_buffer;

        return_array.RemoveAll();

        while(strings[ 0 ] != 0x00) {
            return_array.Add((LPCTSTR) strings);
            strings += (_tcslen((LPCTSTR) strings) + 1); // Paul Ostrowski [postrowski@xantel.com]
        }

        ret = TRUE;
    }
    else {
        ret = FALSE;
    }

    delete [] memory_buffer;
   
    return(ret);
}

BOOL CRegistry::GetValue(LPCTSTR name_of_value, CByteArray& return_array)
{

    ASSERT(name_of_value != NULL);

    if (name_of_value == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    return(GetBinaryValue(name_of_value, return_array));
}

BOOL CRegistry::GetValue(LPCTSTR name_of_value, DWORD& ret)
{

    ASSERT(name_of_value != NULL);

    if (name_of_value == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    return(GetDoubleWordValue(name_of_value, ret));
}

BOOL CRegistry::GetValue(LPCTSTR name_of_value, CString& return_string)
{

    ASSERT(name_of_value != NULL);

    if (name_of_value == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    return(GetStringValue(name_of_value, return_string));
}

BOOL CRegistry::GetValue(LPCTSTR name_of_value, CStringArray& return_array)
{

    ASSERT(name_of_value != NULL);

    if (name_of_value == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    return(GetStringArrayValue(name_of_value, return_array));
}

BOOL CRegistry::Load(LPCTSTR name_of_subkey, LPCTSTR name_of_file_containing_information)
{

    ASSERT(name_of_subkey != NULL);
    ASSERT(name_of_file_containing_information != NULL);

    if (name_of_subkey == NULL || name_of_file_containing_information == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    // We were passeda pointer, do not trust it

    try {
        m_ErrorCode = ::RegLoadKey(m_RegistryHandle, name_of_subkey, name_of_file_containing_information);

        if (m_ErrorCode == ERROR_SUCCESS) {
            return(TRUE);
        }
        else {
            return(FALSE);
        }
    }
    catch(...) {
        m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
        return(FALSE);
    }
}

BOOL CRegistry::NotifyChange(const HANDLE             event_handle, 
                             const NotifyChangeFilter changes_to_be_reported,
                             const BOOL               this_subkey_or_all_subkeys,
                             const BOOL               wait_for_change_or_signal_event)
{

    m_ErrorCode = ::RegNotifyChangeKeyValue(m_KeyHandle,
                                            this_subkey_or_all_subkeys,
                                            changes_to_be_reported,
                                            event_handle,
                                            wait_for_change_or_signal_event);

    if (m_ErrorCode == ERROR_SUCCESS) {
        return(TRUE);
    }
    else {
        return(FALSE);
    }
}

BOOL CRegistry::Open(LPCTSTR name_of_subkey_to_open, const CreatePermissions security_access_mask)
{

    /*
    ** name_of_subkey_to_open can be NULL
    */

    // We were passed a pointer, do not trust it

    try {
        if (m_KeyHandle != (HKEY) NULL) {
            ::RegCloseKey(m_KeyHandle);
            m_KeyHandle = (HKEY) NULL;
        }

        m_ErrorCode = ::RegOpenKeyEx(m_RegistryHandle, name_of_subkey_to_open, NULL, security_access_mask, &m_KeyHandle);

        if (m_ErrorCode == ERROR_SUCCESS) {
            QueryInfo();
            m_KeyName = name_of_subkey_to_open;

            return(TRUE);
        }
        else {
            return(FALSE);
        }
    }
    catch(...) {
        m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
        return(FALSE);
    }
}

BOOL CRegistry::QueryInfo(void)
{

    TCHAR class_name[ 2048 ];

    ::ZeroMemory(class_name, sizeof(class_name));

    DWORD size_of_class_name = DIMENSION_OF(class_name) - 1;
   
    m_ErrorCode = ::RegQueryInfoKey(m_KeyHandle,
                                    class_name,
                                    &size_of_class_name,
                                    (LPDWORD) NULL,
                                    &m_NumberOfSubkeys,
                                    &m_LongestSubkeyNameLength,
                                    &m_LongestClassNameLength,
                                    &m_NumberOfValues,
                                    &m_LongestValueNameLength,
                                    &m_LongestValueDataLength,
                                    &m_SecurityDescriptorLength,
                                    &m_LastWriteTime);

    if (m_ErrorCode == ERROR_SUCCESS) {
        m_ClassName = class_name;

        return(TRUE);
    }
    else {
        return(FALSE);
    }
}

BOOL CRegistry::QueryValue(LPCTSTR        name_of_value, 
                           KeyValueTypes& value_type, 
                           LPBYTE         address_of_buffer, 
                           DWORD&         size_of_buffer)
{

    ASSERT(name_of_value != NULL);

    /*
    ** address_of_buffer and size_of_buffer can be NULL
    */

    if (name_of_value == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    // We were passed a pointer, do not trust it

    try {
        DWORD temp_data_type = (DWORD) value_type;

        m_ErrorCode = ::RegQueryValueEx(m_KeyHandle,
                                        (LPTSTR) name_of_value,
                                        NULL,
                                        &temp_data_type,
                                        address_of_buffer,
                                        &size_of_buffer);

        if (m_ErrorCode == ERROR_SUCCESS) {
            value_type = (KeyValueTypes) temp_data_type;
            return(TRUE);
        }
        else {
            return(FALSE);
        }
    }
    catch(...) {
        m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
        return(FALSE);
    }
}

BOOL CRegistry::Replace(LPCTSTR name_of_subkey,
                        LPCTSTR name_of_file_with_new_data,
                        LPCTSTR name_of_backup_file)
{

    ASSERT(name_of_subkey             != NULL);
    ASSERT(name_of_file_with_new_data != NULL);
    ASSERT(name_of_backup_file        != NULL);

    if (name_of_subkey             == NULL ||
        name_of_file_with_new_data == NULL ||
        name_of_backup_file        == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    // We were passed a pointer, do not trust it

    try {
        m_ErrorCode = ::RegReplaceKey(m_KeyHandle, 
                                      name_of_subkey,
                                      name_of_file_with_new_data,
                                      name_of_backup_file);

        if (m_ErrorCode == ERROR_SUCCESS) {
            return(TRUE);
        }
        else {
            return(FALSE);
        }
    }
    catch(...) {
        m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
        return(FALSE);
    }
}

BOOL CRegistry::Restore(LPCTSTR name_of_file_holding_saved_tree, const DWORD volatility_flags)
{

    ASSERT(name_of_file_holding_saved_tree != NULL);

    if (name_of_file_holding_saved_tree == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    // We were passed a pointer, do not trust it

    try {
        m_ErrorCode = ::RegRestoreKey(m_KeyHandle,
                                      name_of_file_holding_saved_tree,
                                      volatility_flags);

        if (m_ErrorCode == ERROR_SUCCESS) {
            return(TRUE);
        }
        else {

            // Maybe we need to get permission...
            HANDLE token_handle = NULL;

            DWORD error_code = 0;

            if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token_handle) == FALSE) {
                error_code = ::GetLastError();
                return(FALSE);
            }

            TOKEN_PRIVILEGES token_privileges;

            ::ZeroMemory(&token_privileges, sizeof(token_privileges));

            if (::LookupPrivilegeValue(NULL, SE_RESTORE_NAME, &token_privileges.Privileges[ 0 ].Luid) == FALSE) {
                error_code = ::GetLastError();
                ::CheckedCloseHandle(token_handle);
                token_handle = NULL;
                return(FALSE);
            }

            token_privileges.PrivilegeCount             = 1;
            token_privileges.Privileges[ 0 ].Attributes = SE_PRIVILEGE_ENABLED;

            if (::AdjustTokenPrivileges(token_handle, FALSE, &token_privileges, 0, (PTOKEN_PRIVILEGES) NULL, 0) == FALSE) {
                error_code = ::GetLastError();
                ::CheckedCloseHandle(token_handle);
                token_handle = NULL;
                return(FALSE);
            }

            // Try the restore again

            m_ErrorCode = ::RegRestoreKey(m_KeyHandle,
                                          name_of_file_holding_saved_tree,
                                          volatility_flags);

            // Regardless of the turnout, we're through with the token handle

            ::CheckedCloseHandle(token_handle);
            token_handle = NULL;

            if (m_ErrorCode != ERROR_SUCCESS) {
#if defined(_DEBUG)
                error_code = ::GetLastError();

#endif // _DEBUG
                return(FALSE);
            }

            return(TRUE);
        }
    }
    catch(...) {
        m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
        return(FALSE);
    }
}

BOOL CRegistry::Save(LPCTSTR name_of_file_to_hold_tree, LPSECURITY_ATTRIBUTES security_attributes_p)
{

    ASSERT(name_of_file_to_hold_tree != NULL);

    if (name_of_file_to_hold_tree == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    // We were passed a pointer, do not trust it

    try {
        m_ErrorCode = ::RegSaveKey(m_KeyHandle, name_of_file_to_hold_tree, security_attributes_p);

        if (m_ErrorCode == ERROR_SUCCESS) {
            return(TRUE);
        }
        else {
            return(FALSE);
        }
    }
    catch(...) {
        m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
        return(FALSE);
    }
}

BOOL CRegistry::SetBinaryValue(LPCTSTR name_of_value, const CByteArray& bytes_to_write)
{

    ASSERT(name_of_value != NULL);

    if (name_of_value == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    DWORD size_of_buffer = bytes_to_write.GetSize();

    LPBYTE memory_buffer = NULL;
   
    try {
        memory_buffer = new BYTE[ size_of_buffer ];
    }
    catch(...) {
        memory_buffer = NULL;
    }

    if (memory_buffer == NULL) {
        m_ErrorCode = ::GetLastError();
        return(FALSE);
    }

    DWORD index = 0;

    while(index < size_of_buffer) {
        memory_buffer[ index ] = bytes_to_write.GetAt(index);
        index++;
    }

    BOOL ret = SetValue(name_of_value, typeBinary, memory_buffer, size_of_buffer);

    delete [] memory_buffer;

    return(ret);
}

BOOL CRegistry::SetDoubleWordValue(LPCTSTR name_of_value, DWORD value_to_write)
{

    ASSERT(name_of_value != NULL);

    if (name_of_value == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    return(SetValue(name_of_value, typeDoubleWord, (const PBYTE) &value_to_write, sizeof(DWORD)));
}

BOOL CRegistry::SetSecurity(const SECURITY_INFORMATION& security_information,
                            const PSECURITY_DESCRIPTOR  security_descriptor_p)
{

    ASSERT(security_descriptor_p != NULL);

    if (security_descriptor_p == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    // We were passed a pointer, do not trust it

    try {
        m_ErrorCode = ::RegSetKeySecurity(m_KeyHandle, security_information, security_descriptor_p);

        if (m_ErrorCode == ERROR_SUCCESS) {
            return(TRUE);
        }
        else {
            return(FALSE);
        }
    }
    catch(...) {
        m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
        return(FALSE);
    }
}

BOOL CRegistry::SetStringValue(LPCTSTR name_of_value, const CString& string_value)
{

    ASSERT(name_of_value != NULL);

    if (name_of_value == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    // Thanks to Ravi Kiran (kravi@blr.sni.de) for finding a bug here in the UNICODE build

    return(SetValue(name_of_value, typeString, (const PBYTE) (LPCTSTR) string_value, (string_value.GetLength() + 1) * sizeof(TCHAR)));
}

BOOL CRegistry::SetStringArrayValue(LPCTSTR name_of_value, const CStringArray& string_array)
{

    ASSERT(name_of_value != NULL);

    if (name_of_value == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    DWORD size_of_buffer = 0;

    /*
    ** Find out how big our buffer needs to be...
    */

    int index             = 0;
    int number_of_strings = string_array.GetSize();

    CString temp_string;

    while(index < number_of_strings) {
        temp_string = string_array.GetAt(index);

        // Thanks go to George Koukoulommatis (gemasoft@hol.gr)
        // for finding another UNICODE bug here...

        size_of_buffer += ((temp_string.GetLength() + 1) * sizeof(TCHAR));
        index++;
    }

    /*
    ** Don't forget the second NULL needed for double null terminated strings...
    */

    // Thanks go to George Koukoulommatis (gemasoft@hol.gr)
    // for finding another UNICODE bug here...

    size_of_buffer += sizeof(TCHAR);

    LPBYTE memory_buffer = NULL;
   
    try {
        memory_buffer = new BYTE[ size_of_buffer ];
    }
    catch(...) {
        memory_buffer = NULL;
    }

    if (memory_buffer == NULL) {
        m_ErrorCode = ::GetLastError();
        return(FALSE);
    }

    ::ZeroMemory(memory_buffer, size_of_buffer);

    /*
    ** OK, now add the strings to the memory buffer
    */

    LPTSTR string = (LPTSTR) memory_buffer;

    index             = 0;
    int string_length = 0;

    while(index < number_of_strings) {
        temp_string = string_array.GetAt(index);
        _tcscpy(&string[ string_length ], temp_string);

        // Thanks go to George Koukoulommatis (gemasoft@hol.gr)
        // for finding another UNICODE bug here...

        string_length += ((temp_string.GetLength() + 1) * sizeof(TCHAR));

        index++;
    }

    // Thanks go to George Koukoulommatis (gemasoft@hol.gr)
    // for finding another UNICODE bug here...

    string_length += sizeof(TCHAR);

    BOOL ret = TRUE;

    KeyValueTypes type = typeMultipleString; // A double NULL terminated string

    if (SetValue(name_of_value, type, memory_buffer, size_of_buffer) == FALSE) {
        ret = FALSE;
    }

    delete [] memory_buffer;

    return(ret);
}

BOOL CRegistry::SetValue(LPCTSTR name_of_value, const CByteArray& bytes_to_write)
{

    ASSERT(name_of_value != NULL);

    if (name_of_value == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    return(SetBinaryValue(name_of_value, bytes_to_write));
}

BOOL CRegistry::SetValue(LPCTSTR name_of_value, DWORD value)
{

    ASSERT(name_of_value != NULL);

    if (name_of_value == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    return(SetDoubleWordValue(name_of_value, value));
}

BOOL CRegistry::SetValue(LPCTSTR name_of_value, const CStringArray& strings_to_write)
{

    ASSERT(name_of_value != NULL);

    if (name_of_value == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    return(SetStringArrayValue(name_of_value, strings_to_write));
}

BOOL CRegistry::SetValue(LPCTSTR name_of_value, const CString& string_to_write)
{

    ASSERT(name_of_value != NULL);

    if (name_of_value == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    return(SetStringValue(name_of_value, string_to_write));
}

BOOL CRegistry::SetValue(LPCTSTR             name_of_value, 
                         const KeyValueTypes type_of_value_to_set, 
                         const PBYTE         address_of_value_data, 
                         const DWORD         size_of_data)
{

    ASSERT(name_of_value         != NULL);
    ASSERT(address_of_value_data != NULL);

    if (name_of_value == NULL || address_of_value_data == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    // We were passed a pointer, do not trust it

    try {
        m_ErrorCode = ::RegSetValueEx(m_KeyHandle,
                                      name_of_value,
                                      0,
                                      type_of_value_to_set,
                                      address_of_value_data,
                                      size_of_data);

        if (m_ErrorCode == ERROR_SUCCESS) {
            return(TRUE);
        }
        else {
            return(FALSE);
        }
    }
    catch(...) {
        m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
        return(FALSE);
    }
}

BOOL CRegistry::UnLoad(LPCTSTR name_of_subkey_to_unload)
{

    ASSERT(name_of_subkey_to_unload != NULL);

    if (name_of_subkey_to_unload == NULL) {
        m_ErrorCode = ERROR_INVALID_PARAMETER;
        return(FALSE);
    }

    // We were passed a pointer, do not trust it

    try {
        m_ErrorCode = ::RegUnLoadKey(m_KeyHandle, name_of_subkey_to_unload);

        if (m_ErrorCode == ERROR_SUCCESS) {
            return(TRUE);
        }
        else {
            return(FALSE);
        }
    }
    catch(...) {
        m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
        return(FALSE);
    }
}
/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

