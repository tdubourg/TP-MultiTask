#include "keyboard.h"

using namespace std;
void keyboard() {
    Menu();
}

void Commande(char code, unsigned int valeur) {
    switch(code) {
	case 'E':
	    exit(EXIT_CODE);
	    break;
	
	case 'P':
	    //* valeur = n° de la porte (0,1,2)
	    break;
	    
	case 'A':
	    //* valeur = n° de la porte (0,1,2)
	    break;
	    
	case 'S':
	    //* valeur = n° de la porte (0,1,2)
	    break;
    }
}
