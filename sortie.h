/* 
 * File:   sortie.h
 * Author: fhyren
 *
 * Created on February 27, 2012, 10:21 PM
 */

#ifndef SORTIE_H
#define	SORTIE_H

#include <signal.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "Outils.h"
#include "Config.h"
#include "affichageSortie.h"

void Sortie();

#endif	/* SORTIE_H */

