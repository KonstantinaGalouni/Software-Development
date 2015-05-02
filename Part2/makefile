all:	
	cd graph;	$(MAKE)	--no-print-directory 
	cd list;	$(MAKE)	--no-print-directory		
	cd metrics;	$(MAKE)	--no-print-directory
	cd main;	$(MAKE)	--no-print-directory
	cd queries;	$(MAKE) --no-print-directory
	
	gcc	-o	main1	-g3	./graph/graph.o	./main/main_functions.o	./list/list.o	./main/main1.o	./metrics/metrics.o	./queries/queries.o
	gcc	-o	main2	-g3	./graph/graph.o	./main/main_functions.o	./list/list.o	./main/main2.o	./metrics/metrics.o	./queries/queries.o
	
	@echo "\nPress ./main1 or ./main2 \n"

clean:
	cd graph; 	$(MAKE) clean	--no-print-directory
	cd main;	$(MAKE) clean	--no-print-directory
	cd list; 	$(MAKE) clean	--no-print-directory
	cd metrics;	$(MAKE)	clean	--no-print-directory
	cd queries; $(MAKE)	clean	--no-print-directory
	rm	-rf	*o
	rm	-rf main1
	rm	-rf main2

