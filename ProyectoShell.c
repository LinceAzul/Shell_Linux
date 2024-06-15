/*------------------------------------------------------------------------------
Proyecto Shell de UNIX. Sistemas Operativos
Grados I. Inform�tica, Computadores & Software
Dept. Arquitectura de Computadores - UMA

Algunas secciones est�n inspiradas en ejercicios publicados en el libro
"Fundamentos de Sistemas Operativos", Silberschatz et al.

Para compilar este programa: gcc ProyectoShell.c ApoyoTareas.c -o MiShell
Para ejecutar este programa: ./MiShell
Para salir del programa en ejecuci�n, pulsar Control+D
------------------------------------------------------------------------------*/

#include "ApoyoTareas.h" // Cabecera del m�dulo de apoyo ApoyoTareas.c
 
#define MAX_LINE 256 // 256 caracteres por l�nea para cada comando es suficiente
#include <string.h>  // Para comparar cadenas de cars. (a partir de la tarea 2)

// Autor: Iroslavov Petkov, Ivan

job *jobs;

void manejador(int sig) {
    block_SIGCHLD();
    job *aux;
    int status, info, pid_wait = 0;
    enum status status_res; 
    for(int i = 1; i <= list_size(jobs); ++i) {
        aux = get_item_bypos(jobs, i);
		pid_wait = waitpid(aux->pgid, &status, WUNTRACED | WNOHANG);
		// Si el pid del trabajo coincide con el que le ha enviado la senal
		if(pid_wait == aux->pgid) {
	  	  	status_res = analyze_status(status, &info); // Analizamos el tipo de senal
	    	if(status_res == FINALIZADO){
				printf("Comando %s ejecutado en segundo plano con  PID %d ha concluido\n", aux->command, aux->pgid);
	       		 delete_job(jobs, aux); // y eliminamos el trabajo de la lista
	    	}
	    	else if(status_res == SUSPENDIDO) {
			printf("Comando %s ejecutado en segundo plano con  PID %d ha suspendido su ejecucion\n", aux->command, aux->pgid);
			aux->ground = DETENIDO;
	    	}
		}// fin if
    }// fin for
    unblock_SIGCHLD();
} // fin manejador
// --------------------------------------------
//                     MAIN          
// --------------------------------------------

