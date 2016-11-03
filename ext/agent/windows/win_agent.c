/*
 * Windows in-VM agent of MBA implementation
 * This program is compiled under TDM-GCC 4.9.2 64 bit with the compile flag "-static-libgcc -lws2_32"
 * The IDE used is Orwell Dev-C++ 5.1.1
 *
 *  Copyright (c)   2016 E-lin Ho
 *                  2016 Chiawei Wang
 *                  2016 JuiChien, Jao
 *                  
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <process.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <Shlobj.h>

#include "win_agent.h"

// Handle output and input between socket and agent
static BOOL   g_bRunThread = TRUE;          // Boolean valuable, True for writing to child
static SOCKET g_sClientSocket;              // Socket for connecting, writing, and reading in different function
static SOCKET g_sClientDupSocket;           // Duplicate client socket for execute_cmd
static struct sockaddr_in clientaddr;
static int    slen;

// Log file
static SYSTEMTIME localTime;                // Local system time
static HANDLE g_hLog;                       // Handle to log file
static char   g_sLogPath[MAX_PATH];         // Log fullpath name
static char   g_sLogMessage[SZ_MAX_LOG];    // Log message
static char   g_sLogTime[SZ_MAX_LOG];       // Message of local time
static DWORD  g_dwBytesWritten;             // Used for function 'write_log' to store string length
NTSTATUS (WINAPI *NtSetSystemInformation)(INT, PVOID, ULONG);

/// Write system time and log message to log file.
///
/// \param sLogMessage       message to be writen into log file
///
/// No return value
static inline void write_log() {
 
    sprintf_s( g_sLogTime, SZ_MAX_LOG, "[ %d/%d %02d:%02d ]   ", 
    localTime.wMonth, 
    localTime.wDay, 
    localTime.wHour,
    localTime.wMinute );
    
    // write time
    WriteFile(
        g_hLog,                         // open file handle
        g_sLogTime,                     // start of data to write
        (DWORD)strlen(g_sLogTime),      // number of bytes to write
        &g_dwBytesWritten,              // number of bytes that were written
        NULL);                          // no overlapped structure
        
    // write log message
    WriteFile(
        g_hLog,                         // open file handle
        g_sLogMessage,                    // start of data to write
        (DWORD)strlen(g_sLogMessage),     // number of bytes to write
        &g_dwBytesWritten,              // number of bytes that were written
        NULL);                          // no overlapped structure
        
    FlushFileBuffers( g_hLog );
}

/// Display error message and error code.
///
/// \param pszAPI            error message showing what API is error
/// \param socket_send       send to MBA through socket or not. TRUE for send to MBA
///
/// No return value
static void display_error(char *pszAPI, bool socket_send)
{
    LPVOID lpvMessageBuffer;
    TCHAR szPrintBuffer[512];
    ZeroMemory(szPrintBuffer, sizeof(szPrintBuffer));

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpvMessageBuffer, 0, NULL);

    sprintf_s( g_sLogMessage, 
               SZ_MAX_LOG,
               "ERROR:\tAPI\t\t= %s.\r\n\terror code\t= %d.\r\n\tmessage\t\t= %s.\r\n",
               pszAPI,
               GetLastError(),
               (char *)lpvMessageBuffer );
    write_log();

    if (socket_send)
        sendto(g_sClientSocket, (char *)szPrintBuffer, lstrlen((LPSTR)(szPrintBuffer)), 0, (struct sockaddr*)&clientaddr, slen);

    LocalFree(lpvMessageBuffer);
}

/// Checkout command input. The command list is in enum 'command_type'.
///
/// \param input            fully command from MBA
///
/// Return a enum 'command_type' value.
static int identify_command(char input[SZ_MAX_CMD])
{
    if (strncmp(input, "exec", 4)==0)
        return MBA_CMD_EXEC;
    else if (strncmp(input, "invo", 4)==0)
        return MBA_CMD_INVO;
    else if (strncmp(input, "expo", 4)==0)
        return MBA_CMD_EXPO;
    else if (strncmp(input, "impo", 4)==0)
        return MBA_CMD_IMPO;
    else if (strncmp(input, "logf", 4)==0)
        return MBA_CMD_LOGF;
    else if (strncmp(input, "sync", 4)==0)
        return MBA_CMD_SYNC;
        
    return MBA_CMD_UNKNOWN;
}

/// Receive command from MBA and send to child through pipe 'lpvThreadParam'.
///
/// \param lpvThreadParam    child input pipe
///
/// Return DWORD WINAPI
static DWORD WINAPI get_and_send_input_thread(LPVOID lpvThreadParam)
{
    CHAR cInBuf[SZ_MAX_CMD];            // input buffer, write to child
    CHAR errorbuf[sizeof(MSG_REC_SUCCESS)];
    
    DWORD nBytesRead, 
          nBytesWrote;
          
    HANDLE hPipeWrite = (HANDLE)lpvThreadParam;

    /// Get input from our console and send it to child through the pipe.
    /// Two conditions to break the inifinite loop
    ///      1. The child process is terminated, causing broken pipe
    ///      2. The main thread detects child process termination and close the DUPLICATED socket,
    ///         causing the recv failed with return <= 0
    while( TRUE ) {
        nBytesRead = recvfrom(g_sClientDupSocket, cInBuf, SZ_MAX_CMD, 0, (struct sockaddr*)&clientaddr, &slen);
        if( nBytesRead > 0 ) {

            if( !WriteFile(hPipeWrite, cInBuf, strlen(cInBuf), &nBytesWrote, NULL) ) {
                if (GetLastError() == ERROR_NO_DATA) 
                    break;  // Pipe was closed (normal exit path).
                else
                    display_error("get_and_send_input_thread - WriteFile", TRUE);
            }
        }
        else
            break;
        
    }
    return 0;
}

/// Monitors handle for input. Exits when child exits or pipe breaks.
///
/// \param hPipeRead        child output pipe
///
/// No return value
static void read_and_handle_output(HANDLE hPipeRead)
{
    CHAR  cOutBuf[4096];        // output buffer, read from child
    CHAR  cInBuf[sizeof(MSG_REC_SUCCESS)];
    DWORD nBytesRead;

    // send ready message for agent 'execute' action
    sendto( g_sClientSocket, MSG_EXEC_READY, sizeof(MSG_EXEC_READY), 0, (struct sockaddr*)&clientaddr, slen);
    display_error("MSG_EXEC_READY - Sent", FALSE);

    while (TRUE) {
        
        ZeroMemory(cOutBuf, sizeof(cOutBuf));
        
        // Get the stdout from child process
        if (!ReadFile(hPipeRead, cOutBuf, sizeof(cOutBuf), &nBytesRead, NULL) || !nBytesRead)
        {
            if (GetLastError() == ERROR_BROKEN_PIPE)
                break;  // pipe done - normal exit path.
            else
                display_error("read_and_handle_output - ReadFile", TRUE);
        }

        sendto(g_sClientSocket, (const char*)&nBytesRead, sizeof(nBytesRead), 0, (struct sockaddr*)&clientaddr, slen);
        sendto(g_sClientSocket, cOutBuf, nBytesRead, 0, (struct sockaddr*)&clientaddr, slen);
        
    }
}

/// Sets up STARTUPINFO structure, and launches redirected child. Start command.
///
/// \param sCommand_line    fully command line, after modify
/// \param hChildStdOut     child std out pipe
/// \param hChildStdIn      child std in pipe
/// \param hChildStdErr     child std error pipe
///
/// no return value
static void prep_and_launch_redirected_child(char *sCommand_line, HANDLE hChildStdOut, HANDLE hChildStdIn, HANDLE hChildStdErr)
{
    PROCESS_INFORMATION pi;
    STARTUPINFO si;

    // Set up the start up info struct.
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdOutput = hChildStdOut;
    si.hStdInput = hChildStdIn;
    si.hStdError = hChildStdErr;
    si.wShowWindow = SW_HIDE; 

    // Launch the process that you want to redirect.
    if (!CreateProcess(
        NULL,                // command line
        sCommand_line,        // command line, with argument
        NULL,                // process security attributes
        NULL,                // primary thread security attributes
        TRUE,                // handles are inherited
        CREATE_NO_WINDOW,    // creation flags, CREATE_NO_WINDOW to prevent console window
        NULL,                // use parent's environment
        NULL,                // use parent's current direct
        &si,                // STARTUPINFO pointer
        &pi))                // receives PROCESS_INFORMATION
        display_error("prep_and_launch_redirected_child - CreateProcess", TRUE);

    // Close any unnecessary handles.
    if ( !CloseHandle(pi.hProcess) && !CloseHandle(pi.hThread)) 
        display_error("prep_and_launch_redirected_child - CloseHandle - after redirect", TRUE);
}

/// Do 'execute' instruction.
///
/// \param sCommand_line    fully command line, after modify
///
/// Return AGENT_RET_SUCCESS if succeed, or AGENT_RET_FAIL if fail
static MBA_AGENT_RETURN execute_cmd(char *sCmdline) {
    // Child  : hOutputWrite, hInputRead, hErrorWrite
    // Parent : hOutputRead, hInputWrite
    HANDLE hOutputRead, hOutputWrite;
    HANDLE hInputRead, hInputWrite;
    HANDLE hErrorWrite;
    
    // Thread handler
    HANDLE hThread;
    DWORD dTid;
    
    DWORD dEndSize = 0;

    WSAPROTOCOL_INFO protoInfo;
    
    SECURITY_ATTRIBUTES pipeAttr;

    // Set up the security attributes struct.
    pipeAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    pipeAttr.lpSecurityDescriptor = NULL;
    pipeAttr.bInheritHandle = TRUE;

    // Create the child output pipe.
    if (!CreatePipe(&hOutputRead, &hOutputWrite, &pipeAttr, 0)) {
        display_error("execute_cmd - CreatePipe", TRUE);
        return AGENT_RET_FAIL;
    }

    // Create a duplicate of the output write handle for the std error write handle.
    // This is necessary in case the child application closes one of its std output handles.
    if ( !DuplicateHandle(
            GetCurrentProcess(), hOutputWrite,
            GetCurrentProcess(), &hErrorWrite,
            0, TRUE, DUPLICATE_SAME_ACCESS) ) {
                display_error("execute_cmd - DuplicateHandle : hErrorWrite -> hOutputWrite", TRUE);
                return AGENT_RET_FAIL;
            }
        

    // Create the child input pipe.
    if ( !CreatePipe( &hInputRead, &hInputWrite, &pipeAttr,0) ){
        display_error("execute_cmd - CreatePipe", TRUE);
        return AGENT_RET_FAIL;
    }

    // Ensure the handle for reading from child stdout pipe is not inherited
    if ( !SetHandleInformation( hOutputRead, HANDLE_FLAG_INHERIT, 0) ) {
        display_error("execute_cmd - SetHandleInformation : Child read", TRUE);
        return AGENT_RET_FAIL;
    }

    // Ensure the handle for writing to child stdin pipe is not inherited
    if ( !SetHandleInformation( hInputWrite, HANDLE_FLAG_INHERIT, 0) ) {
        display_error("execute_cmd - SetHandleInformation : Child write", TRUE);
        return AGENT_RET_FAIL;
    }
        
    // Sets up STARTUPINFO structure, and launches redirected child.
    prep_and_launch_redirected_child(sCmdline, hOutputWrite, hInputRead, hErrorWrite);

    // Close pipe handles (do not continue to modify in the parent).
    if (!CloseHandle(hOutputWrite)) {
        display_error("execute_cmd - CloseHandle : Child Write", TRUE);
        return AGENT_RET_FAIL;
    }
    if (!CloseHandle(hInputRead)) {
        display_error("execute_cmd - CloseHandle : Child Read", TRUE);
        return AGENT_RET_FAIL;
    }
    if (!CloseHandle(hErrorWrite)) {
        display_error("execute_cmd - CloseHandle : Child Error", TRUE);
        return AGENT_RET_FAIL;
    }

    // Duplicate ClientSocket for thread
    WSADuplicateSocket( g_sClientSocket, GetCurrentProcessId(), &protoInfo );
    g_sClientDupSocket = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, &protoInfo, 0, 0 );
    if( g_sClientDupSocket == INVALID_SOCKET ) {
        display_error("execute_cmd - WSASocket : Dup ClientSocket", TRUE );
        return AGENT_RET_FAIL;
    }

    // Launch the thread that gets the input and sends it to the child.
    hThread = CreateThread(
        NULL,                       // a pointer to a SECURITY_ATTRIBUTES structure
        0,                          // the initial size of the stack, in bytes
        get_and_send_input_thread,  // a pointer to the application-defined function to be executed by the thread
        (LPVOID)hInputWrite,        // a pointer to a variable to be passed to the thread.
        0,                          // the flags that control the creation of the thread
        &dTid);                      // a pointer to a variable that receives the thread identifier.
                                    //      If this parameter is NULL, the thread identifier is not returned.
    if (hThread == NULL) {
        display_error("execute_cmd - CreateThread : Write", TRUE);
        return AGENT_RET_FAIL;
    }

    // Read the child's output
    read_and_handle_output( hOutputRead );
    // Redirection is complete 

    // Tell the thread to exit and wait for thread to die.
    closesocket( g_sClientDupSocket );
    
    // send out the zero-sized message to terminate agent 'exec' action
    sendto(g_sClientSocket, (const char*)&dEndSize, sizeof(dEndSize), 0, (struct sockaddr*)&clientaddr, slen);

    // Wait Thread which keep receiving & forwarding commands
    if (WaitForSingleObject(hThread, INFINITE) == WAIT_FAILED) {
        display_error("WaitForSingleObject", TRUE);
        return AGENT_RET_FAIL;
    }

    // Close input and output handle
    if (!CloseHandle(hOutputRead)) {
        display_error("execute_cmd - CloseHandle : hOutputRead", TRUE);
        return AGENT_RET_FAIL;
    }
    if (!CloseHandle(hInputWrite)) {
        display_error("execute_cmd - CloseHandle : hInputWrite", TRUE);
        return AGENT_RET_FAIL;
    }
    return AGENT_RET_SUCCESS;
}

/// Do 'invoke' instruction.
///
/// \param sCommand_line    fully command line, after modify
///
/// Return AGENT_RET_SUCCESS if succeed, or AGENT_RET_FAIL if fail
static MBA_AGENT_RETURN invoke_cmd(char *sCmdline) {
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    if ( CreateProcess(NULL, sCmdline, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi) == TRUE ) {
        sendto(g_sClientSocket, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, slen);
        return AGENT_RET_SUCCESS;
    }
    else {
        sendto(g_sClientSocket, MSG_REC_FAIL, sizeof(MSG_REC_FAIL), 0, (struct sockaddr*)&clientaddr, slen);
        return AGENT_RET_FAIL;
    }
}

/// Do 'import' instruction.
///
/// \param filepath         fully file path, after modify. The file receive from MBA saving in the path
/// \param sClientSocket    client socket, in order to receive file data
///
/// Return AGENT_RET_SUCCESS if succeed, or AGENT_RET_FAIL if fail
static MBA_AGENT_RETURN import_cmd(char *filePath)
{
    HANDLE hFile;                   // handle the import file

    DWORDLONG nBytesRead;
    DWORDLONG nBytesWrite;
    DWORDLONG nErrorBytesRead;

    DWORDLONG fileSize;
    DWORDLONG fileSizeStored = 0;

    DWORD recvRound;
    DWORD ptrSetFile;
    
    char fBuf[SZ_MAX_FILECHUNK];
    char errorbuf[sizeof(MSG_REC_SUCCESS)];
    
    // Get total file size messege from client
    nBytesRead = recvfrom( g_sClientSocket, (char*)&fileSize, sizeof(fileSize), 0, (struct sockaddr*)&clientaddr, &slen );
    if ( nBytesRead != sizeof(fileSize) ) {
        display_error("import_cmd - Receive - can't receive fileszie", FALSE);
        return AGENT_RET_FAIL;
    }
   
    sprintf_s(g_sLogMessage, SZ_MAX_LOG, "filePath:[%s]\r\n", filePath);
    write_log();
    
    sprintf_s(g_sLogMessage, SZ_MAX_LOG, "Total file size : %ld bytes\r\n", fileSize);
    write_log();

    hFile = CreateFile(
            filePath,                                           // name of the Write
            GENERIC_WRITE,                                      // open for writing
            0,                                                  // do not share
            NULL,                                               // default security
            CREATE_ALWAYS,                                      // create new file only
            FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING,   // non-buffering I/O
            NULL );                                             // no attr. template

    if (g_hLog == INVALID_HANDLE_VALUE) {
        display_error("import_cmd - CreateFile - can't open file", FALSE);
        sendto(g_sClientSocket, MSG_REC_FAIL, sizeof(MSG_REC_FAIL), 0, (struct sockaddr*)&clientaddr, slen);
        return AGENT_RET_FAIL;
    }
    else
        sendto(g_sClientSocket, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, slen);
    
    // caculate how many rounds to receive the file content;
    recvRound = fileSize / SZ_MAX_FILECHUNK;

    // start to fetch & store the QEMU file
    while (recvRound) {

        // --------Check client can read file successfully-------- //
        nErrorBytesRead = recvfrom( g_sClientSocket, errorbuf, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, &slen );
        if ( nErrorBytesRead != sizeof(MSG_REC_SUCCESS) ) {
            display_error("import_cmd - CheckClient - can't check client read file successfully", FALSE);
            CloseHandle(hFile);
            return AGENT_RET_FAIL;
        }
        if ( strncmp(errorbuf, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS)) != 0 ) {
            display_error("import_cmd - CheckClient - client can't read file", FALSE);
            CloseHandle(hFile);
            return AGENT_RET_FAIL;
        }
    
        // Receive file contents
        nBytesRead = recvfrom( g_sClientSocket, fBuf, SZ_MAX_FILECHUNK, 0, (struct sockaddr*)&clientaddr, &slen );
        if( nBytesRead != SZ_MAX_FILECHUNK ) {
            display_error("import_cmd - recv", TRUE);
            CloseHandle(hFile);
            return AGENT_RET_FAIL;
        }

        // Write file contents
        if( WriteFile( hFile, fBuf, nBytesRead, (LPDWORD)&nBytesWrite, NULL ) == FALSE ) {
            display_error("import_cmd - WriteFile", FALSE);
            sendto(g_sClientSocket, MSG_REC_FAIL, sizeof(MSG_REC_FAIL), 0, (struct sockaddr*)&clientaddr, slen);
            CloseHandle(hFile);
            return AGENT_RET_FAIL;
        }
        else
            sendto(g_sClientSocket, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, slen);

        sprintf_s(g_sLogMessage, SZ_MAX_LOG, "read %ld bytes, fwrite %ld bytes\r\n", nBytesRead, nBytesWrite);
        write_log();

        fileSizeStored += nBytesWrite;

        --recvRound;
    }

    // perform the last round of file write in times of sector size (for non-buffering write)
    nBytesRead = fileSize % SZ_MAX_FILECHUNK;
    if( nBytesRead ) {
        
        // --------Check client can read file successfully-------- //
        nErrorBytesRead = recvfrom( g_sClientSocket, errorbuf, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, &slen );
        if ( nErrorBytesRead != sizeof(MSG_REC_SUCCESS) ) {
            display_error("import_cmd - CheckClient - can't check client read file successfully", FALSE);
            CloseHandle(hFile);
            return AGENT_RET_FAIL;
        }
        if ( strncmp(errorbuf, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS)) != 0 ) {
            display_error("import_cmd - CheckClient - client can't read file", FALSE);
            CloseHandle(hFile);
            return AGENT_RET_FAIL;
        }
    
        ZeroMemory( fBuf, SZ_MAX_FILECHUNK );
        if( nBytesRead != recvfrom( g_sClientSocket, fBuf, nBytesRead, 0, (struct sockaddr*)&clientaddr, &slen ) ) {
            display_error("import_cmd - recv", TRUE);
            CloseHandle(hFile);
            return AGENT_RET_FAIL;
        }
        //nBytesRead = recv( g_sClientSocket, fBuf, nBytesRead, 0 );
        
        // Write file contents
        if( WriteFile(hFile, fBuf, SZ_MAX_FILECHUNK, (LPDWORD)&nBytesWrite, NULL) == FALSE ) {
            display_error("import_cmd - WriteFile", TRUE);
            sendto(g_sClientSocket, MSG_REC_FAIL, sizeof(MSG_REC_FAIL), 0, (struct sockaddr*)&clientaddr, slen);
            CloseHandle(hFile);
            return AGENT_RET_FAIL;
        }
        else
            sendto(g_sClientSocket, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, slen);

        fileSizeStored += nBytesRead;

        // close & re-open file to set the true end of file
        // as we try to minimize the cache activity
        CloseHandle( hFile );
        hFile = CreateFile( filePath, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );
        
        // Set file pointer 
        ptrSetFile = SetFilePointer( hFile, fileSizeStored, NULL, FILE_BEGIN );
        if ( ptrSetFile ==  INVALID_SET_FILE_POINTER ) {
            display_error("expolog_cmd - GetFileSizeEx", FALSE);
            sendto(g_sClientSocket, MSG_REC_FAIL, sizeof(MSG_REC_FAIL), 0, (struct sockaddr*)&clientaddr, slen);
            return AGENT_RET_FAIL;
        }
        else
            sendto(g_sClientSocket, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, slen);
        SetEndOfFile( hFile );
    }
    
    sprintf_s(g_sLogMessage, SZ_MAX_LOG, "Total stored size : %ld bytes\r\n", fileSizeStored);
    write_log();

    CloseHandle(hFile);

    return AGENT_RET_SUCCESS;
}

/// Do 'export' instruction.
///
/// \param filepath         fully file path, after modify. The file in the path send to MBA
///
/// Return AGENT_RET_SUCCESS if succeed, or AGENT_RET_FAIL if fail
static MBA_AGENT_RETURN export_cmd(char *filePath)
{
    HANDLE hFile;           // handle the export file
    
    DWORDLONG nBytesRead;
    DWORDLONG nBytesWrite;
    DWORDLONG nBytesWriteOneTime;
    DWORDLONG nErrorBytesRead;

    DWORD      sendRound;                // rounds of content sent
    
    LARGE_INTEGER fileSize;     // store the file size (8 bytes)


    char fBuf[SZ_MAX_FILECHUNK];   // export buffer
    char errorbuf[sizeof(MSG_REC_SUCCESS)];
    
    // Open File
    hFile = CreateFile(
            filePath,                                           // path of export file
            GENERIC_READ,                                       // open for reading
            FILE_SHARE_READ,                                    // share for reading
            NULL,                                               // default security
            OPEN_EXISTING,                                      // existing file only
            FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING,   // nonbuffering
            NULL);                                              // no attr. template
        
    if (hFile == INVALID_HANDLE_VALUE) {
        display_error("export_cmd - CreateFile", TRUE);
        sendto(g_sClientSocket, MSG_REC_FAIL, sizeof(MSG_REC_FAIL), 0, (struct sockaddr*)&clientaddr, slen);
        return AGENT_RET_FAIL;
    }
    else
        sendto(g_sClientSocket, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, slen);
    
    // Get file size
    if (!GetFileSizeEx(hFile, &fileSize)) {
        display_error("export_cmd - GetFileSizeEx", TRUE);
        sendto(g_sClientSocket, MSG_REC_FAIL, sizeof(MSG_REC_FAIL), 0, (struct sockaddr*)&clientaddr, slen);
        CloseHandle(hFile);
        return AGENT_RET_FAIL;
    }
    else
        sendto(g_sClientSocket, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, slen);
            
    // Show information and sendto file size to client
    sprintf_s(g_sLogMessage, SZ_MAX_LOG, "filePath:[%s]\r\n", filePath);
    write_log();

    sprintf_s(g_sLogMessage, SZ_MAX_LOG, "\"%s\" has %lld Bytes.\r\n", filePath, fileSize);
    write_log();
    
    // Send file size
    nBytesWrite = sendto(g_sClientSocket, (const char*)&fileSize.QuadPart, sizeof(fileSize.QuadPart), 0, (struct sockaddr*)&clientaddr, slen);
    if( nBytesWrite != sizeof(fileSize.QuadPart) ) {
        display_error("export_cmd - sendto", TRUE);
        CloseHandle(hFile);
        return AGENT_RET_FAIL;
    }

    // --------Check if client can open file successfully-------- //
    nErrorBytesRead = recvfrom( g_sClientSocket, errorbuf, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, &slen );
    if ( nErrorBytesRead != sizeof(MSG_REC_SUCCESS) ) {
        display_error("export_cmd - CheckClient - can't check client open file successfully", FALSE);
        CloseHandle(hFile);
        return AGENT_RET_FAIL;
    }
    if ( strncmp(errorbuf, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS)) != 0 ) {
        display_error("export_cmd - CheckClient - client can't open file", FALSE);
        CloseHandle(hFile);
        return AGENT_RET_FAIL;
    }
    
    // calculate how many rounds to sendto file content
    sendRound = fileSize.QuadPart / SZ_MAX_FILECHUNK;

    // start to deliver the file content to QEMU
    nBytesWrite = 0;
    while (sendRound) {

        // Read file contents
        if( ReadFile(hFile, fBuf, SZ_MAX_FILECHUNK, (LPDWORD)&nBytesRead, NULL) == 0 ) {
            display_error( "export_cmd - ReadFile", TRUE);
            sendto(g_sClientSocket, MSG_REC_FAIL, sizeof(MSG_REC_FAIL), 0, (struct sockaddr*)&clientaddr, slen);
            return AGENT_RET_FAIL;
        }
        else
            sendto(g_sClientSocket, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, slen);
        
        // Send file contents
        nBytesWriteOneTime = sendto(g_sClientSocket, fBuf, nBytesRead, 0, (struct sockaddr*)&clientaddr, slen);
        if ( nBytesWriteOneTime != nBytesRead ){
            display_error("export_cmd - CheckClient - can't sendto file successfully", FALSE);
            CloseHandle(hFile);
            return AGENT_RET_FAIL;
        }
            nBytesWrite += nBytesWriteOneTime;
        
        // --------Check if client can write file successfully-------- //
        nErrorBytesRead = recvfrom( g_sClientSocket, errorbuf, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, &slen );
        if ( nErrorBytesRead != sizeof(MSG_REC_SUCCESS) ) {
            display_error("export_cmd - CheckClient - can't check client write file successfully", FALSE);
            CloseHandle(hFile);
            return AGENT_RET_FAIL;
        }
        if ( strncmp(errorbuf, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS)) != 0 ) {
            display_error("export_cmd - CheckClient - client can't write file", FALSE);
            CloseHandle(hFile);
            return AGENT_RET_FAIL;
        }
        
        sprintf_s(g_sLogMessage, SZ_MAX_LOG, "fread %ld bytes, Sending out %zd bytes\r\n", nBytesRead, nBytesWrite);
        write_log();
        
        --sendRound;
    }
    CloseHandle(hFile);

    // sendto the remaining file content
    nBytesRead = fileSize.QuadPart % SZ_MAX_FILECHUNK;
    if( nBytesRead ) {

        // Re-open the file without no-buffering restriction
        hFile = CreateFile(
            filePath,                   // name of the write
            GENERIC_READ,               // open for reading
            FILE_SHARE_READ,            // share for reading
            NULL,                       // default security
            OPEN_EXISTING,              // existing file only
            FILE_ATTRIBUTE_NORMAL,      // normal file
            NULL);                      // no attr. template

        if(hFile == INVALID_HANDLE_VALUE) {
            display_error( "export_cmd - CreateFile", TRUE);
            sendto(g_sClientSocket, MSG_REC_FAIL, sizeof(MSG_REC_FAIL), 0, (struct sockaddr*)&clientaddr, slen);
            return AGENT_RET_FAIL;
        }
        else
            sendto(g_sClientSocket, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, slen);

        // move the file pointer to the last-read position
        SetFilePointer(hFile, nBytesWrite, NULL, FILE_BEGIN);

        // read the remaining file content
        if( ReadFile(hFile, fBuf, nBytesRead, (LPDWORD)&nBytesRead, NULL) == 0 ) {
            display_error("export_cmd - ReadFile", TRUE);
            sendto(g_sClientSocket, MSG_REC_FAIL, sizeof(MSG_REC_FAIL), 0, (struct sockaddr*)&clientaddr, slen);
            CloseHandle(hFile);
            return AGENT_RET_FAIL;
        }
        else
            sendto(g_sClientSocket, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, slen);
        
        // sendto the content
        nBytesWriteOneTime = sendto(g_sClientSocket, fBuf, nBytesRead, 0, (struct sockaddr*)&clientaddr, slen);
        if ( nBytesWriteOneTime != nBytesRead ){
            display_error("export_cmd - CheckClient - can't sendto file successfully", FALSE);
            CloseHandle(hFile);
            return AGENT_RET_FAIL;
        }
            nBytesWrite += nBytesWriteOneTime;

        // --------Check if client can write file successfully-------- //
        nErrorBytesRead = recvfrom( g_sClientSocket, errorbuf, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, &slen );
        if ( nErrorBytesRead != sizeof(MSG_REC_SUCCESS) ) {
            display_error("export_cmd - CheckClient - can't check client write file successfully", FALSE);
            CloseHandle(hFile);
            return AGENT_RET_FAIL;
        }
        if ( strncmp(errorbuf, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS)) != 0 ) {
            display_error("export_cmd - CheckClient - client can't write file", FALSE);
            CloseHandle(hFile);
            return AGENT_RET_FAIL;
        }
        
        CloseHandle(hFile);
    }
 
    sprintf_s(g_sLogMessage, SZ_MAX_LOG, "File size: %ld bytes, Total sendto: %ld bytes\r\n", fileSize.QuadPart, nBytesWrite);
    write_log();

    return AGENT_RET_SUCCESS;
}

/// Do 'logf' instruction.
/// 
/// Return AGENT_RET_SUCCESS if succeed, or AGENT_RET_FAIL if fail
static MBA_AGENT_RETURN expolog_cmd( void )
{
    LARGE_INTEGER fileSize;
    
    DWORDLONG nBytesRead;
    DWORDLONG nBytesWrite;
    DWORDLONG nBytesWriteOneTime;
    DWORDLONG nErrorBytesRead;
    DWORD      sendRound;                // rounds of content sent
    DWORD      ptrSetFile;
    
    HANDLE hLog;
    
    char fBuf[SZ_MAX_FILECHUNK];   // export buffer
    char errorbuf[sizeof(MSG_REC_SUCCESS)];
	
	struct sockaddr_in clientaddr;
    
    // Duplicate the global log handle and check the result
    if ( !DuplicateHandle( 
          GetCurrentProcess(),
          g_hLog,
          GetCurrentProcess(),
          &hLog,
          0,
          FALSE,
          DUPLICATE_SAME_ACCESS ) ) {
        display_error("expolog_cmd - GetFileSizeEx", FALSE);
        sendto(g_sClientSocket, MSG_REC_FAIL, sizeof(MSG_REC_FAIL), 0, (struct sockaddr*)&clientaddr, slen);
        return AGENT_RET_FAIL;
    }
    else
        sendto(g_sClientSocket, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, slen);
    
    // Set the file pointer of hLog and check the result
    ptrSetFile = SetFilePointer(hLog, 0, NULL, FILE_BEGIN);
    if ( ptrSetFile ==  INVALID_SET_FILE_POINTER ) {
        display_error("expolog_cmd - GetFileSizeEx", FALSE);
        sendto(g_sClientSocket, MSG_REC_FAIL, sizeof(MSG_REC_FAIL), 0, (struct sockaddr*)&clientaddr, slen);
        return AGENT_RET_FAIL;
    }
    else
        sendto(g_sClientSocket, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, slen);
    
    // Read log file size
    if (!GetFileSizeEx(hLog, &fileSize)) {
        display_error("expolog_cmd - GetFileSizeEx", FALSE);
        sendto(g_sClientSocket, MSG_REC_FAIL, sizeof(MSG_REC_FAIL), 0, (struct sockaddr*)&clientaddr, slen);
        CloseHandle( hLog );
        return AGENT_RET_FAIL;
    }
    else
        sendto(g_sClientSocket, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, slen);
    
    // Send log file size
    nBytesWrite = sendto(g_sClientSocket, (const char*)&fileSize.QuadPart, sizeof(fileSize.QuadPart), 0, (struct sockaddr*)&clientaddr, slen);
    if( nBytesWrite != sizeof(fileSize.QuadPart) ) {
        display_error("expolog_cmd - send", TRUE);
        CloseHandle( hLog );
        return AGENT_RET_FAIL;
    }
    
    // --------Check if client can open file successfully-------- //
    nErrorBytesRead = recvfrom( g_sClientSocket, errorbuf, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, &slen );
    if ( nErrorBytesRead != sizeof(MSG_REC_SUCCESS) ) {
        display_error("export_cmd - CheckClient - can't check client open file successfully", FALSE);
        CloseHandle( hLog );
        return AGENT_RET_FAIL;
    }
    if ( strncmp(errorbuf, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS)) != 0 ) {
        display_error("export_cmd - CheckClient - client can't open file", FALSE);
        CloseHandle( hLog );
        return AGENT_RET_FAIL;
    }
    
    // calculate how many rounds to send file content
    sendRound = fileSize.QuadPart / SZ_MAX_FILECHUNK;

    // start to deliver the file content to QEMU
    nBytesWrite = 0;
    while (sendRound) {

        // Read log file contents
        if( ReadFile(hLog, fBuf, SZ_MAX_FILECHUNK, (LPDWORD)&nBytesRead, NULL) == 0 ) {
            display_error( "expolog_cmd - ReadFile", TRUE);
            sendto(g_sClientSocket, MSG_REC_FAIL, sizeof(MSG_REC_FAIL), 0, (struct sockaddr*)&clientaddr, slen);
            CloseHandle( hLog );
            return AGENT_RET_FAIL;
        }
        else
            sendto(g_sClientSocket, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, slen);
            
        // send the contents
        nBytesWriteOneTime = sendto(g_sClientSocket, fBuf, nBytesRead, 0, (struct sockaddr*)&clientaddr, slen);
        if ( nBytesWriteOneTime != nBytesRead ){
            display_error("export_cmd - CheckClient - can't send file successfully", FALSE);
            CloseHandle( hLog );
            return AGENT_RET_FAIL;
        }
        nBytesWrite += nBytesWriteOneTime;
        
        // --------Check if client can write file successfully-------- //
        nErrorBytesRead = recvfrom( g_sClientSocket, errorbuf, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, &slen );
        if ( nErrorBytesRead != sizeof(MSG_REC_SUCCESS) ) {
            display_error("export_cmd - CheckClient - can't check client write file successfully", FALSE);
            CloseHandle( hLog );
            return AGENT_RET_FAIL;
        }
        if ( strncmp(errorbuf, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS)) != 0 ) {
            display_error("export_cmd - CheckClient - client can't write file", FALSE);
            CloseHandle( hLog );
            return AGENT_RET_FAIL;
        }
        
        sprintf_s(g_sLogMessage, SZ_MAX_LOG, "fread %ld bytes, Sending out %zd bytes\r\n", nBytesRead, nBytesWrite);
        write_log();
        
        --sendRound;
    }

    // send the remaining file content
    nBytesRead = fileSize.QuadPart % SZ_MAX_FILECHUNK;
    if( nBytesRead ) {

        // read the remaining file contents
        if( ReadFile(hLog, fBuf, nBytesRead, (LPDWORD)&nBytesRead, NULL) == 0 ) {
            display_error("expolog_cmd - ReadFile", TRUE);
            sendto(g_sClientSocket, MSG_REC_FAIL, sizeof(MSG_REC_FAIL), 0, (struct sockaddr*)&clientaddr, slen);
            CloseHandle( hLog );
            return AGENT_RET_FAIL;
        }
        else
            sendto(g_sClientSocket, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, slen);
        
        // send the contents
        nBytesWriteOneTime = sendto(g_sClientSocket, fBuf, nBytesRead, 0, (struct sockaddr*)&clientaddr, slen);
        if ( nBytesWriteOneTime != nBytesRead ){
            display_error("export_cmd - CheckClient - can't send file successfully", FALSE);
            CloseHandle( hLog );
            return AGENT_RET_FAIL;
        }
        nBytesWrite += nBytesWriteOneTime;
        
        // --------Check if client can write file successfully-------- //
        nErrorBytesRead = recvfrom( g_sClientSocket, errorbuf, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, &slen );
        if ( nErrorBytesRead != sizeof(MSG_REC_SUCCESS) ) {
            display_error("export_cmd - CheckClient - can't check client write file successfully", FALSE);
            CloseHandle( hLog );
            return AGENT_RET_FAIL;
        }
        if ( strncmp(errorbuf, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS)) != 0 ) {
            display_error("export_cmd - CheckClient - client can't write file", FALSE);
            CloseHandle( hLog );
            return AGENT_RET_FAIL;
        }
        
    }
    
    CloseHandle( hLog );
    
    return AGENT_RET_SUCCESS;
}

/// Do 'sync' instruction.
/// 
/// Return AGENT_RET_SUCCESS if succeed, or AGENT_RET_FAIL if fail
static MBA_AGENT_RETURN sync_cmd( void )
{
	// file buffer flush
	int sysinfo = 4,
	    retVal;
	HANDLE hVol;
	
	hVol = CreateFile(  "\\\\.\\C:",
	           	        GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ | FILE_SHARE_WRITE,
	                    NULL, 
						OPEN_EXISTING, 
						0,
						NULL );
						
	if ( hVol == INVALID_HANDLE_VALUE ){
		display_error("expolog_cmd - [COMMAND ERROR] Flush Openfile Error", FALSE);
        sendto(g_sClientSocket, MSG_REC_FAIL, sizeof(MSG_REC_FAIL), 0, (struct sockaddr*)&clientaddr, slen);
        return AGENT_RET_FAIL;
	}
	else
		sendto(g_sClientSocket, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, slen);
	
	retVal = FlushFileBuffers( hVol );
	if( retVal == 0 ){
        display_error("expolog_cmd - [COMMAND ERROR] Flush Openfile Error", FALSE);
        sendto(g_sClientSocket, MSG_REC_FAIL, sizeof(MSG_REC_FAIL), 0, (struct sockaddr*)&clientaddr, slen);
        return AGENT_RET_FAIL;
	}
	else
	    sendto(g_sClientSocket, MSG_REC_SUCCESS, sizeof(MSG_REC_SUCCESS), 0, (struct sockaddr*)&clientaddr, slen);
	
	CloseHandle( hVol );
	NtSetSystemInformation( 80, &sysinfo, sizeof(sysinfo)) ;
	
    return AGENT_RET_SUCCESS;
}

void server_mainloop( SOCKET server_socket ) 
{
    SOCKET sClientSocket;
    
    int iResult,                        // system information with error message
        iSendResult,                    // system information with error message after send function
        command_code;
    MBA_AGENT_RETURN command_result;
    
    char sRecvbuf[SZ_MAX_CMD << 1];          // receive buffer from client
    char sCommand_line[SZ_MAX_CMD << 1];     // fully command line
                                             // we allocate two times more space for the string manipulation
    // Accept connections
    while( true ) {
        
        // Accept a client socket
		/*
        sClientSocket = accept(sListenSocket, NULL, NULL);
        if (sClientSocket == INVALID_SOCKET) {
            display_error("main - accept", FALSE);
            break;
        }
		*/
        g_sClientSocket = server_socket;

        sprintf_s(g_sLogMessage, SZ_MAX_LOG, "[SYSTEM] Start to get commands...\r\n");
        write_log();
        
		slen = sizeof(clientaddr);

        // Receive until the peer shuts down the connection
        do {
            
            ZeroMemory(sRecvbuf, sizeof(sRecvbuf));
			
            iResult = recvfrom(server_socket, sRecvbuf, SZ_MAX_CMD, 0, (struct sockaddr *)&clientaddr, &slen);
            
            if (iResult > 0) {
            
                sprintf_s(g_sLogMessage, SZ_MAX_LOG, "Bytes received: 0x%08x, Received message: %s\r\n", iResult, sRecvbuf);
                write_log();
                
                command_code = identify_command(sRecvbuf);
                
                if (command_code > 0) {
                    ZeroMemory(sCommand_line, sizeof(sCommand_line));
                    memcpy(sCommand_line, &sRecvbuf[5], SZ_MAX_CMD - 5);
                }

                switch (command_code) {
                    
                    case MBA_CMD_EXEC :
                        command_result = execute_cmd(sCommand_line);
                        break;

                    case MBA_CMD_INVO :
                        command_result = invoke_cmd(sCommand_line);
                        break;

                    case MBA_CMD_EXPO :
                        command_result = export_cmd(sCommand_line);
                        break;

                    case MBA_CMD_IMPO :
                        command_result = import_cmd(sCommand_line);
                        break;

                    case MBA_CMD_LOGF :
                        command_result = expolog_cmd();
                        break;
						
                    case MBA_CMD_SYNC :
                        command_result = sync_cmd();
                        break;

                    case MBA_CMD_UNKNOWN :
                    default:
                    display_error("main - [COMMAND ERROR] Unknown command", TRUE);
                    command_result = AGENT_RET_SUCCESS;    
                    break;
                }
        
                if ( command_result == AGENT_RET_SUCCESS ) {
                
                    // Echo the buffer back to the sender
                    iSendResult = sendto(server_socket, MSG_ACK_PREFIX, sizeof(MSG_ACK_PREFIX), 0, (struct sockaddr *)&clientaddr, slen);
                    if (iSendResult == SOCKET_ERROR) {
                        display_error("main - Send to server_socket", FALSE);
                        break;
                    }
                            
                    iSendResult = sendto(server_socket, sRecvbuf, SZ_MAX_CMD, 0, (struct sockaddr *)&clientaddr, slen);
                    if (iSendResult == SOCKET_ERROR) {
                        display_error("main - Send to server_socket", FALSE);
                        break;
                    }
                }
                else 
                    iResult = 1;
                        
                sprintf_s(g_sLogMessage, SZ_MAX_LOG, "Bytes sent: %d\r\n", iSendResult);
                write_log();
            }
            else if (GetLastError() == 0)
                break;
            else {
                display_error("Receive from server_socket", FALSE);
                break;
            }
        } while (iResult > 0);
		
        // shutdown the connection since we're done
        iResult = shutdown(server_socket, SD_SEND);
        if (iResult != -1 && iResult == SOCKET_ERROR) {
            closesocket(server_socket);
            display_error("Main - shutdown server_socket", FALSE);
        }
        closesocket(server_socket);
        sprintf_s(g_sLogMessage, SZ_MAX_LOG, "[SYSTEM] Connection closing...\r\n");
        write_log();  
    }
}

