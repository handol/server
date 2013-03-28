/**
*/

#include "OS.h"
#include "LogMsg.h"
#include <ctype.h>
#include <time.h>
#ifndef WIN32
#include <sys/time.h>
#endif


LogMsg * LogMsg::_instance = NULL;
char LogMsg::fName[256] = {0};
FILE * LogMsg::logfd = 0;
int LogMsg::old_day = 0;

int LogMsg::priority = 0;
int LogMsg::timeprint = 1;
int LogMsg::thridprint = 1;
int LogMsg::tostd = 0;
struct tm LogMsg::tm;

#ifndef _WIN32
#define strcpy_s(X, Y) strcpy(X, Y)
#define localtime_s(TM, TIME_T)	localtime_r(TIME_T, TM)
#define sprintf_s snprintf
#define vsprintf_s vsnprintf
#endif

LogMsg * LogMsg::getInstance()
{
    if (_instance==0)
    {
        _instance = new LogMsg();
		

    }
    return _instance;
}

int LogMsg::open(char *module_name)
{
	strcpy_s(fName, module_name);	
	return 0;
}

int LogMsg::close()
{
    if (logfd != NULL)
		fclose(logfd);
    return 0;
}



/**
화일을 지우고 새로 쓰기 (overwrite)
*/
void	LogMsg::overwrite()
{
    char logf_path[256];
    time_t  t_val;

    time(&t_val);
    localtime_s(&tm, &t_val);
    if (logfd == 0 || old_day != tm.tm_mday)
    {
        old_day = tm.tm_mday;

        if (logfd && !tostd) fclose(logfd);        
        sprintf_s(logf_path, sizeof(logf_path)-1, "%s.%02d%02d.log", fName, tm.tm_mon + 1, tm.tm_mday);
		#ifndef _WIN32
        logfd = fopen(logf_path, "w+");
		#else
        fopen_s(&logfd, logf_path, "w+");
		#endif
    }
}


// 일자가 바뀌었거나 화일 open이 안된 경우 새로 화일을 연다.
void	LogMsg::logopen()
{
    char logf_path[256];
    time_t  t_val;

	if (tostd) return;
    time(&t_val);
    localtime_s(&tm, &t_val);
    if (logfd == 0 || old_day != tm.tm_mday)
    {
        old_day = tm.tm_mday;

        if (logfd  && !tostd) fclose(logfd);        
        sprintf_s(logf_path, sizeof(logf_path)-1, "%s.%02d%02d.log", fName, tm.tm_mon + 1, tm.tm_mday);
		#ifndef _WIN32
        logfd = fopen(logf_path, "w+");
		#else
        fopen_s(&logfd, logf_path, "w+");
		#endif
    }
}

/**
-- printf() format --
http://www.dgp.toronto.edu/~ajr/209/notes/printf.html

%ld long 
%u unsigned decimal number 
%lu unsigned long decimal number 
*/
void LogMsg::logprint(int pri, const char *fmt, ...)
{
	va_list args;
	char buf[1024*10]={0};

    if (pri < priority && fmt != NULL) return;
    logopen();

    va_start(args, fmt);
    vsprintf_s(buf, sizeof(buf)-1, fmt, args);
    va_end(args);

    fprintf(logfd, "%02d:%02d:%02d ", tm.tm_hour, tm.tm_min, tm.tm_sec);    
    fprintf(logfd, "%s\n", buf);
	fflush(logfd);
}


void LogMsg::_goodhex(byte *ptr, int size, int maxsize)
{
    int i;
	int	half, full;
	full = MIN(maxsize, size);
	half = MIN(maxsize >> 1, size);
	
    for(i=0; i<half; i++)
        fprintf(logfd, "%02X ", ptr[i]);
    fprintf(logfd, "  ");
		
	for(    ;i<full;i++)
		fprintf(logfd, "%02X ", ptr[i]);

	for(    ;i<maxsize;i++)
		fprintf(logfd, "   ");
	
    for(i=0; i<full; i++)
        if (isprint(ptr[i])) fprintf(logfd, "%c", ptr[i]);
    else fprintf(logfd, ".");
    fprintf(logfd, "\n");
	fflush(logfd);

}


void LogMsg::hexdump(int pri, const char *src, int size)
{
    int i;
    if (pri < priority || src==NULL) return;
    logprint(pri, "HEXDUMP %d bytes", size);
    for(i=0; i<size; i+= 16)
        _goodhex((byte*)src+i, size-i, 16);
	fprintf(logfd, "\n");
    fflush(logfd);
}
