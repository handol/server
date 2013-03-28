#ifndef _PMS_SHARED_MEM_H
#define _PMS_SHARED_MEM_H

/*
UNIX 에서는 file path로부터 shm key를 구하는 기능이 있지만,
WIN 에서는 없다.
ftok() 가 없음.

int 타입의 method들은 오류시에 return -1.
*/

#include "PmsOS.h"
#ifdef _WIN32
//typedef unsigned int key_t;
#endif

typedef unsigned int uint4;

/// 공유 메모리 식별자
#ifdef _WIN32
typedef HANDLE PMS_SHMID_TYPE;
#define PMS_INVALID_SHMID INVALID_HANDLE_VALUE
#else
typedef int PMS_SHMID_TYPE;
#define PMS_INVALID_SHMID (-1)
#endif


class PmsSharedMem
{
public:
	PmsSharedMem(); 	
	~PmsSharedMem();
	
	key_t get_key()
	{
		return shmkey;
	}

	size_t get_size()
	{
		return shmsize;
	}
	
	PMS_SHMID_TYPE get_id()
	{
		return shmid;
	}
	
	// SHM 에 attach 된 주소값
	void *get_addr()
	{
		return shmaddr;
	}

	// SHM 을 새로 생성
	int create(const char shmpath[], const size_t shmsize_);
	int create(const key_t keyval, const size_t shmsize_);

	// SHM 을 제거
	int remove();
	
	// SHM 에 shmget and attach. 오류시에 return -1.
	int open(const char shmpath[], bool readonly=false);
	int open(const key_t keyval, bool readonly=false);
	int open(bool readonly=false);
	
	
	// SHM 으로부터 detach
	int	close();
	// system call의 오류 결과를 설명하는 문자열
	const char *get_error();

	static key_t path2key(const char shmpath_[]);

	#ifdef _WIN32
	static uint4 DJBHash(const char * s);
	static key_t ftok(const char *pathname, int proj_id);
	#endif
	
private:
	key_t	shmkey; // SHM KEY값
	size_t	shmsize;
	PMS_SHMID_TYPE shmid; // SHM ID
	void	*shmaddr; // SHM attach된 주소값
	bool	is_creator;
#ifdef _WIN32
	char shmName[256];
#endif

};

#endif // _PMS_SHARED_MEM_H

