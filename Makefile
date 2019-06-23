INSTALLDIR=/etc/zabbix/externalscripts

PKG_CONFIG=libpcre libcurl libssl libcrypto
PKG_CFLAGS=$(shell pkg-config --cflags $(PKG_CONFIG))
PKG_LDFLAGS=$(shell pkg-config --libs-only-L $(PKG_CONFIG))
PKG_LDFLAGS_STATIC=$(shell pkg-config --static --libs-only-L $(PKG_CONFIG))
PKG_LDADD=$(shell pkg-config --libs-only-l --libs-only-other $(PKG_CONFIG))
PKG_LDADD_STATIC=$(shell pkg-config --static --libs-only-l --libs-only-other $(PKG_CONFIG))

# overload this, if needed
CC = gcc

CFLAGS=	-O2 -fno-strict-aliasing -pipe \
	-W -Wall -ansi -pedantic -Wbad-function-cast -Wcast-align \
	-Wcast-qual -Wchar-subscripts -Winline \
	-Wmissing-prototypes -Wnested-externs -Wpointer-arith \
	-Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings \
	-DUSE_SSLEAY -DUSE_OPENSSL
LDFLAGS=
LDFLAGS_STATIC= -static
LDADD=
LDADD_STATIC=

OBJECTS=main.o callback.o

%.d: %.c
	@echo generating $@
	@$(CC) -MM $< | sed -e 's,:, $@:,' >$@

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(PKG_CFLAGS)

.PHONY: default static all
default: http_extend test
#static: http_extend-static
static:
	$(error static linking on SuSE is not possible due to lack of static libcurl)
all: default static

http_extend: $(OBJECTS)
	$(CC) -o $@ $^ $(LDADD) $(PKG_LDADD) $(LDFLAGS) $(PKG_LDFLAGS)

http_extend-static: $(OBJECTS)
	$(CC) -o $@ $^ $(LDADD_STATIC) $(PKG_LDADD_STATIC) $(LDFLAGS_STATIC) $(PKG_LDFLAGS_STATIC)

ifneq ($(MAKECMDGOALS),clean)
-include $(OBJECTS:.o=.d)
endif

.PHONY: install
install: http_extend
	install -s -g root -m 0755 -o root http_extend $(INSTALLDIR)/

.PHONY: clean
clean:
	rm -f http_extend http_extend-static $(OBJECTS) *~ *.d


test: http_extend
	chmod +x ./test-all.sh
	./test-all.sh
