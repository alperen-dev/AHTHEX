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

#ifndef LOG_H
#define LOG_H

#include "ahtdefs.h"

#define LOG_FILE_NAME "ahthex.log"

#define LOG_ERROR(errno, message) log_error(errno, __FILE__, __LINE__, message)

bool log_init(void);
bool log_close(void);
bool logf(const char *format, ...);
bool log_error(int errno, const char *fileName, int line, const char *message);

#endif /* LOG_H */
