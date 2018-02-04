
# Automatically generated Makefile
# Makefile-generator programmed by Hans de Nivelle, 2002


Flags = -Wreturn-type -pedantic -pedantic-errors -Wundef -std=c++11
CPP = g++


main : Makefile   term.o fol.o examples.o main.o 
	$(CPP) $(Flags) -o main   term.o fol.o examples.o main.o 

term.o : Makefile   term.cpp   term.h ../shared/shared_vector.h 
	$(CPP) -c $(Flags) term.cpp -o  term.o


fol.o : Makefile   fol.cpp   fol.h term.h ../shared/shared_vector.h nondet.h 
	$(CPP) -c $(Flags) fol.cpp -o  fol.o


examples.o : Makefile   examples.cpp   examples.h nondet.h fol.h term.h ../shared/shared_vector.h 
	$(CPP) -c $(Flags) examples.cpp -o  examples.o


main.o : Makefile   main.cpp   examples.h nondet.h fol.h term.h ../shared/shared_vector.h 
	$(CPP) -c $(Flags) main.cpp -o  main.o


