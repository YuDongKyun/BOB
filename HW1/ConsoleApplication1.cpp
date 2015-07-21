// ConsoleApplication1.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include <Windows.h>
#include <memory.h>
#include <strsafe.h>
#include <stdint.h>
#include <crtdbg.h>
#include <atlstr.h>

/**
* @brief
* @param
* @see
* @remarks
* @code
* @endcode
* @return
**/


wchar_t* Utf8MbsToWcs(_In_ const char* utf8)
{
	_ASSERTE(NULL != utf8);
	if (NULL == utf8) return NULL;

	int outLen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8, -1, NULL, 0);
	if (0 == outLen) return NULL;

	wchar_t* outWchar = (wchar_t*)malloc(outLen * (sizeof(wchar_t)));  // outLen contains NULL char 
	if (NULL == outWchar) return NULL;
	RtlZeroMemory(outWchar, outLen);

	if (0 == MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8, -1, outWchar, outLen))
	{
		free(outWchar);
		return NULL;
	}

	return outWchar;
}
char* WcsToMbsUTF8(_In_ const wchar_t* wcs)
{
	_ASSERTE(NULL != wcs);
	if (NULL == wcs) return NULL;

	int outLen = WideCharToMultiByte(CP_UTF8, 0, wcs, -1, NULL, 0, NULL, NULL);
	if (0 == outLen) return NULL;

	char* outChar = (char*)malloc(outLen * sizeof(char));
	if (NULL == outChar) return NULL;
	RtlZeroMemory(outChar, outLen);

	if (0 == WideCharToMultiByte(CP_UTF8, 0, wcs, -1, outChar, outLen, NULL, NULL))
	{
		free(outChar);
		return NULL;
	}

	return outChar;
}

char* ConvertUnicodeToMultibyte(CString strUnicode)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, NULL, 0, NULL, NULL);

	char* pMultibyte = new char[nLen];
	memset(pMultibyte, 0x00, (nLen)*sizeof(char));

	WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, pMultibyte, nLen, NULL, NULL);

	return pMultibyte;
}

bool is_file_existsW(_In_ const wchar_t* file_path)
{
	_ASSERTE(NULL != file_path);
	_ASSERTE(TRUE != IsBadStringPtrW(file_path, MAX_PATH));
	if ((NULL == file_path) || (TRUE == IsBadStringPtrW(file_path, MAX_PATH))) return false;

	WIN32_FILE_ATTRIBUTE_DATA info = { 0 };

	if (GetFileAttributesExW(file_path, GetFileExInfoStandard, &info) == 0)
		return false;
	else
		return true;
}

void print(_In_ const char* fmt, _In_ ...)
{
	char log_buffer[2048];
	va_list args;

	va_start(args, fmt);
	HRESULT hRes = StringCbVPrintfA(log_buffer, sizeof(log_buffer), fmt, args);
	if (S_OK != hRes)
	{
		fprintf(
			stderr,
			"%s, StringCbVPrintfA() failed. res = 0x%08x",
			__FUNCTION__,
			hRes
			);
		return;
	}

	OutputDebugStringA(log_buffer);
	fprintf(stdout, "%s \n", log_buffer);
}


