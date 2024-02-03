SRC := $(shell find src -name "*.c")
OBJ := $(SRC:%.c=obj/%.o)
DEP := $(SRC:%.c=obj/%.d)

CFLAGS := -std=c99 -Isrc -Wall -Wextra -g
CFLAGS += -I/usr/local/include

LDFLAGS := -L/usr/local/lib -lraylib

obj/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

bin/game: $(OBJ)
	mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

clean:
	rm -rf bin obj

run: bin/game
	bin/game

all: bin/game

-include $(DEP)
