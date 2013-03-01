// zlibtest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
/* HTTP gzip decompress */

//最终接口：从某个目录创建zip文件  
void CreateZipFromDir(const CString& dirName, const CString& zipFileName);  


//将文件添加到zip文件中，注意如果源文件srcFile为空则添加空目录  
//fileNameInZip: 在zip文件中的文件名，包含相对路径  
void AddFileToZip(zipFile zf, const char* fileNameInZip, const char* srcFile)  
{  
	FILE* srcfp = NULL;  

	//初始化写入zip的文件信息  
	zip_fileinfo zi;  
	zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =  
		zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;  
	zi.dosDate = 0;  
	zi.internal_fa = 0;  
	zi.external_fa = 0;  

	//如果srcFile为空，加入空目录  
	char new_file_name[MAX_PATH];  
	memset(new_file_name, 0, sizeof(new_file_name));  
	strcat(new_file_name, fileNameInZip);  
	if (srcFile == NULL)  
	{  
		strcat(new_file_name, "/");  
	}  

	//在zip文件中创建新文件  
	zipOpenNewFileInZip(zf, new_file_name, &zi, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);  

	if (srcFile != NULL)  
	{  
		//打开源文件  
		srcfp = fopen(srcFile, "rb");  
		if (srcfp == NULL)  
		{  
			//MessageBox(_T("无法添加文件") + CString(srcFile) + _T("!")); 
			MessageBox(NULL,_T("无法添加文件") + CString(srcFile) + _T("!"),_T("错误"),MB_OK);  
			zipCloseFileInZip(zf); //关闭zip文件  
			return;  
		}  

		//读入源文件并写入zip文件  
		char buf[100*1024]; //buffer  
		int numBytes = 0;  
		while( !feof(srcfp) )  
		{  
			numBytes = fread(buf, 1, sizeof(buf), srcfp);  
			zipWriteInFileInZip(zf, buf, numBytes);  
			if( ferror(srcfp) )  
				break;  
		}  

		//关闭源文件  
		fclose(srcfp);  
	}  

	//关闭zip文件  
	zipCloseFileInZip(zf);  
}  


//递归添加子目录到zip文件  
void CollectFilesInDirToZip(zipFile zf, const CString& strPath, const CString& parentDir)  
{  
	USES_CONVERSION; //for W2CA  

	CString strRelativePath;  
	CFileFind finder;   
	BOOL bWorking = finder.FindFile(strPath + _T("\\*.*"));  
	while(bWorking)   
	{   
		bWorking = finder.FindNextFile();   
		if(finder.IsDots())  
			continue;   

		if (parentDir == _T(""))  
			strRelativePath = finder.GetFileName();  
		else  
			strRelativePath = parentDir + _T("\\") + finder.GetFileName(); //生成在zip文件中的相对路径  

		if(finder.IsDirectory())  
		{  
			AddFileToZip(zf, (strRelativePath), NULL); //在zip文件中生成目录结构  
			CollectFilesInDirToZip(zf, finder.GetFilePath(), strRelativePath); //递归收集子目录文件  
			continue;  
		}  

		AddFileToZip(zf, (strRelativePath), (finder.GetFilePath())); //将文件添加到zip文件中  
	}  
}  


//最终接口：从某个目录创建zip文件  
void CreateZipFromDir(const CString& dirName, const CString& zipFileName)  
{  
	USES_CONVERSION; //使用W2CA转换unicode字符集  
	zipFile newZipFile = zipOpen((zipFileName), APPEND_STATUS_CREATE); //创建zip文件  
	if (newZipFile == NULL)  
	{  
		MessageBox(NULL,_T("无法创建zip文件!"),_T("错误"),MB_OK);  
		return;  
	}  

	CollectFilesInDirToZip(newZipFile, dirName, _T(""));  
	zipClose(newZipFile, NULL); //关闭zip文件  
}  

int _tmain(int argc, _TCHAR* argv[])
{
	const uLong BUFFER_SIZE=1024*4;
	
	CString res;
	HANDLE hFile=CreateFile("error.log1",GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	//ReadFile(hFile,buf,1024*10,&read,NULL);
	//httpgzdecompress((Bytef*)buf,read,(Bytef*)des,&desLen);

	

	char buffer[BUFFER_SIZE*2]={0};
	char desBuffer[BUFFER_SIZE*10]={0};
	int err = 0;
	z_stream d_stream = {0}; /* decompression stream */
	uLong read=0;

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;
	d_stream.next_in  = (Bytef*)buffer;
	d_stream.avail_in = 0;
	d_stream.next_out = (Bytef*)desBuffer;
	d_stream.avail_out=BUFFER_SIZE;

	if(inflateInit2(&d_stream, 47) != Z_OK) 
		return -1;
	;
	char* p=buffer;
	while(ReadFile(hFile,p,BUFFER_SIZE,&read,NULL)&&read>0){
		ZeroMemory(desBuffer,BUFFER_SIZE*10);
		d_stream.next_in  = (Bytef*)buffer;
		d_stream.avail_in = read;

		d_stream.next_out = (Bytef*)desBuffer;
		d_stream.avail_out=BUFFER_SIZE*10;

		err=inflate(&d_stream,Z_NO_FLUSH);
		if(err==Z_OK){
			res+=desBuffer;
		}else if(err==Z_STREAM_END){
			res+=desBuffer;
			break;
		}

		memcpy(buffer,buffer+d_stream.avail_in,BUFFER_SIZE-d_stream.avail_in);
		ZeroMemory(buffer+BUFFER_SIZE-d_stream.avail_in,BUFFER_SIZE);
		p+=d_stream.avail_in;

	}
	inflateEnd(&d_stream);
	


	CloseHandle(hFile);



	CreateZipFromDir("f:\\Temp\\UIPCall","f:\\abc.zip");







	return 0;
}

