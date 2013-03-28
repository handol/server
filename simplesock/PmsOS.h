
#ifndef PMS_OS_H
#define PMS_OS_H

///  NEED_POSMOS_DEFS:  POSMOS�� Type.h �� ����Ǿ� ������  PmsOS.h ������ �������� �ʱ� �����̴�.
//#define NEED_POSMOS_DEFS

///===================  basic defines
#if defined (ghs) || defined (__GNUC__) || defined (__hpux) || defined (__sgi) || defined (__DECCXX) || defined (__KCC) || defined (__rational__) || defined (__USLC__) || defined (ACE_RM544)
# define UNUSED_ARG(a) do {/* null */} while (&a == 0)
#else /* ghs || __GNUC__ || ..... */
# define UNUSED_ARG(a) (a)
#endif /* ghs || __GNUC__ || ..... */

#ifndef NULL
#define NULL 0
#endif

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#ifndef POLLNORM
#define	POLLNORM POLLIN // NOT defined in Linux
#endif

#ifdef _WIN32
#define __attribute__(X)
#endif

#ifdef _AIX
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <time.h>
#endif

///// include common headers
//#include "Config.h"
//#include "ErrCode.h"
//#include "Types.h"
//#include "API.h"

///=================== common macro functions

/// absolute value
#ifndef ABS
#define ABS(x) ((x)>=0?(x):-(x))
#endif

/// minimum value
#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

/// maximum value
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

/// size of string
#ifndef sizeofS
#define sizeofS(str) (sizeof(str) - 1)
#endif

/// size of array
#ifndef sizeofA
#define sizeofA(array) (sizeof(array)/sizeof(array[0]))
#endif


///============ _WIN32 SOCK: 
// �� �κ��� SOCKET �� windows�� type�� �����ϱ� ���� �Ʒ� ������ �� �����Ͽ��� �Ѵ�.

#ifdef _WIN32

#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif		

#define WIN32_LEAN_AND_MEAN

#ifndef _WINSOCKAPI_ // Winsock �� ���� ��� Winsock 2 API �� ���Ǵ��� Ȯ��
#define _WINSOCKAPI_
#endif
#pragma comment (lib, "ws2_32.lib") // Winsock ���̺귯�� �ڵ� ��ũ
#include <winsock2.h>
#include <windows.h>
#endif

///=========== socket �� system header files ����.
#ifdef _WIN32
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <share.h>
#include <io.h>
#include <time.h>
#endif


#ifndef _WIN32
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h> // for socket(), bind(), connect()
#include <arpa/inet.h> // for sockaddr_in, inet_ntoa()
#include <unistd.h> // for close()
#include <stdlib.h> // for exit()
#include <netdb.h> // for gethostbyname()
#include <sys/time.h>
#include <sys/wait.h>
#endif

#ifdef _AIX
//#include <errno.h>
//#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#endif



///============== �⺻ type ���� - define primitive data type
#ifdef _WIN32
typedef INT8				int8_t;
typedef INT16				int16_t;
typedef INT32				int32_t;
typedef UINT8				uint8_t;
typedef UINT16			uint16_t;
typedef UINT32			uint32_t;
#endif

#ifndef _WIN32 
typedef signed char         INT8, *PINT8;
typedef signed short        INT16, *PINT16;
typedef signed int          INT32, *PINT32;
typedef unsigned char       UINT8, *PUINT8;
typedef unsigned short      UINT16, *PUINT16;
typedef unsigned int        UINT32, *PUINT32;

typedef int                 INT;
typedef unsigned int        UINT;

typedef unsigned long       DWORD;
#ifndef BOOL
typedef int                 BOOL;
#endif
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;

typedef void *HANDLE;

typedef unsigned int ULONG32, *PULONG32;
typedef unsigned int DWORD32, *PDWORD32;
#endif


///======= POSMOS specific type/define
typedef void				P_VOID;
typedef int8_t				P_INT8;
typedef int16_t			P_INT16;
typedef int32_t			P_INT32;
typedef uint8_t			P_UINT8;
typedef uint16_t			P_UINT16;
typedef uint32_t			P_UINT32;
typedef float				P_FLOAT;
typedef double			P_DOUBLE;
typedef char				P_CHAR;
typedef int32_t			P_BOOLEAN;

/// UnixTime
#ifdef _WIN32
typedef __time32_t UnixTime;	///< seconds since epoch(1970-01-01 00:00:00)	
#endif
///==============================


