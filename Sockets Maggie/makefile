RM=rm -rf
CC=gcc

C_SRCS=$(shell find . -iname "*.c" | tr '\n' ' ')
H_SRCS=$(shell find . -iname "*.h" | tr '\n' ' ')
OBJS=$(C_SRCS:./%.c=%.o)

# Clean and compile .so
all: build/libtheDebuggers.so

build/libtheDebuggers.so: $(OBJS)
	$(CC) -shared -o "libtheDebuggers.so" $(OBJS)

%.o: ./%.c
	$(CC) -c -fmessage-length=0 -fPIC -MMD -W -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -W -o "$@" "$<" -lcommons

clean:
	$(RM) ./*.o
	$(RM) ./*.d
	$(RM) ./libtheDebuggers.so
	

install: all
	cp -u ./libtheDebuggers.so /usr/lib
	mkdir -p /usr/include/theDebuggers/
	cp -u $(H_SRCS) /usr/include/theDebuggers/

uninstall: clean
	rm -f /usr/lib/libtheDebuggers.so
	rm -rf /usr/include/theDebuggers

.PHONY: all clean install uninstall
