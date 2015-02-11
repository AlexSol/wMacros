#pragma once
using namespace std;

void DelSelf(const char* cmd1, const char* cmd2);

void Run(const char* cmd1, const char* cmd2);
void RunWait(const char* cmd1, const char* cmd2);
void ProcessClose(const char* cmd1, const char* cmd2);

void Open(const char* cmd1, const char* cmd2);

void FileCopy(const char* cmd1, const char* cmd2);
void FileDel(const char* cmd1, const char* cmd2);


void DirOperation(const char* cmd1, const char* cmd2, const char opt);
void DirCopy(const char* cmd1, const char* cmd2);
void DirDel(const char* cmd1, const char* cmd2);
void DirCreate(const char* cmd1, const char* cmd2);
void DirMove(const char* cmd1, const char* cmd2);
void DirRename(const char* cmd1, const char* cmd2);

void DownloadFile(const char* cmd1, const char* cmd2);


void m_FormatMessage(int codError);