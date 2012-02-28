/* 
 * File:   entree.h
 * Author: troll
 *
 * Created on February 13, 2012, 4:53 PM
 */

#ifndef ENTREE_H
#define	ENTREE_H

//* systèmes ... 
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/shm.h>
#include <semaphore.h>

//*custom ... 
#include "Config.h"
#include "Outils.h"

#ifdef MAP
#include <fstream>
#endif


void entree(int porte_num);


#endif	/* ENTREE_H */

