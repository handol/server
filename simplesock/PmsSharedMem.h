#ifndef _PMS_SHARED_MEM_H
#define _PMS_SHARED_MEM_H

/*
UNIX ������ file path�κ��� shm key�� ���ϴ� ����� ������,
WIN ������ ����.
ftok() �� ����.

int Ÿ���� method���� �����ÿ� return -1.
*/

#include "PmsOS.h"
#ifdef _WIN32
//typedef unsigned int key_t;
#endif

typedef unsigned int uint4;

/// ���� �޸� �ĺ���
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
	
	// SHM �� attach �� �ּҰ�
	void *get_addr()
	{
		return shmaddr;
	}

	// SHM �� ���� ����
	int create(const char shmpath[], const size_t shmsize_);
	int create(const key_t keyval, const size_t shmsize_);

	// SHM �� ����
	int remove();
	
	// SHM �� shmget and attach. �����ÿ� return -1.
	int open(const char shmpath[], bool readonly=false);
	int open(const key_t keyval, bool readonly=false);
	int open(bool readonly=false);
	
	
	// SHM ���κ��� detach
	int	close();
	// system call�� ���� ����� �����ϴ� ���ڿ�
	const char *get_error();

	static key_t path2key(const char shmpath_[]);

	#ifdef _WIN32
	static uint4 DJBHash(const char * s);
	static key_t ftok(const char *pathname, int proj_id);
	#endif
	
private:
	key_t	shmkey; // SHM KEY��
	size_t	shmsize;
	PMS_SHMID_TYPE shmid; // SHM ID
	void	*shmaddr; // SHM attach�� �ּҰ�
	bool	is_creator;
#ifdef _WIN32
	char shmName[256];
#endif

};

#endif // _PMS_SHARED_MEM_H

