subscriber:
	make -f Makefile.subscriber

publisher:
	make -f Makefile.publisher

server:
	make -f Makefile.server

all:
	make -f Makefile.subscriber
	make -f Makefile.publisher
	make -f Makefile.server

clean_subscriber:
	make clean -f Makefile.subscriber
	
clean_publisher:
	make clean -f Makefile.publisher

clean_server:
	make clean -f Makefile.server

cleanall:
	make clean -f Makefile.subscriber
	make clean -f Makefile.publisher
	make clean -f Makefile.server
