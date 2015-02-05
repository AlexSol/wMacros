// wMacros.cpp: ���������� ����� ����� ��� ����������� ����������.
//
//#pragma comment(linker,"/FILEALIGN:512")
//#pragma comment(linker,"/NODEFAULTLIB")
#pragma comment(linker, "/MERGE:.CRT=.text")
#pragma comment(linker, "/merge:.rdata=.text")

#include "stdafx.h"
#include <Windows.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <string>
#include <map>

using namespace std;

#include "listCommand.h"

#include "version.h"
#include "CResource.h"

struct sCommand
{
	void (*fun)(const char* cmd1, const char* cmd2);
	std::string cmd_1;
	std::string cmd_2;
	char* os_ver;
	char os_type; // 1-0x32; 2-0x62; 3-all 
	bool win64;
} *COMMAND;

bool isNameMacros(char*);
bool isScript();
bool addResScript(const char* outFileExe, const char* inFileScript);

void Fn();

void RunCommand(sCommand *);

void strTolower(std::string& str);

int main(int argc, char* argv[])
{
	COMMAND = new sCommand;

	//if (isScript() == false){ MessageBox(0, "not found Script", "Error", MB_ICONERROR); return true; }

	if (strcmp(argv[2], "-b") == 0){
		if (addResScript(argv[3], argv[4]) == false) { printf("Error create exe-file\n"); }
	}
	delete COMMAND;
	ExitProcess(0);
	return 0;
}

bool isNameMacros(char* name)
{
	char t_name[20] = {0};
	char nameMacros[] = "wMacros.exe";

	char *temp = strrchr(name, '\\') + 1;
	
	if( strlen(temp) >=20 ){ return false; }
	strcpy( t_name,  temp );
	
	if(strcmp(nameMacros, t_name) == 0) return true;
	return false;
}

bool isScript()
{
	WORD indexResScript = 1;
	WORD typeResScript = 10; // RCDATA = 10,

	HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(indexResScript), MAKEINTRESOURCE(typeResScript));
	if (hResource == NULL){ return false; }
	return true;
}