/// Set up socket for listen on 'DEFAULT_PORT' port.
///
/// No parameter
///
/// Return a socket number which is for listen socket(sListenSocket)
/// Return -1 for socket error, program should be terminate
static SOCKET socket_setup()
{
    SOCKET server_socket;
	
    int iResult;            // system information with error message
	
    char host_name[128];    // host name
    SOCKET sListenSocket;

    struct addrinfo hints;
    struct addrinfo *result = NULL;
    struct addrinfo *ptr = NULL;

    // Initial socket information
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_addrlen = 0;

    // Resolve the server address and port
    gethostname(host_name, sizeof(host_name));
    sprintf_s(g_sLogMessage, SZ_MAX_LOG, "Calling gethostname with %s\r\n", host_name);
    write_log();
    iResult = getaddrinfo(host_name, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        WSACleanup();
        display_error("main - getaddrinfo", FALSE);
        return -1;
    }

    // Create a SOCKET for connecting to server
    server_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (server_socket == INVALID_SOCKET) {
        freeaddrinfo(result);
        WSACleanup();
        display_error("main - socket", FALSE);
        return -1;
    }

    iResult = bind(server_socket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        display_error("main - bind", FALSE);
        freeaddrinfo(result);
        closesocket(server_socket);
        WSACleanup();
        return -1;
    }

    freeaddrinfo(result);
    /*
    iResult = listen(sListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        closesocket(sListenSocket);
        WSACleanup();
        display_error("main - listen", FALSE);
        return -1;
    }
    */
    return server_socket;

}

