TARGET = 3600sh

$(TARGET): $(TARGET).c
	gcc -D_POSIX_SOURCE -std=c99 -O0 -g -lm -Wall -pedantic -Werror -Wextra -o $@ $<

all: $(TARGET)

test: all
	./test

clean:
	rm $(TARGET)

