all:	
	cd	graph;	$(MAKE)
	cd	main;	$(MAKE)
	cd	queue;	$(MAKE)
	gcc	-o	main1	-g3	./graph/graph.o	./main/main_functions.o	./queue/queue.o	./main/main1.o
	gcc	-o	main2	-g3	./graph/graph.o	./main/main_functions.o	./queue/queue.o	./main/main2.o	
	gcc	-o	main3	-g3	./graph/graph.o	./main/main_functions.o	./queue/queue.o	./main/main3.o		
	@echo "\nPress ./main1 or ./main2 or ./main3\n"

main.o:	./main/main.c	./graph/graph.o	./main/main_functions.o
	gcc	-c	-g3	./main/main.c

clean:
	cd graph; 	$(MAKE) clean
	cd main;	$(MAKE) clean
	cd queue; 	$(MAKE) clean
	rm	-rf	*o
	rm	-rf main1
	rm	-rf main2
	rm	-rf main3
	
