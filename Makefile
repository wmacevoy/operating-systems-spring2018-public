
tmp/forkexec.o : src/forkexec.c
	gcc -c -o tmp/forkexec.o src/forkexec.c

bin/forkexec : tmp/forkexec.o
	gcc -o bin/forkexec tmp/forkexec.o

tmp/integrate-st.o : src/integrate-st.c
	gcc -c -o tmp/integrate-st.o src/integrate-st.c

bin/integrate-st : tmp/integrate-st.o
	gcc -o bin/integrate-st tmp/integrate-st.o -lm

tmp/integrate-mt.o : src/integrate-mt.c
	gcc -c -o tmp/integrate-mt.o src/integrate-mt.c

bin/integrate-mt : tmp/integrate-mt.o
	gcc -o bin/integrate-mt tmp/integrate-mt.o -lm -lpthread

tmp/integrate-mt-cpp.o : src/integrate-mt-cpp.cpp
	g++ -g -std=c++17 -c -o tmp/integrate-mt-cpp.o src/integrate-mt-cpp.cpp

bin/integrate-mt-cpp : tmp/integrate-mt-cpp.o
	g++ -g -std=c++17 -o bin/integrate-mt-cpp tmp/integrate-mt-cpp.o -lm -lpthread

bin/Integrate.class : src/Integrate.java
	javac -d bin -s src src/Integrate.java

clean:
	/bin/rm -rf tmp/* bin/*


