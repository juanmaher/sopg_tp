#ifndef __READER_H__
#define __READER_H__

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "common.h"

#define DATA_FILE "out/log.txt"
#define SIGNALS_FILE "out/signals.txt"

#endif // __READER_H__