PRG=gnu.exe
OSTYPE := $(shell uname)
ifeq ($(OSTYPE),Linux)
CYGWIN=
else
CYGWIN=-Wl,--enable-auto-import
endif

GCC=g++
GCCFLAGS=-O2 -Wall -Wextra -std=c++03 -pedantic -Wold-style-cast -Woverloaded-virtual -Wsign-promo  -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder
DEFINE=

VALGRIND_OPTIONS=-q --leak-check=full
DIFF_OPTIONS=-y --strip-trailing-cr --suppress-common-lines

OBJECTS0=
DRIVER1=main_part1.cpp
DRIVER2=main_part2.cpp

part1:
	g++  $(DRIVER1) $(OBJECTS0) $(GCCFLAGS) $(DEFINE) -lpthread -o $(PRG)
part2:
	g++  $(DRIVER2) $(OBJECTS0) $(GCCFLAGS) $(DEFINE) -lpthread -o $@.exe
0 1:
	@echo "should run in less than 2000 ms"
	./$(PRG) init$@ 10 final$@ &>/dev/null
	sleep 1
	draw final$@ > student-picture$@
	@echo "lines after the next are mismatches with master output -- see out$@"
	diff student-picture$@ picture$@ $(DIFF_OPTIONS)
clean:
	rm -f *.exe *.o *.obj
