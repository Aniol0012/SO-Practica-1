/* -----------------------------------------------------------------------
 PRA1: EPSlotto
 Codi font: loteria.c
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

#define N 5 // Número de fills (pot ser modificat, i alterarà el nombre de xifres)

void ctrlc ();
void ctrl4 ();

// Declarem unes variables globals per a poder-les utilitzar en totes les funcions

char outputmsg [150];
int pid[50]; // Array dels pid dels N fills
int p1[N][2]; // Creem un array bidimensional pel pipe 1 de N fills i 2 posicions, una per escriptura i una altra per lecutra
int p2[N][2]; // Creem un array bidimensional pel pipe 2 de N fills i 2 posicions, una per escriptura i una altra per lecutra
int llavor;

int main(int n_args, char *param_llavor[]) {

    sprintf(outputmsg, "Pressiona Ctrl + 4 per a començar el programa i Ctrl + C per acabar-lo: \n");

    write(1, outputmsg, strlen(outputmsg));
    
    /* Agafem el paràmetre que li passem al main() en l'execució abans del fork() (per a que es comparteixin les variables) 
    i el transformem a enter (ja que ha d'entrar com a char) per a usar-lo pel tractament de nombres aleatoris */
    
    llavor = atoi(param_llavor[1]);
    srand(llavor); // Inicialitzem la llavor per a utilitzar el rand()

    // Creem un bucle per a crear els N fills

    for (int i=0; i < N; i++) {
        // Creem els dos pipes, en total en crearem 2N
        pipe(p1[i]);
        pipe(p2[i]);

        switch (pid[i] = fork()) {
            case -1:
                perror("Ha sorgit un error en crear el fill\n");
                exit(-1);
            
            case 0: // En cas que el procés fill és creo correctament
                close(0); // Tanquem l'entrada estàndard per teclat
                dup(p1[i][0]); // Posem l'entrada per teclat en el pipe 1
                close(1); // Tanquem la sortida estàndard (terminal)
                dup(p2[i][1]); // Posem la sortida de la terminal en el pipe 2
                close(p1[i][0]); // Tanquem totes les entrades i sortides dels pipes
                close(p1[i][1]);
                close(p2[i][0]);
                close(p2[i][1]);

                execl("./loteria_2", "loteria_2", NULL); // Fem el recobriment i executem l'executable de loteria_2.c, és a dir, executem loteria_2
                perror("No s'ha pogut fer el recobriment de loteria_2\n");
                exit(-1); // Retorna error si no s'ha pogut fer el recobriment
        }
    }

    // Un cop creats els fills creem un bucle infinit per tal d'estar esperant que es pressioni Ctrl + 4 o Ctrl + C

    while (1) {
    	if (signal(SIGINT, ctrlc) == SIG_ERR) { // Ctrl + C
            perror("No s'ha pogut enviar el senyal SIGINT\n");
        }
        
        if (signal(SIGQUIT, ctrl4) == SIG_ERR){ // Ctrl + 4
            perror("No s'ha pogut enviar el senyal SIGQUIT\n");
        }

		// Durant l'execució mentre que el programa hagi enviat cap senyal, el programa roman a l'espera d'una d'elles
        pause();
    }
}

void ctrlc () { // Tractament del senyal del Pare per a Ctrl + C
	
	for (int i = 0; i < N; i++) {		
		kill(pid[i],SIGTERM); // Enviem un senyal per a acabar l'execució per a cada fill
		wait(NULL); // Esperem als N fills
	}
	
	sprintf(outputmsg,"Pare > Adeu\n"); 
	write(1,outputmsg, strlen(outputmsg));
	exit(0);
}

void ctrl4 () { // Tractament del senyal del Pare per a Ctrl + 4
        
	int array_aleatoris [N]; // Array de nombres aleatoris
    int num_llegit [N];
    
	sprintf(outputmsg,"Pare > Número premiat: ");
	write(1,outputmsg,strlen(outputmsg));

    for (int i = 0; i < N; i++) {
		array_aleatoris[i] = rand(); // Creem un nombre aleatori per a cada posicio del array
	    write(p1[i][1], &array_aleatoris[i], sizeof(int)); // Escrivim al pipe 1 el nombre aleatori en la posició i del array
	    read(p2[i][0], &num_llegit[i], sizeof(int)); // Llegim del pipe 2 a la posició i del array
        sprintf(outputmsg,"%i", num_llegit[i]);
        write(1,outputmsg,strlen(outputmsg));
    }
    
	write(1,"\n", sizeof(char));
}
