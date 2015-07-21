// memoryMapedIO_HW2.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//


#include "stdafx.h"
#include <stdint.h>
#include <conio.h>
#include "StopWatch.h"
#include "mmio.h"
#include "FileIoHelperClass.h"
#include "DebugMessage.h"
#include "StatusCode.h"


int _tmain(int argc, _TCHAR* argv[])
{
	StopWatch sw = StopWatch();
	//=========================<함수 속도 비교>================================/
//	/*
	
	_ASSERTE(create_very_big_file(L"D:\\ori.txt", 400));

	sw.Start();
	_ASSERTE(file_copy_using_read_write(L"D:\\ori.txt", L"D:\\hw2_readwrite.txt"));
	sw.Stop();
	printf("readwrite : %f\n", sw.GetDurationSecond());

	sw.Start();
	file_copy_using_memory_map(L"D:\\ori.txt", L"D:\\hw2_memory.txt");
	sw.Stop();
	printf("memory : %f\n", sw.GetDurationSecond());


//	*/



	//=========================<5GB 이상일 때>================================
//	/*
	FileIoHelper *Fhelper_create = new FileIoHelper();
	LARGE_INTEGER Size;
	Size.QuadPart = (uint64_t)1024 * 1024 * 1024 * 8;

	Fhelper_create->FIOCreateFile(L"D:\\big.txt", Size);
	Fhelper_create->FIOClose();


	FileIoHelper *Fhelper_read = new FileIoHelper();
	FileIoHelper *Fhelper_write = new FileIoHelper();

	LARGE_INTEGER offset, file_size;
	offset.QuadPart = 0;
	file_size.QuadPart = (unsigned long long) 1024 * 1024 * 1024 * 8;

	sw.Start();

	Fhelper_read->FIOpenForRead(L"D:\\big.txt");
	Fhelper_write->FIOCreateFile(L"D:\\test_memory.txt", file_size);

	uint64_t bufSize = 1024 * 1024 * 1024;
	PUCHAR buf = (PUCHAR)malloc(bufSize);

	while (offset.QuadPart < Size.QuadPart) {
		if ((Size.QuadPart - offset.QuadPart) >(LONGLONG)bufSize) {
			bufSize = (unsigned long long)(1024 * 1024 * 128);
		}
		else {
			bufSize = (unsigned long long)(Size.QuadPart - offset.QuadPart);
		}
		Fhelper_read->FIOReadFromFile(offset, bufSize, buf);
		Fhelper_write->FIOWriteToFile(offset, bufSize, buf);

		offset.QuadPart += (LONGLONG)bufSize;
	}

	sw.Stop();
	printf("memory : %f\n", sw.GetDurationSecond());

	free(buf);
	Fhelper_read->FIOClose();
	Fhelper_write->FIOClose();
//	*/

	return 0;
}

