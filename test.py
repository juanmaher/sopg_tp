import os
import signal
import subprocess
import time

# Ejecutar el proceso writer
writer_process = subprocess.Popen(
    ['./out/writer_program'],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    text=True
)

# Esperar un poco para que el proceso writer imprima su PID
time.sleep(1)

# Leer y mostrar el PID del proceso writer
writer_pid = writer_process.stdout.readline().strip()
print(f"PID del proceso writer: {writer_pid}")

# Ejecutar el proceso reader
reader_process = subprocess.Popen(
    ['./out/reader_program'],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    text=True
)

# Esperar un poco para que el proceso reader se inicie
time.sleep(1)

# Enviar datos al proceso writer
writer_process.stdin.write("Hello\n")
writer_process.stdin.flush()

# Esperar un poco para que el proceso writer escriba en el FIFO
time.sleep(1)

# Enviar señales al proceso writer
os.kill(int(writer_pid), signal.SIGUSR1)
time.sleep(1)
os.kill(int(writer_pid), signal.SIGUSR2)

# Enviar EOF al proceso writer
writer_process.stdin.close()

# Esperar a que los procesos finalicen
writer_process.wait()
reader_process.wait()

# Leer los archivos de salida
with open('out/log.txt', 'r') as log_file:
    log_data = log_file.read()

with open('out/signals.txt', 'r') as signals_file:
    signals_data = signals_file.read()

# Imprimir los resultados
print("Contenido de log.txt:")
print(log_data)

print("Contenido de signals.txt:")
print(signals_data)

# Validar los resultados
assert "DATA:Hello" in log_data, "El mensaje de datos no se encontró en log.txt"
assert "SIGN:1" in signals_data, "La señal SIGN:1 no se encontró en signals.txt"
assert "SIGN:2" in signals_data, "La señal SIGN:2 no se encontró en signals.txt"

print("Pruebas completadas con éxito")
