/*++
         Module Name:

            CONSPAWN.C

         Description:

            Serves as an intermediate stub Win32 console application to
            avoid a hanging pipe when redirecting 16-bit console based
            programs (including MS-DOS console based programs and batch
            files) on Window 95 and Windows 98.
            This program is to be launched with redirected standard
            handles. It will launch the command line specified 16-bit
            console based application in the same console, forwarding
            it's own redirected standard handles to the 16-bit child.

--*/    
#include <windows.h>

void main (int argc, char *argv[])
{
   BOOL                bRet = FALSE;
   STARTUPINFO         si   = {0};
   PROCESS_INFORMATION pi   = {0};

   // Make child process use this app's standard files.
   si.cb = sizeof(si);
   si.dwFlags    = STARTF_USESTDHANDLES;
   si.hStdInput  = GetStdHandle (STD_INPUT_HANDLE);
   si.hStdOutput = GetStdHandle (STD_OUTPUT_HANDLE);
   si.hStdError  = GetStdHandle (STD_ERROR_HANDLE);

   bRet = CreateProcess (NULL, argv[1],
                         NULL, NULL,
                         TRUE, 0,
                         NULL, NULL,
                         &si, &pi
                         );
   if (bRet)
   {
      WaitForSingleObject (pi.hProcess, INFINITE);
      CloseHandle (pi.hProcess);
      CloseHandle (pi.hThread);
   }
} 
