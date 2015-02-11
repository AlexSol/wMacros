#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>

#include <Windows.h>
#include <Shellapi.h>
#include <Shobjidl.h>
#include <TlHelp32.h>
#include <wininet.h>

#include <memory>
#include "listCommand.h"

using namespace std;

unique_ptr<WCHAR[]> charToWchar(const char* str);

void DelSelf(const char* cmd1, const char* cmd2)
{
	char batName[] = "wMacrosDel.bat";
	const int BUFSIZE  = 4096;
	char pathTemp[BUFSIZ] = {0};
	(GetEnvironmentVariable("TEMP", pathTemp, BUFSIZE) == 0);

	std::string batPathFull = pathTemp;
	batPathFull += "\\";
	batPathFull += batName;

    char exeFileName[MAX_PATH];
    ::GetModuleFileName(NULL, exeFileName, MAX_PATH);

    std::ofstream file;
	file.open(batPathFull.c_str());

	file << "@echo off" << endl
		<< "ping test" << endl
		<< "del " << exeFileName << endl
		<< "del " << batName <<endl;
    file.close();

    Sleep(200);

	::ShellExecute(0, "open", batPathFull.c_str(), NULL, NULL, SW_HIDE);
}

void Run(const char* cmd1, const char* cmd2)
{
	//cout<< "Run" <<endl;
	STARTUPINFO si = {0};
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = {0};

	std::string scmd2 = cmd1;
	scmd2 += " ";
	scmd2 += cmd2;

	CreateProcess(NULL, const_cast<char*>(scmd2.c_str()), NULL, NULL, true, 0, NULL, NULL, &si, &pi);
}

void RunWait(const char* cmd1, const char* cmd2)
{
	STARTUPINFO si = {0};
	si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {0};

	std::string scmd2 = cmd1;
	scmd2 += " ";
	scmd2 +=  cmd2;

	int codError =  CreateProcess( NULL, const_cast<char*>(scmd2.c_str()), NULL, NULL, true, 0, NULL, NULL, &si, &pi);

    if(codError == 0) { 
		m_FormatMessage(GetLastError());
	}

   WaitForSingleObject(pi.hProcess, INFINITE);

   CloseHandle(pi.hProcess);
   CloseHandle(pi.hThread);
}

void ProcessClose(const char* cmd1, const char* cmd2)
{
    HANDLE hndl = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    DWORD dwsma = GetLastError();
    DWORD dwExitCode = 0;
    PROCESSENTRY32  procEntry={0};
    procEntry.dwSize = sizeof( PROCESSENTRY32 );
    Process32First(hndl,&procEntry);
    do
    {
        if(strcmpi(procEntry.szExeFile,cmd1) == 0 ){
			DWORD dwDesiredAccess = PROCESS_TERMINATE;
			BOOL  bInheritHandle  = FALSE;

			HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, procEntry.th32ProcessID);
			if (hProcess == NULL){ 
				m_FormatMessage(GetLastError());
			}
			if (TerminateProcess(hProcess, 0) != 0){
				m_FormatMessage(GetLastError());
			}
			CloseHandle(hProcess);
			CloseHandle(hndl);
        }
    }while(Process32Next(hndl,&procEntry));
	CloseHandle(hndl);

    return;
}

void Open(const char* cmd1, const char* cmd2)
{
	ShellExecute( NULL, TEXT("open"), cmd1, cmd2, NULL, SW_SHOWNORMAL);
}

void FileCopy(const char* cmd1, const char* cmd2)
{
	if( CopyFileW( charToWchar(cmd1).get(), charToWchar(cmd2).get(), FALSE) == 0	) { 
		m_FormatMessage(GetLastError());
	}
}

void FileDel(const char* cmd1, const char* cmd2)
{
	if( DeleteFileW(charToWchar(cmd1).get()) == 0 ){
		m_FormatMessage(GetLastError());
	}
}

void DirCopy(const char* cmd1, const char* cmd2)
{
	DirOperation(cmd1, cmd2, 'c');
}

void DirDel(const char* cmd1, const char* cmd2)
{
	DirOperation(cmd1, cmd2, 'd');
}

void DirCreate(const char* cmd1, const char* cmd2)
{
	if (::CreateDirectory(cmd1, NULL) == 0){
		//DWORD codError = GetLastError();
		m_FormatMessage(GetLastError());
	}

}

void DirMove(const char* cmd1, const char* cmd2)
{
	DirOperation(cmd1, cmd2, 'm');
}

void DirRename(const char* cmd1, const char* cmd2)
{
	DirOperation(cmd1, cmd2, 'r');
}

