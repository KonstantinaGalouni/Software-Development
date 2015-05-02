all:	main_functions.o	main1.o	main2.o

main_functions.o:	main_functions.c	../graph/graph.h	
	gcc	-c	-g3	main_functions.c

main1.o:	main1.c	../graph/graph.o	main_functions.o
	gcc	-c	-g3	main1.c

main2.o:	main2.c	../graph/graph.o	main_functions.o
	gcc	-c	-g3	main2.c

clean:
	rm	-rf	*o
