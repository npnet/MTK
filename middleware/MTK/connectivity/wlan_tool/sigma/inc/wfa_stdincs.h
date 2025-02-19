/****************************************************************************
*
* Copyright (c) 2016 Wi-Fi Alliance
*
* Permission to use, copy, modify, and/or distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
* WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
* SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
* RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
* NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
* USE OR PERFORMANCE OF THIS SOFTWARE.
*
*****************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#ifndef _FREERTOS
#include <signal.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#else
#include "lwip/sockets.h"
#include "lwip/inet.h"
#endif

#ifndef _FREERTOS
#ifdef __CYGWIN__
#include <cygwin/types.h>
#include <cygwin/socket.h>
#include <cygwin/if.h>
#else
#include <linux/types.h>
#include <linux/socket.h>
#include <linux/if.h>
#endif
#endif
#include <sys/select.h>
#ifndef _FREERTOS
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#endif
#include <sched.h>
#include <math.h>
#include <errno.h>

