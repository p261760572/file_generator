CC = gcc
CFLAGS = -g -Wall -pthread -I$(ORACLE_HOME)/rdbms/public -Iinclude -Iinclude/cutil -Iinclude/json-c -Iinclude/exp -Iinclude/oci -Iinclude/sql -Iinclude/libxl -I.
LDFLAGS= -L$(ORACLE_HOME)/lib -Llib -Wl,-rpath,lib
LIBS = -lsql -loci -lexp -ljson-c -lclntsh -lcutil -luuid -lxl -ldcsutil -lcurl

ALL_SRCS = main.c process_util.c file_generate.c gen_sql.c data_acl.c json_ext.c db_hander.c updown.c

ALL_OBJS = $(ALL_SRCS:.c=.o)

all: FileGenerate

FileGenerate: $(ALL_OBJS)
	$(CC) $(ALL_OBJS) -o $@ $(CFLAGS) $(LDFLAGS) $(LIBS)
	rm *.o

%o: %c
	$(CC) $(CFLAGS) -c -o $@ $<

clean: 
	rm -f *~ *.lis *.log $(ALL_OBJS)


