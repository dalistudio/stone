.PHONY : all clean

all:	libsms.a \
	sms

libsms.a:
	gcc -g -c cssl.c -D_GNU_SOURCE
	gcc -g  -c socket_server.c unicode.c http_parser.c
	ar crv libsms.a cssl.o socket_server.o unicode.o http_parser.o
	rm *.o

sms:
	gcc -g -c main.c
	gcc -o sms main.o libsms.a
	rm *.o

clean:
	rm sms libsms.a
