UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	CFLAGS := -D_XOPEN_SOURCE -Wno-deprecated-declarations -g
endif

CC := gcc
BUILD=bin

all: clean pruebaMuse recursiva supermasivo

clean:
	$(RM) *.o
	$(RM) *.so
	$(RM) -r bin/
	mkdir -p bin

pruebaMuse:
	$(CC) -I/home/utnso/workspace/tp-2019-2c-No-C-Nada/libMUSE/libMUSE -I/home/utnso/workspace/tp-2019-2c-No-C-Nada/biblioNOC -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF$(BUILD)/pruebaMuse.d -MT"src/pruebaMuse.o" -o "$(BUILD)/pruebaMuse.o" "src/pruebaMuse.c"
	$(CC) -L/home/utnso/workspace/tp-2019-2c-No-C-Nada/libMUSE/Debug -o "$(BUILD)/pruebaMuse"  $(BUILD)/pruebaMuse.o -llibMUSE


recursiva:
	$(CC) -I/home/utnso/workspace/tp-2019-2c-No-C-Nada/libMUSE/libMUSE -I/home/utnso/workspace/tp-2019-2c-No-C-Nada/biblioNOC -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF$(BUILD)/recursiva.d -MT"src/recursiva.o" -o "$(BUILD)/recursiva.o" "src/recursiva.c"
	$(CC) -L/home/utnso/workspace/tp-2019-2c-No-C-Nada/libMUSE/Debug -o "$(BUILD)/recursiva"  $(BUILD)/recursiva.o -llibMUSE
	
supermasivo:
	$(CC) -I/home/utnso/workspace/tp-2019-2c-No-C-Nada/libMUSE/libMUSE -I/home/utnso/workspace/tp-2019-2c-No-C-Nada/biblioNOC -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF$(BUILD)/supermasivo.d -MT"src/supermasivo.o" -o "$(BUILD)/supermasivo.o" "src/archivo_de_swap_supermasivo.c"
	$(CC) -L/home/utnso/workspace/tp-2019-2c-No-C-Nada/libMUSE/Debug -o "$(BUILD)/supermasivo"  $(BUILD)/supermasivo.o -llibMUSE