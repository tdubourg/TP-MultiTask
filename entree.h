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
#include <signal.h>
#include <vector>
#include <sys/wait.h>

//*custom ... 
#include "Config.h"
#include "Outils.h"

typedef std::vector<pid_t> pidvect;

#ifdef MAP
#include <fstream>
#endif


void entree(int porte_num);


#endif	/* ENTREE_H */

