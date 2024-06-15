# Shell con control de tareas para Linux

En este proyecto universitario, he desarrollado un **intérprete de comandos** o **_Shell_** de aspecto similar al ya existente en Linux, que permite
al usuario lanzar sus programas y ejecutar comandos de manera personalizada

## Uso

1. Compilar
   
   ```bash
   gcc ProyectoShell.c ApoyoTareas.c -o MiShell
   ```
2. Ejecutar
   ```bash
   ./MiShell
   ```
3. Salir:
   
   Se sale pulsando `Control+D` 
   
Este shell, leerá un comando tecleado por el usuario y generará un proceso hijo que lo ejecute. Para ello, se apoyará en las siguientes cuatro llamadas al sistema:

- **fork():** Crea un proceso hijo.
- **waitpid():** Espera a que termine un proceso hijo.
- **execvp():** Lanza un programa.
- **exit():** Termina un proceso.

Al ejecutar un comando en nuestra shell, todos los procesos involucrados y sus hijos forman un grupo de procesos. Por ejemplo, al compilar un programa con gcc, los procesos de preprocesamiento, compilación, ensamblaje y enlace forman parte del mismo grupo. Nuestra shell permite el acceso exclusivo al terminal a un único grupo de procesos en cada momento, correspondiente al comando en primer plano (foreground - FG).

Los demás grupos de procesos lanzados por la shell se ejecutan en segundo plano (background - BG) sin acceso al terminal. La shell permanecerá en primer plano mientras acepta comandos del usuario, transfiriendo el control del terminal al comando ejecutado en primer plano y recuperándolo cuando este termine o se suspenda, lista para recibir el siguiente comando.

## Funcionalidades Principales
### Ejecución de Comandos:

Utilizando las llamadas al sistema **fork()**, **waitpid()**, **execvp()** y **exit()**, el Shell podrá crear procesos hijos para ejecutar comandos.
Los comandos se ejecutarán en primer plano o segundo plano según lo indicado por el usuario.

### Gestión de Procesos:

Los procesos se agruparán en grupos de procesos, lo que permitirá gestionar la ejecución de múltiples comandos simultáneamente.
El Shell mantendrá el control del terminal mientras está en primer plano, cediéndolo al comando en ejecución y recuperándolo al finalizar dicho comando.

### Comandos Internos:

Implementación de comandos internos como **_cd_** para cambiar el directorio de trabajo, **_bg_** y **_fg_** para poner a tareas en primer o segundo plano, **_logout_** para salir del Shell y **_jobs_** para imprimir la lista de tareas.

### Interfaz de Usuario:

El Shell mostrará un prompt personalizado para la entrada de comandos.
Se proporcionará retroalimentación sobre la ejecución de los comandos, incluyendo el pid, estado y cualquier información relevante.


## Ciclo de Desarrollo

El desarrollo siguió un ciclo iterativo de compilación, ejecución, depuración y adición de nuevas funcionalidades, asegurando un crecimiento incremental y controlado del proyecto.
