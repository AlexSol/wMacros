#include "stdafx.h"
#include <Windows.h>
#include <stdexcept>
#include "File.h"


File::File()
{
}


File::~File()
{
	CloseHandle(m_File);
}

bool File::open(const char* file, DWORD DesiredAccess, DWORD CreationDisposition)
{	
	m_File = ::CreateFile(file, DesiredAccess, FILE_SHARE_READ, NULL, CreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_File == INVALID_HANDLE_VALUE){ throw std::runtime_error("file open failure"); }
	return true;
}

long long File::getSize()
{
	LARGE_INTEGER size = { 0 };

	if (::GetFileSizeEx(m_File, &size) == 0) { throw std::runtime_error("file get size failure"); }
	return size.QuadPart;
}

void File::write(void* data, unsigned int sizeData)
{
	DWORD sizeBufferToWrite = 1024;
	DWORD BytesWritten(0);

	if (sizeBufferToWrite > sizeData) { sizeBufferToWrite = sizeData; }

	unsigned int tSize = sizeBufferToWrite;
	do
	{
		if (::WriteFile(m_File, data, sizeBufferToWrite, &BytesWritten, NULL) == 0){ throw std::runtime_error("file error write"); }
		if (tSize < sizeData){
			tSize += sizeBufferToWrite;
		}else{
			sizeBufferToWrite = tSize += sizeData - tSize;
		}
	} while (tSize != sizeData);
}

bool File::close()
{
	CloseHandle(m_File);
}
