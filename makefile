
GPP = g++
GCC = gcc
PathFinder_OUTFILE = "./PathFinder.so"

COMPILE_FLAGS = -m32 -fPIC -c -w -DLINUX -D PROJECT_NAME=\"PathFinder\"

PathFinder = -D PathFinder $(COMPILE_FLAGS)

all: PathFinder

clean:
	-rm -f *~ *.o *.so

PathFinder: clean
	$(GCC) $(PathFinder) ./SDK/amx/*.c
	$(GPP) $(PathFinder) ./SDK/*.cpp
	$(GPP) $(PathFinder) ./MapAndreas/*.cpp
	$(GPP) $(PathFinder) ./*.cpp
	$(GPP) -m32 -fshort-wchar -pthread -shared -export-dynamic -o $(PathFinder_OUTFILE) *.o
	echo "Done"

