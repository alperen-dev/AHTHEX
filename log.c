/*
* ahthex - a cross platform hex editor
* Copyright (C) 2026 alperen-dev
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*/

#include "log.h"

#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>


static FILE *logFile = NULL;


static char *get_time_as_string(char *buffer, size_t length)
{
	time_t ti = 0;
	
	assert(buffer);
	
	ti = time(NULL);
	
	strftime(buffer, length, "%Y-%m-%d %H:%M:%S %Z", localtime(&ti));
	
	return buffer;
}

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
	else
	{
		char buffer[64];
		logf("[+] Logger Started at %s\n", get_time_as_string(buffer, sizeof(buffer)));
	}
	
	return true;
}

bool log_close(void)
{
	if(logFile == NULL) /* already closed (or not initialized) */
	{
		return false;
	}
	else
	{
		char buffer[64];
		logf("[+] Logger Closed at %s\n\n", get_time_as_string(buffer, sizeof(buffer)));
		
		if(!fclose(logFile))
		{
			logFile = NULL;
			return true;
		}
	}
	return false;
}

bool logf(const char *format, ...) /* log formatted, like print formatted */
{
	va_list args;
	if(logFile == NULL)
	{
		return false;
	}
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
		char buffer[64];
		logf("[-] %s Errno: %d, File Name: %s, Line: %d\n\tError: %s\n",
			get_time_as_string(buffer, sizeof(buffer)), errno, fileName, line, (message == NULL ? strerror(errno) : message));
		return true;
	}
}
