# This is my first edit!! - oliver was here

CC=gcc
PROG_NAME=fileclient fileserver    
# ELF文件
INCS=common/common.h
SRCS=client2.c server2.c    
# C文件

# 从 .c 文件到 .o 文件
OBJS=${SRCS:.c=.o}

# libs
LIBS=
# ---- 用户修改区域结束

# _________________________________________________

all:${PROG_NAME}

fileclient:client2.o common/common.o
	${CC} -o $@ $^

fileserver:server2.o common/common.o
	${CC} -o $@ $^

# 如果有头文件进行修改，则自动编译源文件
${OBJS}:${INCS}

.c.o:
	${CC} -c $< -Icommon

common/common.o:common/common.c common/common.h
	cd common && ${CC} -c common.c

clean:
	rm -f *.o ${PROG_NAME} common/*.o

rebuild:clean all 