void DirOperation(const char* cmd1, const char* cmd2, const char opt)
{

	HMODULE hLibrary =  LoadLibrary("Shell32.dll");
	if (hLibrary != NULL){
		FreeLibrary(hLibrary);
	}

	unsigned int optFunc = 0;
	switch (opt)
	{
	case 'c':
		optFunc = FO_COPY;
		break;
	case 'd':
		optFunc = FO_DELETE;
		break;
	case 'm':
		optFunc = FO_MOVE;
		break;
	case 'r':
		optFunc = FO_RENAME;
		break;
	}
	SHFILEOPSTRUCTW sh;
	ZeroMemory(&sh, sizeof(sh));
	sh.wFunc = optFunc;
	sh.pFrom = charToWchar(cmd1).get();
	sh.pTo = charToWchar(cmd2).get();
	sh.fFlags =  FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;
	
	DWORD codError =  SHFileOperationW(&sh);

	if( codError != 0 ) {
	}
}


void DownloadFile(const char* cmd1, const char* cmd2)
{
	const char* userAgent = "Mozilla/5.0 (Windows NT) AppleWebKit/537.36 (KHTML, like Gecko)";
	HINTERNET hInternet = ::InternetOpen(userAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	
	if (hInternet != NULL){
		/*
		cout << "open" << endl;
		HINTERNET hConnect = ::InternetConnect(hInternet, "www.ex.ua", INTERNET_DEFAULT_HTTP_PORT,	NULL, NULL,	INTERNET_SERVICE_HTTP, 0, 1u);
		if (hConnect != NULL){
			cout << "InternetConnect" << endl;
			DWORD dwOpenRequestFlags = INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
				INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS |
				INTERNET_FLAG_KEEP_CONNECTION |
				INTERNET_FLAG_NO_AUTH |
				INTERNET_FLAG_NO_AUTO_REDIRECT |
				INTERNET_FLAG_NO_COOKIES |
				INTERNET_FLAG_NO_UI |
				INTERNET_FLAG_RELOAD;
			;
			HINTERNET hRequest = ::HttpOpenRequest(hConnect, "GET", "get/150410685", NULL, "www.ex.ua", NULL, dwOpenRequestFlags, 1);
			if (hRequest != NULL){
				cout << "Open REquest" << endl;
				bool bSend = HttpSendRequest(hRequest, NULL, 0, NULL, 0);
				if (bSend != false){
					DWORD dwCodeLen = 120*4;
					char a[120*3] = { 0 };
					DWORD ind = 0;
					HttpQueryInfo(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, a, &dwCodeLen, 0);
					char *t = strstr(a, "Location:");

					cout << t+strlen("Location:")+1 << endl;
					HINTERNET hRequest1 = ::HttpOpenRequest(hConnect, "GET", t, NULL, "www.ex.ua", NULL, dwOpenRequestFlags, 1);
					bool bSend = HttpSendRequest(hRequest1, NULL, 0, NULL, 0);

					char  szData[1024];
					DWORD dwBytesRead;
					BOOL bRead = ::InternetReadFile(hRequest1, szData, sizeof(szData)-1, &dwBytesRead);
					cout << dwBytesRead << "  " << szData << endl;
				}
			}
			InternetCloseHandle(hRequest);
		}
		InternetCloseHandle(hConnect);
		*/
		
		HINTERNET hOpenUrl = ::InternetOpenUrl(hInternet, cmd1, NULL, 0, INTERNET_FLAG_NO_AUTO_REDIRECT | INTERNET_FLAG_RELOAD, 0);
		if (hOpenUrl != NULL){
			
			const DWORD sizeBuffer = 1024;
			DWORD dwBytesRead = 0;
			char buffer[sizeBuffer] = {0};
			FILE *outFile;
			outFile = fopen(cmd2, "wb");
			size_t count;
			if (outFile == NULL) { cout << "Error create file  " << cmd2 << endl; }
			int  counter = 0;
			do
			{
				if(InternetReadFile(hOpenUrl, buffer, sizeBuffer, &dwBytesRead)){
					++counter;
					cout << counter << "  " << dwBytesRead << endl;
					count = fwrite(buffer, dwBytesRead, 1, outFile);
				}else {
					break;
				}
			} while (dwBytesRead == sizeBuffer);
			fclose(outFile);
			
		}
		::InternetCloseHandle(hOpenUrl);
	}

	::InternetCloseHandle(hInternet);

}

unique_ptr<WCHAR[]> charToWchar(const char* str)
{
	int wstrlend = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	unique_ptr<WCHAR[]> temp (new WCHAR[wstrlend+1]);
	
	MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str, -1, temp.get(), wstrlend);

	return temp;
}

void m_FormatMessage(int codError)
{
	TCHAR strMes[4096] = { 0 };
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, codError, 0, strMes, sizeof(strMes), NULL);

	cout << strMes << endl;
}