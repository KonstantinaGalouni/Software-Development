all:	threadpool.o	communities.o	CPM.o	GN.o

CPM.o:	CPM.c	../graph/graph.o
	gcc	-c	-g3	CPM.c
	
GN.o:	GN.c	../graph/graph.o
	gcc	-c	-g3	GN.c

communities.o:	communities.c	../graph/graph.o
	gcc	-c	-g3	communities.c	-lpthread

threadpool.o:	threadpool.c
	gcc	-c	-g3	threadpool.c	-lpthread

clean:
	rm	-rf	*o
