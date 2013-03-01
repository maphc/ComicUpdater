#pragma once
#include "stdafx.h"



typedef struct _MAP_FILE_STRUCT
{
	HANDLE hFile;
	HANDLE hMapping;
	LPVOID imageBase;
}  MAP_FILE_STRUCT,* PMAP_FILE_STRUCT;

class PEUtil
{
private :
	MAP_FILE_STRUCT mapFile;
	BOOL valid;
public:
	PEUtil(void);
	~PEUtil(void);
	BOOL loadFile(LPTSTR lpFilename);
	void unloadFile();
	BOOL isPEFile();
	PIMAGE_DOS_HEADER getDosHeader();
	PIMAGE_FILE_HEADER getFileHeader();
	PIMAGE_NT_HEADERS getNtHeaders();
	PIMAGE_OPTIONAL_HEADER getOptHeader();
	PIMAGE_SECTION_HEADER getFirstSectionHeader();
	PIMAGE_DATA_DIRECTORY getDataDirectory(USHORT dirEntry);
	PIMAGE_IMPORT_DESCRIPTOR getIID();
	LPVOID rva2ra(DWORD rva);
	BOOL isValid();
};