/// Process ID
#ifdef _WIN32
typedef int pid_t;
#endif



/// Thread �ĺ���
#ifdef _WIN32
typedef DWORD pthread_t;
#endif


#ifdef _WIN32
/// mutex id
typedef HANDLE pthread_mutex_t;
#endif

/// ���� �޸� �ĺ���
#ifdef _WIN32
typedef unsigned int key_t;
typedef HANDLE SYS_SHM_ID;
const static SYS_SHM_ID INVALID_SHM_ID = INVALID_HANDLE_VALUE;
#else
#endif

/// Socket �ĺ���
#ifndef _WIN32
typedef int	SOCKET;
	#ifndef INVALID_SOCKET
	#define INVALID_SOCKET  (SOCKET)(-1)
	#define SOCKET_ERROR            (-1)
	#endif
#endif


#ifdef _WIN32
/// UnixTime
typedef __time32_t EpochTime;	///< seconds since epoch(1970-01-01 00:00:00)	
#else
typedef time_t EpochTime;
#endif


///  NEED_POSMOS_DEFS:  POSMOS�� Type.h �� ����Ǿ� ������  PmsOS.h ������ �������� �ʱ� �����̴�.
#ifdef NEED_POSMOS_DEFS 
/// Process ID
typedef pid_t SYS_PROCESS_ID;
//#define INVALID_SYS_PROCESS_ID -1
const static pid_t INVALID_SYS_PROCESS_ID = (pid_t) (-1);

/// Thread �ĺ���
typedef pthread_t SYS_THREAD_ID;
//#define INVALID_SYS_THREAD_ID 0xFFFF
const static pthread_t INVALID_SYS_THREAD_ID = 0xFFFF;

/// ���� �޸� �ĺ���
#ifndef _WIN32
typedef int SYS_SHM_ID;
const static SYS_SHM_ID INVALID_SHM_ID = -1;
//#define INVALID_SHM_ID -1
#endif

/// IPv4 �ּ�
typedef P_UINT32 IPAddr;

/// Mutex, CondVar�� ����ȭ ���� ����
typedef enum
{
	INTRA_PROC = 0,			///< Process ���� Thread�� ������ ���ȭ
	INTER_PROC				///< ���� �ý����� Process�� ������ ����ȭ
} SynchronizerScope;
#endif

///======== SHM
//-- for mmap()
#ifndef MAP_FAILED
#define MAP_FAILED ((void *) -1)
#endif

#ifndef MAP_VARIABLE
#define	MAP_VARIABLE 0 // NOT defined in Linux
#endif


///============= _WIN32 SOCK: unix ���� ���������� Win32 �� ���� ��.
#ifdef _WIN32
typedef int in_addr_t;
typedef unsigned short in_port_t;
typedef int	socklen_t;

struct sockaddr_un {
	unsigned short sun_family;  /* AF_UNIX */
	char sun_path[108];
};

//====
struct timespec
{
	time_t   tv_sec;   /* seconds */
	long     tv_nsec;  /* nanoseconds */
};

struct sched_param { 
	int32_t  sched_priority; 
	int32_t  sched_curpriority; 
	union { 
		int32_t  reserved[8]; 
		struct {    
			int32_t  __ss_low_priority;  
			int32_t  __ss_max_repl;  
			struct timespec     __ss_repl_period;   
			struct timespec     __ss_init_budget;   
		}           __ss;   
	}           __ss_un;    
};

#define sched_ss_low_priority   __ss_un.__ss.__ss_low_priority
#define sched_ss_max_repl       __ss_un.__ss.__ss_max_repl
#define sched_ss_repl_period    __ss_un.__ss.__ss_repl_period
#define sched_ss_init_budget    __ss_un.__ss.__ss_init_budget


///================ UNIX poll() 
struct pollfd
{
	SOCKET fd;         // �����־��ϴ� ����������
	short events;   // �߻��� �̺�Ʈ
	short revents;  // �������� �̺�Ʈ
};


/// Windows WSAPoll()�� ����. �Ʒ��� define���� Unix define���� �ٸ���. �׷��� #undef �ʿ�. 
/// WSAPoll() -  Windows 2008 �̻󿡼� ����.
#undef POLLIN
#undef POLLPRI
#undef POLLOUT
#undef POLLERR
#undef POLLHUP
#undef POLLNVAL
#undef POLLRDNORM

