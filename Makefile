
MCU = atmega8
CC  = avr-

LFUSE = 0xE4
HFUSE = 0xD9

INC = 	-Iapp \
		-Iradio \
		-Iradio/inc \
		-Ispi \
		-Iuart \
		-Iadc

VPATH = app:radio:spi:uart:adc

OBJS  = main.o spi.o radio.o uart.o adc.o

OUTPUT = app.elf
HEXFILE = app.hex

CFLAGS = -g -c -Wall -Os -mmcu=$(MCU) $(INC) -std=c99
LFLAGS = -g -Wall -Os -mmcu=$(MCU)

all: $(OBJS)
	$(CC)gcc $(LFLAGS) $(OBJS) -o $(OUTPUT)
	$(CC)size $(OUTPUT)
	$(CC)objcopy -O ihex $(OUTPUT) $(HEXFILE)

clean: $(OBJS)
	-rm $(OBJS)

load: $(ELFFILE)
	avrdude -c avrispmkII -p $(MCU) -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U flash:w:$(HEXFILE)

main.o: main.c
	$(CC)gcc $(CFLAGS) $^ -o $@
		
spi.o: spi.c
	$(CC)gcc $(CFLAGS) $^ -o $@

radio.o: radio.c
	$(CC)gcc $(CFLAGS) $^ -o $@

uart.o: uart.c
	$(CC)gcc $(CFLAGS) $^ -o $@

adc.o: adc.c
	$(CC)gcc $(CFLAGS) $^ -o $@ 