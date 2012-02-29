/* 
 * File:   keyboard.h
 * Author: troll
 *
 * Created on February 13, 2012, 4:53 PM
 */

#ifndef KEYBOARD_H
#define	KEYBOARD_H

#include <stdlib.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>


#include "Menu.h"
#include "Config.h"

void keyboard();
void Commande(char code, unsigned int valeur);

#endif	/* KEYBOARD_H */

