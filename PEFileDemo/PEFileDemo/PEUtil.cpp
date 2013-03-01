#include "StdAfx.h"


PEUtil::PEUtil(void)
{
	memset(&mapFile,0,sizeof(mapFile));
}

PEUtil::~PEUtil(void)
{
	if(valid){
		unloadFile();
	}
}

PIMAGE_DOS_HEADER PEUtil::getDosHeader()
{	
	LPVOID imageBase=mapFile.imageBase;
	PIMAGE_DOS_HEADER pDH=(PIMAGE_DOS_HEADER)imageBase;
	return pDH;
}

PIMAGE_FILE_HEADER PEUtil::getFileHeader()
{
	if(isPEFile()){
		PIMAGE_NT_HEADERS pNt=getNtHeaders();
		return &pNt->FileHeader;
	}else{
		return NULL;
	}
	
}



PIMAGE_NT_HEADERS PEUtil::getNtHeaders()
{
	if(isPEFile()){
		PIMAGE_DOS_HEADER pDH=getDosHeader();
		return (PIMAGE_NT_HEADERS)(pDH->e_lfanew+(DWORD)pDH);
	}else{
		return NULL;
	}
}
PIMAGE_OPTIONAL_HEADER PEUtil::getOptHeader()
{
	if(isPEFile()){
		PIMAGE_NT_HEADERS pNt=getNtHeaders();
		return &pNt->OptionalHeader;
	}else{
		return NULL;
	}
	
}

BOOL PEUtil::isValid()
{
	return valid;
}

BOOL PEUtil::loadFile( LPTSTR lpFilename )
{
	if(valid){
		unloadFile();
	}
	HANDLE hFile=CreateFile(lpFilename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(!hFile){
		return FALSE;
	}

	HANDLE hMapping=CreateFileMapping(hFile,NULL,PAGE_READONLY,0,0,NULL);
	if(!hMapping){
		return FALSE;
	}

	LPVOID imageBase=MapViewOfFile(hMapping,FILE_MAP_READ,0,0,0);
	if(!imageBase){
		return FALSE;
	}
	mapFile.hFile=hFile;
	mapFile.hMapping=hMapping;
	mapFile.imageBase=imageBase;

	valid=TRUE;
	return valid;
}

void PEUtil::unloadFile()
{
	if(mapFile.hFile){
		CloseHandle(mapFile.hFile);
	}
	if(mapFile.hMapping){
		CloseHandle(mapFile.hMapping);
	}
	if(mapFile.imageBase){
		UnmapViewOfFile(mapFile.imageBase);
	}
	valid=FALSE;
}

BOOL PEUtil::isPEFile()
{
	if(!valid){
		return FALSE;
	}
	LPVOID imageBase=mapFile.imageBase;
	if(IMAGE_DOS_SIGNATURE!=((PIMAGE_DOS_HEADER)imageBase)->e_magic){
		return FALSE;
	}
	if(IMAGE_NT_SIGNATURE!=((PIMAGE_NT_HEADERS)((DWORD)imageBase+((PIMAGE_DOS_HEADER)imageBase)->e_lfanew))->Signature){
		return FALSE;
	}
	
	return TRUE;
}

PIMAGE_SECTION_HEADER PEUtil::getFirstSectionHeader()
{
	if(isPEFile()){
		return IMAGE_FIRST_SECTION(getNtHeaders());
	}else{
		return NULL;
	}
}

PIMAGE_DATA_DIRECTORY PEUtil::getDataDirectory( USHORT dirEntry )
{
	if(isPEFile()){
		return &getOptHeader()->DataDirectory[dirEntry];
	}else{
		return NULL;
	}
	
}

PIMAGE_IMPORT_DESCRIPTOR PEUtil::getIID()
{
	if(isPEFile()){
		PIMAGE_DATA_DIRECTORY pDD=getDataDirectory(IMAGE_DIRECTORY_ENTRY_IMPORT);
		if(!pDD){
			return NULL;
		}else{
			return (PIMAGE_IMPORT_DESCRIPTOR)rva2ra(getDataDirectory(IMAGE_DIRECTORY_ENTRY_IMPORT)->VirtualAddress);
		}
		
	}else{
		return NULL;
	}
}

LPVOID PEUtil::rva2ra( DWORD rva )
{
	return ImageRvaToVa(getNtHeaders(),mapFile.imageBase,rva,NULL);
}
