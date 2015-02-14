#pragma once
class File
{
public:
	File();
	bool open(const char* file, DWORD DesiredAccess, DWORD CreationDisposition);
	long long getSize();
	void write(void* data, unsigned int sizeData);
	bool close();

	~File();
private:
	HANDLE m_File;
};

