FORMATTER = clang-format-3.8
FORMATTER_ARGS = -i -style=file

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

install:
	@sudo apt-get install libglib2.0-dev
	@sudo apt-get install clang-format-3.8
	@sudo apt-get install clang-format-3.7

formatsource:
	$(FORMATTER) $(FORMATTER_ARGS) src/*.c
	$(FORMATTER) $(FORMATTER_ARGS) include/*.h
