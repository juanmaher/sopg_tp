# Definimos los compiladores y flags
CC = gcc
CFLAGS = -std=gnu11 -Wall -pedantic

# Definimos las carpetas
OUT_DIR = out
WRITER_DIR = writer
READER_DIR = reader
COMMON_DIR = common

# Directorios de inclusión
INCLUDE_DIRS = -I$(COMMON_DIR)

WRITER_SRC = $(wildcard $(WRITER_DIR)/*.c)
READER_SRC = $(wildcard $(READER_DIR)/*.c)

WRITER_OBJ = $(WRITER_SRC:.c=.o)
READER_OBJ = $(READER_SRC:.c=.o)

WRITER_HEADERS = $(wildcard $(WRITER_DIR)/*.h)
READER_HEADERS = $(wildcard $(READER_DIR)/*.h)
COMMON_HEADERS = $(wildcard $(COMMON_DIR)/*.h)

# Archivos ejecutables
WRITER_EXEC = $(OUT_DIR)/writer_program
READER_EXEC = $(OUT_DIR)/reader_program

# Regla para compilar todo
all: $(WRITER_EXEC) $(READER_EXEC)

writer: $(WRITER_EXEC)
reader: $(READER_EXEC)

$(WRITER_EXEC): $(WRITER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^
$(READER_EXEC): $(READER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(WRITER_DIR)/%.o: $(WRITER_DIR)/%.c $(WRITER_HEADERS) $(COMMON_HEADERS)
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c -o $@ $<
$(READER_DIR)/%.o: $(READER_DIR)/%.c $(READER_HEADERS) $(COMMON_HEADERS)
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c -o $@ $<

clean:
	rm -f $(WRITER_OBJ) $(READER_OBJ) $(WRITER_EXEC) $(READER_EXEC)

clean_exec:
	rm -f $(WRITER_EXEC) $(READER_EXEC)

clean_obj:
	rm -f $(WRITER_OBJ) $(READER_OBJ)

.PHONY: all writer reader clean clean_exec clean_obj