void Fn()
{
	
	typedef void (*Command)(const char*, const char*);

	std::map<char*, Command> listCommand;
	
	listCommand.insert( pair<char*, Command>("DelSelf", DelSelf) );

	listCommand.insert( pair<char*, Command>("Run", Run) );
	listCommand.insert( pair<char*, Command>("RunWait", RunWait) );

	listCommand.insert( pair<char*, Command>("Open", Open) );

	listCommand.insert( pair<char*, Command>("FileCopy", FileCopy) );
	listCommand.insert( pair<char*, Command>("FileDell", FileDel) );

	//listCommand.insert( pair<char*, Command>("DirCopy", DirCopy) );
	listCommand.insert( pair<char*, Command>("DirDell", DirDel) );
	listCommand.insert( pair<char*, Command>("DirMove", DirMove) );
	listCommand.insert( pair<char*, Command>("DirRename", DirRename) );

	auto search = listCommand.find("Run");
	if(search != listCommand.end()){
	
	}
	
	
	char *file = "d:\\Test.wMacros";
	std::ifstream fileMacros;
	fileMacros.open(file,std::ios_base::in);

	//if(fileMacros.fail() == true) { cout<< "Error opend file" <<endl;} 

	std::string strLine;

	int indexLine = 0;
	bool foundCommad = false;
	char indexCmd = 0;

	while (getline(fileMacros, strLine))
	{
		++indexLine;
		if(strLine[0] == '#') { continue; }  //comment
		if(strLine[0] == '$') { continue; }  //comment

		if(indexCmd == 0){
			for (auto& i :listCommand)
			{
				std::string tempCom = "-"; 
				tempCom += i.first;
				tempCom += ":";

				if(tempCom == strLine) { 
					COMMAND->fun = i.second;
					foundCommad = true;
					cout<< strLine << endl;
					++indexCmd;
					continue;
				}
			}
			if(foundCommad == false) {cout<< indexLine << " : " << "Error command" <<endl; return; }
		}else{
			std::stringstream sstemp(strLine);
			//sstemp << strLine;
			std::string tCmd;
			sstemp >> tCmd;

			switch (indexCmd)
			{
				case 1:
				{
					if(tCmd != "cmd_1:") {cout<< indexLine << " : " << "Error: "<< tCmd <<endl; return;}
					sstemp >> tCmd;
					COMMAND->cmd_1 =  tCmd.c_str();
					indexCmd++;
				}
				break;
				case 2:
				{
					if(tCmd != "cmd_2:") {cout<< indexLine << " : " << "Error: "<< tCmd <<endl; return;}
					sstemp >> tCmd;
					COMMAND->cmd_2 =  tCmd.c_str();
					indexCmd++;
				}
				break;
				case 3:
				{
					if(tCmd != "os_ver:") {cout<< indexLine << " : " << "Error: "<< tCmd <<endl; return;}
					sstemp >> tCmd;
					COMMAND->os_ver = const_cast<char*>( tCmd.c_str());
					indexCmd++;
				}
				break;
				case 4:
				{
					if(tCmd != "os_type:") {cout<< indexLine << " : " << "Error: "<< tCmd <<endl; return;}
					sstemp >> tCmd;
					strTolower(tCmd);

					if(tCmd == "0x32"){ COMMAND->os_type = 1;}
					else{
						if(tCmd == "0x64") {COMMAND->os_type = 2;}
						else{
							if(tCmd == "all")  {COMMAND->os_type = 3;}
							else{
								cout<< indexLine << " : " << "Error: "<< tCmd <<endl; return;
							}
						}
					}
					indexCmd++;
				}
				break;
				case 5:
				{
					if(tCmd != "win64:") {cout<< indexLine << " : " << "Error: "<< tCmd <<endl; return;}
					sstemp >> tCmd;
					strTolower(tCmd);
					if( (tCmd == "disable") || (tCmd == "enable")){
						COMMAND->win64 =  (tCmd == "enable") ? true: false;
					}else{
						cout<< indexLine << " : " << "Error: "<< tCmd <<endl; return;
					}
					indexCmd = 6;
				}
				break;
			}
		}
		if(indexCmd == 6){
		RunCommand(COMMAND);
		indexCmd = 0;
		}
	}

	
	fileMacros.close();
}
	

void RunCommand(sCommand * cmd)
{
	cmd->fun(cmd->cmd_1.c_str(), cmd->cmd_2.c_str());
}


void strTolower(std::string& str)
{
	//std::locale loc;
	for (std::string::size_type i=0; i<str.length(); ++i)
	{
		str[i] = tolower(str[i]);
	}
}

bool addResScript(const char* outFileExe, const char* inFileScript)
{
	const int sizeBuffer = MAX_PATH;
	char exeFile[sizeBuffer] = {0};

	GetModuleFileName(NULL, exeFile, sizeBuffer);

	bool errorCod = ::CopyFile(exeFile, outFileExe, false);
	if(errorCod == 0 ){ return false; }

	std::unique_ptr<CResource> res(new CResource);

	if (res->load(outFileExe) == true) { cout << "error load file" << endl; return false; }
	

	HANDLE hFile = CreateFile(inFileScript, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) { return false; }
	
	const int maxSizeScript = 5242880;  // 5 MB
	LARGE_INTEGER sizeFileScript = {0};

	bool resSize = GetFileSizeEx(hFile, &sizeFileScript);

	if ((resSize == INVALID_FILE_SIZE) || (sizeFileScript.QuadPart > maxSizeScript)) { CloseHandle(hFile); return false; }
	
	char* resData = new(char[sizeFileScript.QuadPart]);
	memset(resData, 0, sizeFileScript.QuadPart);
	
	FILE *file;
	file = fopen(inFileScript, "rb");

	if (file == 0){ return false; }
	
	char t = 0;
	for ( int i = 0; (t = fgetc(file)) != EOF; i++)
	{
		resData[i] = t;
	}

	fclose(file);

	if (res->addRes(TypeRes::RCDATA, 1, &resData[0], sizeFileScript.QuadPart) != 4) { return false;  }

	delete[] resData;

	res->freeLibrary();
}