int main(void) {
      char inputBuffer[MAX_LINE]; // B�fer que alberga el comando introducido
      int background;         // Vale 1 si el comando introducido finaliza con '&'
      char *args[MAX_LINE/2]; // La l�nea de comandos (de 256 cars.) tiene 128 argumentos como m�x
                              // Variables de utilidad:
      int pid_fork, pid_wait; // pid para el proceso creado y esperado
      int status;             // Estado que devuelve la funci�n wait
      enum status status_res; // Estado procesado por analyze_status()
      int info;		      // Informaci�n procesada por analyze_status()
      
      job *item;
      ignore_terminal_signals();  
      signal(SIGCHLD, manejador); // leemos las senales de los hijos
      jobs = new_list("jobs"); // creamos la lsita de trabajos
      int primerPlano = 0; 

      while (1) // El programa termina cuando se pulsa Control+D dentro de get_command()
      {   		
        printf("COMANDO->");
        fflush(stdout);
        get_command(inputBuffer, MAX_LINE, args, &background); // Obtener el pr�ximo comando
        if (args[0]==NULL) continue; // Si se introduce un comando vac�o, no hacemos nada
    /* Los pasos a seguir a partir de aqu�, son:
       (1) Genera un proceso hijo con fork()
       (2) El proceso hijo invocar� a execvp()
       (3) El proceso padre esperar� si background es 0; de lo contrario, "continue" 
       (4) El Shell muestra el mensaje de estado del comando procesado 
       (5) El bucle regresa a la funci�n get_command()
    */	
    	
    	if(strcmp(args[0], "logout") == 0) {
    	    if(args[1] != NULL && strlen(args[1]) > 0) {
    	    	printf("Error. Comando %s %s... no encontrado.\n", args[0], args[1]);
    	    }
    	    else {
    	        printf("Saliendo del Shell");
    	        exit(0);
    	    }
    	}
    	else if(strcmp(args[0], "cd") == 0) { // Cambio de directorio
    	    if(args[1] == NULL || strlen(args[1]) == 0) {
    	    	const char* home_dir = getenv("HOME");
    	    	chdir(home_dir);
    	    } // chdir devuelve 0, funciona perfecto, sino falla
    	    else if(chdir(args[1]) == -1) {
    	    	printf("cd: %s: No existe el archivo o el directorio\n", args[1]);
    	    }
    	}
		else {
			if(!strcmp(args[0], "jobs")) { // Imprime la lista de tareas
				block_SIGCHLD();
				print_job_list(jobs);
				unblock_SIGCHLD();
				continue;
			}
			if(!strcmp(args[0], "fg")) { // Manda a 1er plano una tarea detenida o en 2o plano
				block_SIGCHLD();
				int pos = 1;
				primerPlano = 1;
				// Creo que cogemos la posicion en la que este args[1]
				if(args[1] != NULL) {
					pos = atoi(args[1]); // preguntar por atoi()??????
				}	
				// Tomamos la tarea con esa posicion en la lista (variable auxiliar item creada fuera)
				item = get_item_bypos(jobs, pos);
				if(item != NULL) {
					set_terminal(item->pgid); // Le cedo la terminal al proceso en cuestion
					if(item->ground == DETENIDO) { killpg(item->pgid, SIGCONT); } // Continue if stopped 
					// inputBuffer inicializado al principio del programa
					// Copio ahi, el comando que tenga en la tarea de la lista
					strcpy(inputBuffer, item->command);
					pid_fork = item->pgid;
					delete_job(jobs, item);
					unblock_SIGCHLD();
			} else { 
				printf("FG ERROR: Job not found\n"); 
				unblock_SIGCHLD();
				continue;
			}
				
			}
			if(!strcmp(args[0], "bg")) { // Manda la tarea pasada como parametro a fg
				block_SIGCHLD();
				int pos = 1;
				if (args[1] != NULL) {
				pos = atoi(args[1]);
				}
				item = get_item_bypos(jobs, pos);
				if(item != NULL && item->ground == DETENIDO){
					item->ground = SEGUNDOPLANO;
					//printf("Puesto el primer plano el job el comando %s en la posicion %d", item->command, pos);
					killpg(item->pgid, SIGCONT);
				}
				unblock_SIGCHLD();
				continue;
			}
			if(!primerPlano) pid_fork = fork(); // FORK
			
			if(pid_fork > 0) { // PROCESO PADRE
			if(background == 0) {
				pid_wait = waitpid(pid_fork, &status, WUNTRACED);
				set_terminal(getpid());
				status_res = analyze_status(status, &info);
				if(status_res == FINALIZADO) {
					if(info != 255) {
						printf("Comando %s ejecutado en primer plano con pid %d. Estado %s. Info: %d.\n", args[0], pid_fork, status_strings[status_res], info);
					}
				}
				else if(status_res == SUSPENDIDO) {
					block_SIGCHLD();
					item = new_job(pid_fork, args[0], DETENIDO);
					add_job(jobs, item);
					printf("Comando %s ejecutado en primer plano con  pid %d. Estado %s. Info: %d\n", args[0], pid_fork, status_strings[status_res],info);
					unblock_SIGCHLD();
				} // fin else-if
				primerPlano = 0;
				} // fin primer if (es background)
				else {
					// Anado en el auxiliar el nuevo comando en segundo plano
					// Item es un puntero auxiliar que apunta al new_job
					item = new_job(pid_fork, args[0], SEGUNDOPLANO);
					add_job(jobs, item);
					printf("Comando %s ejecutado en segundo plano con pid %d.\n", args[0], pid_fork);
					unblock_SIGCHLD();
				}
			} // PROCESO HIJO
			else { 
				new_process_group(pid_fork);
				if(background == 0) {  set_terminal(getpid());  }
				restore_terminal_signals();
				execvp(args[0], args);
				printf("Error. Comando %s no encontrado.\n", args[0]);
				exit(-1);
			}
		}
			} // end while
} // end main



