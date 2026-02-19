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
