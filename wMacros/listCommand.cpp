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
	char batFile[] = "wMacrosDel.bat";
	const int BUFSIZE  = 4096;
	char exeBat[BUFSIZ] = {0};
	GetEnvironmentVariable("%TEMP%", exeBat, BUFSIZE);
	std::string exeFile =  exeBat;
	exeFile += "\\";
	exeFile += batFile;

    char exeFileName[MAX_PATH];
    ::GetModuleFileName(NULL, exeFileName, MAX_PATH);

    std::ofstream file;
	file.open(exeFile.c_str());
	cout<< file.failbit << endl;

    file<< "@echo off" << endl
    << "del " << exeFileName << endl;
   // << "del " << batFile <<endl;
    file.close();

    Sleep(200);

    ::ShellExecute(0, "open",batFile, NULL, NULL, SW_HIDE);
}

void Run(const char* cmd1, const char* cmd2)
{
	//cout<< "Run" <<endl;
	STARTUPINFO si = {0};
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = {0};

	char *Tcmd2 = const_cast<char*>(cmd2);
	CreateProcess(cmd1, Tcmd2, NULL, NULL, true, 0, NULL, NULL, &si, &pi);
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
		codError = GetLastError();
		TCHAR strMes[4096] = {0};
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, codError, 0, strMes , sizeof(strMes), NULL);
		cout<< strMes <<endl;
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
		DWORD codError = GetLastError(); 
		cout<< codError << endl;
	}
}

void FileDel(const char* cmd1, const char* cmd2)
{
	if( DeleteFile(cmd1) == 0 ){
		DWORD codError = GetLastError(); 
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
	if (hLibrary != NULL)
	{
		
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
	SHFILEOPSTRUCT sh;
	ZeroMemory(&sh, sizeof(sh));
	sh.wFunc = optFunc;
	sh.pFrom = cmd1;
	sh.pTo = cmd2;
	sh.fFlags =  FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;
	
	DWORD codError =  SHFileOperation(&sh);

	if( codError != 0 ) {
		DWORD codError = GetLastError(); 
		cout<< "Error  "<< codError <<endl;
	}
}

unique_ptr<WCHAR[]> charToWchar(const char* str)
{
	int wstrlend = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	unique_ptr<WCHAR[]> temp (new WCHAR[wstrlend+1]);
	
	MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str, -1, temp.get(), wstrlend);

	return temp;
}