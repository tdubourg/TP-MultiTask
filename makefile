COMP=g++
EXE=Parking
EFFACE=clean
INT=affichageSortie.h entree.h keyboard.h main.h sortie.h
OBJ=$(INT:.h=.o)
LIBS= -lpthread -lncurses -ltcl
CUSTOM_LIBS=/public/tp/tp-multitache/libtp.a
ECHO=echo
RM=rm
RMFLAGS=-f

%.o: %.cpp
#	$(ECHO) compilation de $<
	$(COMP) -c $< -o $@

$(EXE): $(OBJ)
	$(ECHO) $(EXE)
	$(COMP) -o"$(EXE)" $(OBJ) $(CUSTOM_LIBS) $(LIBS)
	
$(EFFACE):
	$(RM) $(RMFLAGS) $(EXE) $(OBJ)

	
