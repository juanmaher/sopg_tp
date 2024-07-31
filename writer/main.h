#ifndef __WRITER_H__
#define __WRITER_H__

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"

#define SIGNAL_1_CONTENT "1\n"
#define SIGNAL_2_CONTENT "2\n"

#endif // __WRITER_H__