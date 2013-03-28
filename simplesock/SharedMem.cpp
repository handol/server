
/**
void testshm()
{
	//=== shm �������� ����
	SharedMem shmCreator;
	shmCreator.create("myshm.dat", 1024);  // create 1Kb shm
	printf("key=%d, shmid=%d\n", shmCreator.get_key(), shmCreator.get_id());
	if (shmCreator.open() < 0)
		printf("failed\n");
	else
		printf("shm addr=%X\n", shmCreator.get_addr());
	strcpy((char*)shmCreator.get_addr(), "This shm created");
	shmCreator.close();

	// create() ȣ���ϴ��� �̹� shm �� �����ϸ�, �׳� attach �ȴ�.
	SharedMem shmC2;
	shmC2.create("myshm.dat", 1024);  // create 1Kb shm
	printf("key=%d, shmid=%d\n", shmC2.get_key(), shmC2.get_id());
	if (shmC2.open() < 0)
		printf("failed\n");
	else
		printf("shm addr=%X\n", shmC2.get_addr());
	strcpy((char*)shmC2.get_addr(), "This shm2 created");
	shmC2.close();

	//=== shm ������� ����
	SharedMem shmReader;
	shmReader.open("myshm.dat", false);  // read only mode == false
	printf("key=%d, shmid=%d\n", shmReader.get_key(), shmReader.get_id());
	printf("shm addr=%X\n", shmReader.get_addr());
	printf("CONT: %s\n", shmReader.get_addr());
	if (shmReader.get_addr()) strcpy((char*)shmReader.get_addr(), "This shm read by shmReader");
	shmReader.close();

	SharedMem shmR2;
	shmR2.open("myshm.dat", false);  // read only mode == true
	printf("key=%d, shmid=%d\n", shmR2.get_key(), shmR2.get_id());
	printf("shm addr=%X\n", shmR2.get_addr());
	printf("CONT: %s\n", shmR2.get_addr());
	if (shmR2.get_addr())  strcpy((char*)shmR2.get_addr(), "This shm written by shmR2");

	int a;
	scanf("%d", &a);
}
**/

#include "SharedMem.h"
#include <stdio.h>

#ifndef _WIN32
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

SharedMem::SharedMem()
{
	shmkey = 0;
	shmsize = 0;
	shmid = PMS_INVALID_SHMID;
	shmaddr = NULL;
	is_creator = false;
}

SharedMem::~SharedMem()
{

}

/*============ ftok() ���� ===========*/
#ifdef _WIN32
//===DJB Hash Function
// An algorithm produced by Professor Daniel J. Bernstein and shown first to the world on the usenet newsgroup comp.lang.c. It is one of the most efficient hash functions ever published. 
// http://www.partow.net/programming/hashfunctions/#DJBHashFunction


uint4 SharedMem::DJBHash(const char * s)
{
	uint4 h = 5381;  // seed
	while (*s)		
		h = (h << 5) + h + (unsigned char)*s++;  
	return h;
}

#define BASEVAL_FTOK	1000
key_t SharedMem::ftok(const char *pathname, int proj_id)
{
	//  ��� key ���� ������ 1,000 ~ 8,000 �����̴�.
	uint4 hashval = DJBHash(pathname);
	uint4 myval = ((uint4)strlen(pathname) & 0x07) * BASEVAL_FTOK; // ���ڿ� ���� * 1000
	key_t mykey = BASEVAL_FTOK + hashval % BASEVAL_FTOK + myval + (uint4)proj_id;
	return mykey;
}
#endif


// convert a pathname to a key value
// return 0 if fail.
key_t SharedMem::path2key(const char shmpath_[])
{
#ifndef _WIN32
	// key file
	FILE* fp = fopen(shmpath_, "r");
	if (fp == NULL) {
		printf("Error: file for SHM not exist: %s\n", shmpath_);
		return 0;
	}
	fclose(fp);

	// inter-process communication identifier
	key_t keyval = ftok(shmpath_, 1);
	if (keyval == (key_t)-1) {
		perror("ftok");
		return 0;
	}
	return keyval;
#else
	key_t keyval = SharedMem::ftok(shmpath_, 1);
	if (keyval == (key_t)-1) {
		printf("Error: file for SHM not exist: %s\n", shmpath_);
		return 0;
	}
	return keyval;
#endif
}

