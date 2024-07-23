# Definimos los compiladores y las banderas
CC = gcc
CFLAGS = -std=gnu11 -Wall -pedantic

# Definimos las carpetas
WRITER_DIR = writer
READER_DIR = reader

# Archivos fuente
WRITER_SRC = $(wildcard $(WRITER_DIR)/*.c)
READER_SRC = $(wildcard $(READER_DIR)/*.c)

# Archivos objeto
WRITER_OBJ = $(WRITER_SRC:.c=.o)
READER_OBJ = $(READER_SRC:.c=.o)

# Archivos de cabecera
WRITER_HEADERS = $(wildcard $(WRITER_DIR)/*.h)
READER_HEADERS = $(wildcard $(READER_DIR)/*.h)

# Archivos ejecutables
WRITER_EXEC = writer_program
READER_EXEC = reader_program

# Regla para compilar todo
all: $(WRITER_EXEC) $(READER_EXEC)

# Regla para compilar el programa writer
writer: $(WRITER_EXEC)

# Regla para compilar el programa reader
reader: $(READER_EXEC)

# Regla para compilar los archivos de writer
$(WRITER_EXEC): $(WRITER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Regla para compilar los archivos de reader
$(READER_EXEC): $(READER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Regla para compilar los archivos .c a .o en writer
$(WRITER_DIR)/%.o: $(WRITER_DIR)/%.c $(WRITER_HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

# Regla para compilar los archivos .c a .o en reader
$(READER_DIR)/%.o: $(READER_DIR)/%.c $(READER_HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

# Limpiar archivos objeto y ejecutables
clean:
	rm -f $(WRITER_OBJ) $(READER_OBJ) $(WRITER_EXEC) $(READER_EXEC)

# Limpiar solo los ejecutables
clean_exec:
	rm -f $(WRITER_EXEC) $(READER_EXEC)

# Limpiar solo los archivos objeto
clean_obj:
	rm -f $(WRITER_OBJ) $(READER_OBJ)

.PHONY: all writer reader clean clean_exec clean_obj
