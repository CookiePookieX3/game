lExtern: ../externCompiled/extern.cpp
	g++ -std=c++17 -O3 -c extern.cpp -o extern.o -I..
	ar rcs libextern.a extern.o
	rm -f extern.o

clean:
	rm -f libextern.a
