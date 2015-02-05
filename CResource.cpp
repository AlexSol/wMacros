#include <windows.h>

#include "CResource.h"

namespace VarGlobar
{
	int resLen;
};

CResource::CResource()
{

}

CResource::~CResource()
{
	if( m_hLibrary != NULL ){ FreeLibrary(m_hLibrary); }
}

bool CResource::load(const char * nameFile)
{
    m_fimename = const_cast<char*>(nameFile);
	m_hLibrary = LoadLibrary(nameFile);
	if (m_hLibrary == NULL){ return true; }
	return false;
}
bool CResource::freeLibrary()
{
    if( m_hLibrary != NULL ){ FreeLibrary(m_hLibrary); }
	return 0;
}

HRSRC CResource::findResource(int indexRes, WORD type)
{
	HRSRC hResource = FindResource(m_hLibrary, MAKEINTRESOURCE(indexRes), MAKEINTRESOURCE(type));
	if (hResource == NULL){ return NULL; }
	return hResource;
}

HRSRC CResource::findResource(int indexRes, TypeRes type)
{
	HRSRC hResource = FindResource(m_hLibrary, MAKEINTRESOURCE(indexRes), MAKEINTRESOURCE(type));
	if (hResource == NULL){ return NULL; }
	return hResource;
}

DWORD CResource::sizeofResource(HRSRC hRes)
{
	return SizeofResource(m_hLibrary, hRes);
}

WORD CResource::getResourceLanguages(TypeRes type, int indexRes)
{
    using namespace VarGlobar;
	bool cod = EnumResourceLanguages(m_hLibrary, MAKEINTRESOURCE(type), MAKEINTRESOURCE(indexRes), (ENUMRESLANGPROC)EnumResLangProc, 0);
	return VarGlobar::resLen;
}

int CResource::addRes(TypeRes type,  int indexRes, void* data, DWORD sizeRes)
{
    HANDLE hHandle = BeginUpdateResource(m_fimename, false);
    if(hHandle == NULL) {return 1; }
    freeLibrary();

    BOOL result= UpdateResource(hHandle, MAKEINTRESOURCE(type), MAKEINTRESOURCE(indexRes),  MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL), data, sizeRes);
    if(result == false){ return 2; }

    if( !EndUpdateResource(hHandle, false)) { return GetLastError(); }
    return 4;

}
void* CResource::readRes(HRSRC hRes)
{
	HGLOBAL hResLoaded = LoadResource(m_hLibrary, hRes);
	if (hResLoaded != NULL){
		void *lpBuffer = LockResource(hResLoaded);
		if (lpBuffer != NULL){
			return lpBuffer;
		}
	}
	return NULL;
}


bool  EnumResLangProc(HMODULE hModule, LPCTSTR lpszType, LPCTSTR lpszName, WORD wIDLanguage, LONG_PTR lParam)
{
	using namespace VarGlobar;
	//resLan;
	VarGlobar::resLen = wIDLanguage;
	//resLan;// wIDLanguage;
	return true;
}