/// unix header�� ���� �����ϰ� ����.
#define POLLIN      0x0001  // ���� ����Ÿ�� �ִ�.
#define POLLPRI     0x0002  // ����� ���� ����Ÿ�� �ִ�.
#define POLLOUT     0x0004  // ���Ⱑ ����(block)�� �ƴϴ�. 
#define POLLERR     0x0008  // �����߻�
#define POLLHUP     0x0010  // ������ ������
#define POLLNVAL    0x0020  // ���������ڰ� ������ �����Ͱ��� // Invalid request (�߸��� ��û)
#define POLLRDNORM	POLLIN
#define POLLNORM	POLLIN  //  hp-ux: Synonym for POLLIN

///=========

#define SIGUSR1 SIGINT /* user defined signal 1 */
#define SIGUSR2 SIGINT /* user defined signal 2 */
#define SIGPIPE SIGFPE

//*** User ID ����
typedef unsigned int uid_t;
typedef unsigned int gid_t;

#define	SCHED_OTHER	0
#define	SCHED_RR	0

#define	PRIO_PROCESS	0
#define	PRIO_PGRP	0
#define	PRIO_USER	0

// UNIX pthread.h
#define PTHREAD_CANCEL_ENABLE 0
#define PTHREAD_CANCEL_ASYNCHRONOUS 1


static inline uid_t getuid()
{
	return 0;
}

static inline int setuid(uid_t uid)
{
	uid = 0;
	return 0;
}

static inline int setresuid(uid_t ruid, uid_t euid, uid_t suid)
{
	ruid = euid = suid = 0;
	return 0;
}

static inline int setresgid(gid_t rgid, gid_t egid, gid_t sgid)
{
	rgid = egid = sgid = 0;
	return 0;
}

static inline int sched_get_priority_max(int policy)
{
	policy = 0;
	return 0;
}

static inline int sched_get_priority_min(int policy)
{
	policy = 0;
	return 0;
}

static inline int sched_setscheduler(pid_t pid, int policy,  const struct sched_param *param)
{
	pid = policy = 0;
	param = 0;
	return 0;
}

static inline int sched_getscheduler(pid_t pid)
{
	pid = 0;
	return 0;
}

static inline int getpriority(int which, int who)
{
	which = who = 0;
	return 0;
}

static inline int setpriority(int which, int who, int prio)
{
	which = who = prio = 0;
	return 0;
}
#endif /// _WIN32



//====== String 
#ifdef _WIN32
	#define STRNCPY	strncpy_s
	#define STRCPY	strcpy_s
#else
	#define STRNCPY	strncpy
	#define STRCPY	strcpy
#endif

#ifdef _WIN32
static inline void close(SOCKET s) {
	::closesocket(s);
}

static inline void sleep(unsigned int secs) {
	::Sleep(secs*1000);
}

// ���μ����� ���� �����ǵ��� �ϱ� ���� �ʿ�. UNIX������ �⺻ fopen() ���� ������,WIN32 ������ _fsopen() ��� �ʿ�.
#define fopen(A, B) _fsopen(A, B, _SH_DENYNO)


/*
Return Value
mkdir() returns zero on success, or -1 if an error occurred (in which case, errno is set appropriately). 
*/
static int mkdir(const char *pathname, unsigned int mode)
{
	int ret = CreateDirectory(pathname, NULL); // ret is nonzero if succesful.	
	/*
	-- CreateDirectory()
	Return Value
	If the function succeeds, the return value is nonzero.
	If the function fails, the return value is zero. To get extended error information, call GetLastError. Possible errors include the following.

	Return code Description 
	ERROR_ALREADY_EXISTS The specified directory already exists. 
	ERROR_PATH_NOT_FOUND One or more intermediate directories do not exist; this function will only create the final directory in the path. 
	*/ 

	if (ret != 0) {
		return 0; // success
	}
	else {
		// ���丮�� �̹� �����ϴ� ���.
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			errno = EEXIST;
			return 0; // unix ��Ŀ� ������  return -1�� �ؾ� ������, ���� return  �ϱ�� �Ѵ�.
		}
		else 
			return -1; // fail  
	}
}

#define PATH_MAX		MAX_PATH

