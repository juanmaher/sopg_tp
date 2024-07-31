# Trabajo Práctico de SOPG - CESE FIUBA
Comunicación entre dos procesos de Linux mediante Named FIFOs

## Proceso Reader

Este proceso lee los datos del named FIFO y según el encabezado DATA o SIGN los escribe en el archivo log.txt o signals.txt.

## Proceso Writer

Este proceso imprime su propio PID y luego entra en un bucle esperando a que el usuario ingrese texto hasta que presione ENTER. En ese momento escribe en una named FIFO los datos con el siguiente formato:

`DATA:XXXXXXXXX`

En cualquier momento el proceso puede recibir las signals SIGUSR1 y SIGUSR2. En dicho caso escribe en el named FIFO el mensaje SIGN:1 o SIGN:2 y continua con su ejecución normal. Las signals se pueden recibir más de una vez.

En cualquier momento, además, el proceso writer puede detectar el EOF en la entrada estándar (indicada presionando CTRL+D en la consola). En este caso el writer debe finalizar la ejecución.


## Como compilar?
- Para generar ejecutables: `make all`
- Para limpiar las salidas: `make clean`


## Como ejecutar?
1. Iniciar el proceso writer: `./out/write_program`
2. Iniciar el proceso reader: `./out/reader_program`
3. Enviar desde STDIN en la terminal que esté ejecutando el writer
4. Terminar la ejecución con `CTRL+D`
5. Ver resultados en `out/log.txt` y en `signals.txt`


## Comando para mandar señales

`kill -SIGUSR2 $(ps -e | grep writer_program | awk '{print $1}')`
