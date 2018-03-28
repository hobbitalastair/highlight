PREFIX := ${DESTDIR}/usr
BINDIR := ${PREFIX}/bin
LIBS =
CC = gcc
CFLAGS = -Wall -Werror -O2 -g
BIN = lex-c format.py
OUT = lex-c.html lex-c.term

all: $(BIN) $(OUT)

%: %.c
	$(CC) -o $@ $< $(LIBS) $(CFLAGS)

%.html: % format.py
	./format.py $(patsubst $<.%,%,$@) ./$< $<.c > $@
%.term: % format.py
	./format.py $(patsubst $<.%,%,$@) ./$< $<.c > $@

install: $(BIN)
	mkdir -p "${BINDIR}/"
	for bin in $(BIN); do \
	    install -m755 "$$bin" "${BINDIR}/"; \
	done

clean:
	rm -f $(OBJS)

