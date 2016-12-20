all:
	make -f Makefile.client
	make -f Makefile.server

client:
	make -f Makefile.client

server:
	make -f Makefile.server

clean_client:
	make clean -f Makefile.client

clean_server:
	make clean -f Makefile.server

clean:
	make clean -f Makefile.client
	make clean -f Makefile.server
