UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	CFLAGS := -D_XOPEN_SOURCE -Wno-deprecated-declarations -g
endif

CC := gcc
BUILD=bin

all: clean example audiencia afinador solo

clean:
	$(RM) *.o
	$(RM) *.so
	$(RM) -r bin/
	mkdir -p bin


example:
	$(CC) src/example_program.c -g -L"/home/utnso/workspace/tp-2019-2c-No-C-Nada/biblioSuse/Debug" -L"/home/utnso/workspace/tp-2019-2c-No-C-Nada/hilolay/bin" -Wall $(CFLAGS) -o $(BUILD)/example -lbiblioSuse -lhilolay
	
	
audiencia:
	$(CC) src/audiencia.c -g -L"/home/utnso/workspace/tp-2019-2c-No-C-Nada/biblioSuse/Debug" -L"/home/utnso/workspace/tp-2019-2c-No-C-Nada/hilolay/bin" -Wall $(CFLAGS) -o $(BUILD)/audiencia -lbiblioSuse -lhilolay
	

afinador:
	$(CC) src/caballeros_de_SisOp_Afinador.c -g -L"/home/utnso/workspace/tp-2019-2c-No-C-Nada/biblioSuse/Debug" -L"/home/utnso/workspace/tp-2019-2c-No-C-Nada/hilolay/bin" -Wall $(CFLAGS) -o $(BUILD)/afinador -lbiblioSuse -lhilolay
	

solo:
	$(CC) src/caballeros_de_SisOp_Solo.c -g -L"/home/utnso/workspace/tp-2019-2c-No-C-Nada/biblioSuse/Debug" -L"/home/utnso/workspace/tp-2019-2c-No-C-Nada/hilolay/bin" -Wall $(CFLAGS) -o $(BUILD)/solo -lbiblioSuse -lhilolay
