COMP=g++
EXE=Parking
EFFACE=clean
INT=affichageSortie.h entree.h keyboard.h main.h sortie.h
OBJ=$(INT:.h=.o)
LIBS= -lpthread -lcurses -lncurses /usr/lib/libtcl.so
CUSTOM_LIBS=/public/tp/tp-multitache/libtp.a
ECHO=echo
RM=rm
RMFLAGS=-f

#	affichageSortie.o: affichageSortie.h Outils.h Config.h
#	entree.o: entree.h Outils.h Config.h
#	keyboard.o: keyboard.h Menu.h Config.h
#	sortie.o: sortie.h affichageSortie.h
#	main.o: Heure.h keyboard.h entree.h sortie.h Config.h
# $(EXE): $(OBJ)
#all: $(EXE)
%.o: %.cpp
#	$(ECHO) compilation de $<
	$(COMP) -c $< -o $@

$(EXE): $(OBJ)
	$(ECHO) $(EXE)
	$(COMP) -o"$(EXE)" $(OBJ) $(LIBS) $(CUSTOM_LIBS)
	
$(EFFACE):
	$(RM) $(RMFLAGS) $(EXE) $(OBJ)

	
