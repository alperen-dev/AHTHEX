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

#ifndef W32PROC_H
#define W32PROC_H

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include "ahtdefs.h"

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x00000004L
#endif

#endif /* W32PROC_H */