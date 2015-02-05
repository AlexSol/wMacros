//#ifndef CRESOURCE_H
//#define CRESOURCE_H

#include <windows.h>

typedef unsigned char BYTE;
typedef unsigned long DWORD;
typedef unsigned short WORD;

enum class TypeRes: WORD;
//namespace VarGlobar{int resLen;};

bool EnumResLangProc(HMODULE hModule, LPCTSTR lpszType, LPCTSTR lpszName, WORD wIDLanguage, LONG_PTR lParam);

class CResource
{
public:
	CResource();
	~CResource();
	bool load(const char * nameFile);
	bool freeLibrary();
	HRSRC findResource(int indexRes, WORD type);
	HRSRC findResource(int indexRes, TypeRes type);
	DWORD sizeofResource(HRSRC hRes);
	WORD getResourceLanguages(TypeRes type, int indexRes);
	int addRes(TypeRes type,  int indexRes, void* data, DWORD sizeRes);
	void* readRes(HRSRC hRes);
private:
	friend bool EnumResLangProc(HMODULE hModule, LPCTSTR lpszType, LPCTSTR lpszName, WORD wIDLanguage, LONG_PTR lParam);
	HMODULE m_hLibrary;
	char *m_fimename;
};

enum class TypeRes : WORD {
	ACCELERATOR = 9,
	ANICURSOR = 21,
	ANIICON = 22,
	BITMAP = 2,
	CURSOR = 1,
	DIALOG = 5,
	DLGINCLUDE = 17,
	FONT = 8,
	FONTDIR = 7,
	GROUP_CURSOR = 	(CURSOR) + 11,
	ICON = 3,
	GROUP_ICON = (ICON) + 11,
	HTML = 23,
	MENU = 4,
	MESSAGETABLE = 11,
	PLUGPLAY = 19,
	RCDATA = 10,
	STRING = 6,
	VERSION = 16,
	VXD = 20,
	MANIFEST = 24,
};

//#endif
