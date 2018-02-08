
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

clean:
	/bin/rm -rf tmp/* bin/*


