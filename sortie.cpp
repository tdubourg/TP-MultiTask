
//////////////////////////////////////////////INCLUDE
/*----------------------------------------- Includes personels ---------------------------------------------- */
#include "sortie.h"

using namespace std;

#ifdef MAP
std::ofstream f ( "debug_sortie.log", ios_base::app );
#endif

//////////////////////////////////////////////PRIVE
/*----------------------------------------- Variables statiques ---------------------------------------------- */
static int canalKeySortie = -1;
static vector<int> noSorties;

/*----------------------------------------- Fonctions privées ---------------------------------------------- */

static void FinProgramme ( int num ) {
    //Algorithme : aucun
    //
#ifdef MAP
    f << "fin programme" << endl;
#endif
    vector<int>::iterator it;
    for ( it = noSorties.begin ( ); it != noSorties.end ( ); ++it ) {
#ifdef MAP
        f << "envoie signal de kill" << endl;
#endif
        kill ( ( *it ), SIGUSR2 );
        int st = -1;
        waitpid ( ( *it ), &st, 0 );
    }

    if ( canalKeySortie != -1 ) {
        close ( canalKeySortie );
    }

    if ( num != -1 ) {
        num = 0;
    }

    exit ( num );
}//---------Fin de FinProgramme

//////////////////////////////////////////////PUBLIC

/*----------------------------------------- Fonctions publiques ---------------------------------------------- */

void Sortie ( ) {
    //Algorithme : aucun
    //

    //--------------------------------Initialisation------------------------------------

    bool error = false;

    //*Association du signal SIGUSR2 à  la fin du programme
    struct sigaction action;
    action.sa_handler = FinProgramme;
    action.sa_flags = 0;
    sigaction ( SIGUSR2, &action, NULL );

    //*Ouverture du canal de communication en lecture
    canalKeySortie = open ( CANAL_KEY_SORTIE, O_RDONLY );
    if ( canalKeySortie == -1 ) {
        error = true;
    }

    //------------------------------------Moteur--------------------------------------- 
    unsigned int place;
    int noAffSortie;

    if ( error ) {
        Afficher ( MESSAGE, "Erreur de création de tache, quittez le programme." );
        FinProgramme ( -1 );
    }

    int stCanal = -1;
    for ( ;stCanal != 0; ) { 
        stCanal = read ( canalKeySortie, &place, sizeof (unsigned int ) ); //On lis dans le canal tant qu'il y a des éléments à lire, sinon, on attend qu'il y en ai de nouveau	
        if ( stCanal != 0 && ( noAffSortie = fork ( ) ) == 0 ) {
            //Code du fils affichageSortie	
#ifdef MAP
            f << "Creation d'une tache affichage sortie" << endl;
#endif

            affichageSortie ( place );
        }
#ifdef MAP
        f << "filles de sortie :(affichage sortie) :" << noAffSortie << endl;
#endif
        if ( noAffSortie > 0 ) {
            noSorties.push_back ( noAffSortie );
        }
    }
    
    //Si jamais on arrive jusqu'ici, on quitte proprement
    FinProgramme(-1);
}//-----------Fin de Sortie
