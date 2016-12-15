subscriber:
	make -f Makefile.subscriber

server:
	make -f Makefile.server

all:
	make Makefile.subscriber
	make Makefile.server

clean_client:
	make clean -f Makefile.subscriber

clean_server:
	make clean -f Makefile.server

cleanall:
	make clean -f Makefile.subscriber
	make clean -f Makefile.server