///InetNtop : Windows Vista, Windows Server 2008 �̻󿡼� ����.
// http://msdn.microsoft.com/en-us/library/cc805843(VS.85).aspx
//#define inet_ntop	InetNtop  
//#define	inet_ntop(A,B,C,D)	SocketUtil::int2ipaddr_NetOrder((unsigned int)(B),(C),(D))

#define	usleep	Sleep

#define	strdup	_strdup // _WIN32
#define strncasecmp _strnicmp
#define strcasecmp _stricmp

#define snprintf _snprintf

#define chdir	SetCurrentDirectory

#define unlink	_unlink // _WIN32
//#define stat(_path, _buffer)	_stat((_path), (_buffer)) // _WIN32 -- stat �̹� win32�� �����ϹǷ� �ʿ����.
#define fileno(_FP)	_fileno((_FP)) // _WIN32
#define ftruncate(_FD, _FSIZE)	_chsize_s((_FD), (__int64)(_FSIZE)) // http://msdn.microsoft.com/en-us/library/whx354w1(VS.80).aspx

// errno Constants -- http://msdn.microsoft.com/en-us/library/5814770t(VS.100).aspx
#define EDQUOT ENOSPC

#undef LIBXML_ICONV_ENABLED

#define SHUT_RD	SD_RECEIVE
#define SHUT_WR	SD_SEND
#define SHUT_RDWR	SD_BOTH 

#define EALREADY WSAEALREADY
#define EISCONN WSAEISCONN
#define EINPROGRESS WSAEWOULDBLOCK
#define ETIMEDOUT WSAETIMEDOUT
//#undef EINTR // windows errno.h �� ���ǵǾ� ����.
//#define EINTR	WSAEWOULDBLOCK 
#define ECONNRESET	WSAECONNRESET
// WSAECONNRESET, WSAENOTCONN, WSAESHUTDOWN
#define ENETDOWN WSAENETDOWN
#define ENOTCONN WSAENOTCONN
#define EWOULDBLOCK WSAEWOULDBLOCK
#define ENETUNREACH WSAENETUNREACH
#define EHOSTUNREACH WSAEHOSTUNREACH
#define ENOBUFS                 WSAENOBUFS
//typedef size_t  ssize_t; // wrong !!
typedef int ssize_t; // ssize_t: It is similar to size_t, but must be a signed type.  int or long int

#endif

#ifdef _WIN32
#define SLEEP(X) Sleep((X))
#else
#define SLEEP(X) usleep((X)*1000)
#endif

#if defined (SD_RECEIVE)
#define SHUTDOWN_READ SD_RECEIVE
#elif defined (SHUT_RD)
#define SHUTDOWN_READ SHUT_RD
#else
#define SHUTDOWN_READ 0
#endif /* SD_RECEIVE */

#if defined (SD_SEND)
#define SHUTDOWN_WRITE SD_SEND
#elif defined (SHUT_WR)
#define SHUTDOWN_WRITE SHUT_WR
#else
#define SHUTDOWN_WRITE 1
#endif /* SD_SEND */

#if defined (SD_BOTH)
#define SHUTDOWN_BOTH SD_BOTH
#elif defined (SHUT_RDWR)
#define SHUTDOWN_BOTH SHUT_RDWR
#else
#define SHUTDOWN_BOTH 2
#endif /* SD_BOTH */

#if defined(_XOPEN_SOURCE) || defined(_XOPEN_SOURCE_EXTENDED) 
	#define	OPTVAL_CAST	(void *)
	#define	OPTLENPTR_CAST	(socklen_t *)
	#define	OPTLEN_CAST	(socklen_t)

#else
	#if defined(_WIN32)
	#define	OPTVAL_CAST	(char *)
	#define	OPTLENPTR_CAST	(int *)
	//#error "============= WIN32 ============"
	#define	OPTLEN_CAST	(int)
	#elif defined(hpux) || defined(_hpux) || defined(_HPUX) || defined(HPUX)
	#define	OPTVAL_CAST	(void *)
	#define	OPTLENPTR_CAST	(int *)
	#define	OPTLEN_CAST	(int)
	//#error "============= HPUX ============"
	#else
	#define	OPTVAL_CAST	(void *)
	#define	OPTLENPTR_CAST	(socklen_t *)
	#define	OPTLEN_CAST	(socklen_t)
	//#error "============= AIX or Linux ============"
	#endif

#endif // defined(_XOPEN_SOURCE)

#endif // PMS_OS_H