/**
* @brief
* @param
* @see
* @remarks
* @code
* @endcode
* @return
**/
bool create_bob_txt()
{
	// current directory 를 구한다.
	wchar_t *buf = NULL;
	uint32_t buflen = 0;
	buflen = GetCurrentDirectoryW(buflen, buf);
	if (0 == buflen)
	{
		print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
		return false;
	}

	buf = (PWSTR)malloc(sizeof(WCHAR) * buflen);
	if (0 == GetCurrentDirectoryW(buflen, buf))
	{
		print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
		free(buf);
		return false;
	}

	// current dir \\ bob.txt 파일명 생성
	wchar_t file_name[260];
	if (!SUCCEEDED(StringCbPrintfW(
		file_name,
		sizeof(file_name),
		L"%ws\\bob.txt",
		buf)))
	{
		print("err, can not create file name");
		free(buf);
		return false;
	}
	free(buf); buf = NULL;

	if (true == is_file_existsW(file_name))
	{
		::DeleteFileW(file_name);
	}

	// 파일 생성
	HANDLE file_handle = CreateFileW(
		file_name,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (file_handle == INVALID_HANDLE_VALUE)
	{
		print("err, CreateFile(path=%ws), gle=0x%08x", file_name, GetLastError());
		return false;
	}

	printf("%s\n", file_name);
	DWORD bytes_written = 0;
	wchar_t string_buf[1024];
	char *utf_str;

	WriteFile(file_handle, "\xef\xbb\xbf", 3, &bytes_written, NULL);

	// 파일에 데이터 쓰기
	if (!SUCCEEDED(StringCbPrintfW(
		string_buf,
		sizeof(string_buf),
		L"가나다라마바사아자차카타파하")))
	{
		print("err, can not create data to write.");
		CloseHandle(file_handle);
		return false;
	}
	utf_str = WcsToMbsUTF8(string_buf);

	if (!WriteFile(file_handle, utf_str, strlen(utf_str), &bytes_written, NULL))
	{
		print("err, WriteFile() failed. gle = 0x%08x", GetLastError());
		CloseHandle(file_handle);
		return false;
	}
	free(utf_str);

	// 영어로 쓰기
	if (!SUCCEEDED(StringCbPrintfW(
		string_buf,
		sizeof(string_buf),
		L"All work and no play makes jack a dull b oy.")))
	{
		print("err, can not create data to write.");
		CloseHandle(file_handle);
		return false;
	}

	utf_str = WcsToMbsUTF8(string_buf);

	if (!WriteFile(file_handle, utf_str, strlen(utf_str), &bytes_written, NULL))
	{
		print("err, WriteFile() failed. gle = 0x%08x", GetLastError());
		CloseHandle(file_handle);
		return false;
	}
	free(utf_str);

	wchar_t string_bufa[1024];
	if (!SUCCEEDED(StringCbPrintfW(
		string_bufa,
		sizeof(string_bufa),
		L"동해물과 백두산이 마르고 닳도록 하느님이 보우하사 우리나라만세")))
	{
		print("err, can not create data to write.");
		CloseHandle(file_handle);
		return false;
	}

	utf_str = WcsToMbsUTF8(string_bufa);

	if (!WriteFile(file_handle, utf_str, strlen(utf_str), &bytes_written, NULL))
	{
		print("err, WriteFile() failed. gle = 0x%08x", GetLastError());
		CloseHandle(file_handle);
		return false;
	}
	free(utf_str);
	// 영어로 쓰기
	if (!SUCCEEDED(StringCbPrintfW(
		string_bufa,
		sizeof(string_bufa),
		L"All work and no play makes jack a dull boy.")))
	{
		print("err, can not create data to write.");
		CloseHandle(file_handle);
		return false;
	}

	utf_str = WcsToMbsUTF8(string_bufa);

	if (!WriteFile(file_handle, utf_str, strlen(utf_str), &bytes_written, NULL))
	{
		print("err, WriteFile() failed. gle = 0x%08x", GetLastError());
		CloseHandle(file_handle);
		return false;
	}
	// 파일 닫기
	CloseHandle(file_handle);
	free(utf_str);

	CopyFile(file_name, L"bob2.txt", true);

	file_handle = CreateFile(L"bob2.txt", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwRead;
	if (file_handle == INVALID_HANDLE_VALUE)
	{
		print("err, CreateFile(path=%ws), gle=0x%08x", file_name, GetLastError());
		return false;
	}
	char string_bufc[1024];
	memset((void *)string_bufc, 0x00, 1024);
	ReadFile(file_handle, string_bufc, 1024, &dwRead, NULL);
	char* pMultibyte = ConvertUnicodeToMultibyte(Utf8MbsToWcs(string_bufc));

	printf("%s\n\n", pMultibyte);
	CloseHandle(file_handle);
	free(pMultibyte);

	file_handle = CreateFile(L"bob2.txt", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE file_map = CreateFileMapping(
		file_handle, 
		NULL,
		PAGE_READONLY,    // 파일속성과 맞춤
		0,      // dwMaximumSizeHigh
		0,    // dwMaximumSizeLow
		NULL);


	PCHAR file_view = (PCHAR)MapViewOfFile(
		file_map,
		FILE_MAP_READ,
		0,
		0,
		0);

	char a = file_view[3];  // 0xd9
	char b = file_view[4];  // 0xb3

	printf("%02x / %02x \n", a, b);

	UnmapViewOfFile(file_view);
	CloseHandle(file_map);
	CloseHandle(file_handle);

	return true;
}


int main(){
	create_bob_txt();
	return 0;
}