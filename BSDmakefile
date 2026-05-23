CC=		cc
CFLAGS=	-std=gnu23 -O2 -Wall -MMD -MP

SRC_DIR=	src
SRCS!=		find ${SRC_DIR} -name '*.c'
OBJS=		${SRCS:.c=.o}

TARGET=	shio

# Include dependency files if they exist
.for obj in ${OBJS}
-include ${obj:.o=.d}
.endfor

all: ${TARGET}

${TARGET}: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o ${TARGET}

${SRC_DIR}/%.o: ${SRC_DIR}/%.c
	${CC} ${CFLAGS} -c ${.IMPSRC} -o ${.TARGET}

clean:
	rm -f ${SRC_DIR}/*.o ${SRC_DIR}/*.d ${TARGET}

.PHONY: all clean
