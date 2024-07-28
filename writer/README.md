# Proceso Writer

Este proceso debe imprimir su propio PID y luego entrar en un bucle esperando a que el usuario ingrese texto hasta que presione ENTER. En ese momento escribirá en una named FIFO los datos con el siguiente formato:

`DATA:XXXXXXXXX`

En cualquier momento el proceso podrá recibir las signals SIGUSR1 y SIGUSR2. En dicho caso deberá escribir en el named FIFO el mensaje SIGN:1 o SIGN:2 y contnuar con su ejecución normal. Las signals se pueden recibir más de una vez.

En cualquier momento, además, el proceso writer puede detectar el EOF en la entrada estándar (indicada presionando CTRL+D en la consola). En este caso el writer debe finalizar la ejecución.

## Comando para mandar señales

`kill -SIGUSR2 $(ps -e | grep writer_program | awk '{print $1}')`
