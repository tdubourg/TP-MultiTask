/* 
 * File:   affichageSortie.h
 * Author: fhyren
 *
 * Created on February 28, 2012, 9:42 PM
 */

#ifndef AFFICHAGESORTIE_H
#define	AFFICHAGESORTIE_H

#include <signal.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include "Outils.h"
#include "Config.h"

void affichageSortie(unsigned int place);

#endif	/* AFFICHAGESORTIE_H */

