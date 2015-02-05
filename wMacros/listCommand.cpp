#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>

#include <Windows.h>
#include <Shellapi.h>
#include <Shobjidl.h>
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