/* -----------------------------------------------------------------------
 PRA1: EPSlotto
 Codi font: loteria_2.c
 Aleix Segura Paz
 Aniol Serrano Ortega
 ---------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>

int main(int n_args, char *param_llavor[]) {    
	// Ignorem els senyals de teclat (Ctrl + 4 i Ctrl + C) en el fill, ja que només són útils per al pare
	// Acabem el procés en -1 en cas que no es pugui ignorar el senyal.
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR) {
    	perror("No s'ha pogut ignorar SIGQUIT\n");
        exit(-1);
    }

    if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
        perror("No s'ha pogut ignorar SIGINT\n");
        exit(-1);
    }

    if (signal(SIGTERM, exit) == SIG_ERR) {
        perror("No s'ha pogut ignorar SIGTERM\n");
        exit(-1);
    }
    
    // Un cop ignorades les senyals en el fill, ja podem crear un bucle infinit per a generar un nombre aleatori per a cada cop que es pressioni Ctrl + 4       
    while (1) {
        int llavor; 	
        read(0, &llavor, sizeof (int)); // Llegim de l'entrada de teclat del pipe
        srand(llavor); // Definim la llavor per al nombre aleatori
        int nombre_aleatori = rand() % 10; // Un cop definida la llavor creem el numero aleatori d'entre 0 i 9
        write(1, &nombre_aleatori, sizeof (int)); // Escrivim el nombre aleatori al pipe
    }
}
