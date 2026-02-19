#include "log.h"

#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static FILE *logFile = NULL;

bool log_init(void)
{
	if(logFile != NULL) /* already initialized */
	{
		return false;
	}
	
	if((logFile = fopen(LOG_FILE_NAME, "a")) == NULL)
	{
		return false;
	}
	return true;
}

bool log_close(void)
{
	if(logFile == NULL) /* already closed (or not initialized) */
	{
		return false;
	}
	
	if(!fclose(logFile))
	{
		logFile = NULL;
		return true;
	}
	return false;
}

bool logf(const char *format, ...) /* log formatted, like print formatted */
{
	va_list args;
	if(logFile == NULL)
		return false;
	va_start(args, format);
	vfprintf(logFile, format, args);
	va_end(args);
	return true;
}

bool log_error(int errno, const char *fileName, int line, const char *message)
{
	if(logFile == NULL)
	{
		return false;
	}
	else
	{
		time_t ti = time(NULL);
		struct tm *t = localtime(&ti);
		logf("Date: %02d.%02d.%04d, Time: %02d.%02d.%02d, Errno: %d, File Name: %s, Line: %d\nError: %s\n",
			t->tm_mday, t->tm_mon+1, t->tm_year+1900, t->tm_hour, t->tm_min, t->tm_sec, errno, fileName, line, (message == NULL ? strerror(errno) : message));
		return true;
	}
}