HANDLE create_log_file( const char* logPath ) 
{    
    HANDLE hLog;

    // prepare the log path
    if( logPath == NULL ) {
        // get the %My Documents%
        SHGetFolderPath( NULL, 
            CSIDL_PERSONAL | CSIDL_FLAG_CREATE, 
            NULL, 
            0, 
            g_sLogPath );
            
        strcat(g_sLogPath, "\\" DEFAULT_LOG_NAME );
    }
    else
        strncpy(g_sLogPath, logPath, MAX_PATH);
    
    // Createfile of log file and get handler
    hLog = CreateFile(
        g_sLogPath,                // fullpath name of the agent log file
        GENERIC_READ | GENERIC_WRITE,            // open for writing
        FILE_SHARE_READ,        // share
        NULL,                   // default security
        CREATE_ALWAYS,            // open file
        FILE_ATTRIBUTE_NORMAL,  // normal file
        NULL);                  // no attr. template
        
    return hLog;
} 

int __cdecl main( int argc, char* argv[] )
{
    
    ShowWindow(GetConsoleWindow(),SW_HIDE);
    
    int iResult;
	
    WSADATA wsaData;
    SOCKET sListenSocket = INVALID_SOCKET;  // listen socket
    
    LUID pv;
	TOKEN_PRIVILEGES tp;
	HANDLE hProc;
	HMODULE ntdll;
	
	// Raise up the privilege
	LookupPrivilegeValue( NULL, "SeProfileSingleProcessPrivilege", &pv );
	OpenProcessToken( GetCurrentProcess(), TOKEN_WRITE, &hProc );
		
	tp.PrivilegeCount			= 1;
	tp.Privileges[0].Luid		= pv;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(	hProc,
							FALSE,
							&tp,
							sizeof(TOKEN_PRIVILEGES),
							(PTOKEN_PRIVILEGES)NULL,
							(PDWORD)NULL );
	ntdll = LoadLibrary("ntdll.dll");
	NtSetSystemInformation = (NTSTATUS (WINAPI *)(INT, PVOID, ULONG))GetProcAddress(ntdll, "NtSetSystemInformation");	
	
	GetLocalTime(&localTime);
    
    g_hLog = create_log_file( NULL );
    if (g_hLog == INVALID_HANDLE_VALUE) {
        display_error("main - CreateFile - log file", FALSE);
        return 1;
    }

    // Initialize Winsock using Windows socket API version 2.2
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        display_error("main - WSAStartup", FALSE);
        return 2;
    }

    // Server socket setup
    sListenSocket = socket_setup();
    if (sListenSocket == -1) {
        display_error("main - socket_setup", FALSE);
        return 3;
    }

    // Giving welcom message
    sprintf_s(g_sLogMessage, SZ_MAX_LOG, "================ Welcome to MBA Agent ================\r\n");
    write_log();

    // Server infinite loop
    server_mainloop( sListenSocket );

    // Cleanup
    WSACleanup();
    closesocket(sListenSocket);
    CloseHandle(g_hLog);

    return 0;
}