// SHM �� ���� ����
int SharedMem::create(const char shmpath_[], const size_t shmsize_)
{	
	return this->create(SharedMem::path2key(shmpath_), shmsize_);	
}

// SHM �� ���� ����
int SharedMem::create(const key_t keyval, const size_t shmsize_)
{
	this->shmkey = keyval;
	this->shmsize = shmsize_;
	if (this->shmkey == 0)
		return -1;

	// create shared memory
#ifndef _WIN32
	shmid = shmget(shmkey, shmsize, 0666 | IPC_CREAT);
	if (shmid == PMS_INVALID_SHMID)
		return -1;
	is_creator = true;
	return shmid;
#else	
	sprintf_s(shmName, sizeof(shmName)-1, "shmfile_%d", shmkey);
	shmid = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, shmsize, shmName);
	if (shmid == 0) {
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			shmid = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, shmName);
			is_creator = true;
			return 0;
		}
		else {
			perror("CreateFileMapping");
			return -1;
		}
	}
	else {
		is_creator = true;
		return 0;
	}
#endif
}

// SHM �� ����
int SharedMem::remove()
{
#ifndef _WIN32
	if (shmctl(shmid, IPC_RMID, NULL) != 0) {		
		perror("shmctl");
		return -1;
	}
#else
	CloseHandle(shmid);
	if(UnmapViewOfFile(shmaddr) == FALSE) {
		perror("UnmapViewOfFile");
 		return -1;
 	}
#endif
	return 0;	
}

// open() -- ������ �����Ǿ� �ִ�  shm �� ã�� attach �ϴ� ��. �����ÿ� return -1.

// shmget and attach with shmpath
int SharedMem::open(const char shmpath_[], bool readonly)
{
	return this->open(SharedMem::path2key(shmpath_), readonly);
}

// shmget and attach with keyval
int SharedMem::open(const key_t keyval, bool readonly)
{
	this->shmkey = keyval;	
	if (this->shmkey == 0)
		return -1;

	// get shmid
#ifndef _WIN32
	shmid = shmget(shmkey, 0, 0);
	if (shmid == PMS_INVALID_SHMID)
		return -1;
	// get shm size
	struct shmid_ds ipcbuf;
	shmctl( shmid, IPC_STAT, &ipcbuf);
	shmsize = ipcbuf.shm_segsz;
#else
	// 
	sprintf_s(shmName, sizeof(shmName)-1, "shmfile_%d", shmkey);
	shmid = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, shmName);
	if (shmid == NULL) {
		perror("OpenFileMapping");
		return -1;
	}
#endif

	return this->open(readonly);
}

// attach
int SharedMem::open(bool readonly)
{
	/// attach
#ifndef _WIN32
	shmaddr = shmat(shmid, 0, 0);
	if (shmaddr == (void*)-1) {		
		perror("shmat");
		return -1;
	}
#else
	DWORD accessFlag = 0;
	if (readonly)
		accessFlag |= FILE_MAP_READ;
	else
		accessFlag = FILE_MAP_ALL_ACCESS;	
	
	shmaddr = MapViewOfFile(shmid, accessFlag, 0, 0, 0);
	if (shmaddr == NULL) {
		perror("MapViewOfFile");
		return -1;
	}
#endif
	return 0;
}


// SHM ���κ��� detach
int	SharedMem::close()
{
#ifndef _WIN32
	if (shmaddr) {
		if (shmdt(shmaddr) != 0) {
			perror("shmdt");
			return -1;
		}
		shmaddr = NULL;
	}
#else
	// creator �� �ƴ� ��쿡�� close() ���� UnmapViewOfFile() ����.
	// creator �� ��쿡�� remove() ���� UnmapViewOfFile() ����.
	// CloseHandle() �� �ص� shm �� ������� ������ Ȯ����. 

	if (is_creator) {
		shmaddr = NULL;
		return 0;
	}

	CloseHandle(shmid);	
	if(UnmapViewOfFile(shmaddr) == FALSE) {
		perror("UnmapViewOfFile");
 		return -1;
 	}
	shmaddr = NULL;
#endif
	return 0;
}

// system call�� ���� ����� �����ϴ� ���ڿ�
const char *SharedMem::get_error()
{
	return 0;
}
