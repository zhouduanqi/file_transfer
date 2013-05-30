# This is my first edit!! - oliver was here

CC=gcc
CFLAGS=-O2 -march=native -pipe
#CFLAGS=
PROG_NAME=fileclient fileserver
INCS=common/common.h
SRCS=client2.c server2.c
OBJS=${SRCS:.c=.o}

# libs
LIBS=

# _________________________________________________

all:${PROG_NAME}

fileclient:client2.o common/common.o
	${CC} -o $@ $^ ${CFLAGS}

fileserver:server2.o common/common.o
	${CC} -o $@ $^ ${CFLAGS}

# 如果有头文件进行修改，则自动编译源文件
${OBJS}:${INCS}

.c.o:
	${CC} -c $< -Icommon ${CFLAGS}

common/common.o:common/common.c common/common.h
	cd common && ${CC} -c common.c ${CFLAGS}

clean:
	rm -f *.o ${PROG_NAME} common/*.o

rebuild:clean all 
