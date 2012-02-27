/* 
 * File:   main.h
 * Author: fhyren
 *
 * Created on February 27, 2012, 11:06 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#define CLEF_COMPTEUR 11111
#define CLEF_REQUETES 22222

/*-------------------------------------------- Includes systemes ----------------------------------------------- */

#include <cstdlib>
#include <time.h>
#include <unistd.h> 
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>


/*----------------------------------------- Includes non systemes ---------------------------------------------- */

#include "Heure.h"
#include "Menu.h"
#include "Outils.h"
#include "keyboard.h"
#include "sortie.h"
#include "Config.h"

int main(int argc, char** argv);

typedef struct r
{
    char type;
    int arrivee;
} requete ; 

#endif	/* MAIN_H */

