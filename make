COMP = g++
EXE = Parking
EFFACE = clean
INT = Config.h affichageSortie.h entree.h Heure.h keyboard.h main.h Menu.h Outils.h sortie.h
OBJ = $(INT:.h=.o)
LIBS = 
ECHO = echo
RM = rm
RMFLAGS = -f

	affichageSortie.o : affichageSortie.h Outils.h Config.h
	entree.o : entree.h Outils.h Config.h
	keyboard.o : keyboard.h Menu.h Config.h
	sortie.o : sortie.h affichageSortie.h
	main.o : Heure.h keyboard.h entree.h sortie.h Config.h
	%.o: %.cpp
	$(ECHO) compilation de $<
	$(COMP) -c $<

	$(EXE) : $(OBJ)
	$(ECHO) $(EXE)
	$(COMP) -o $(EXE) $(OBJ)
	
	$(EFFACE):
	$(RM) $(RMFLAGS) $(EXE) $(OBJ)

	
