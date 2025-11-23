CC := gcc
CFLAGS := -g -Wall -Wextra -Werror
BIN := battle
SRC := main.c
C_LDLIBS := -lm
CLEAR_SCREEN := printf '\033c'

VALGRIND_FLAGS := --leak-check=full --show-leak-kinds=all --track-origins=yes
VALGRIND := valgrind $(VALGRIND_FLAGS)

.PHONY: build run leaks clean

build:
	$(CLEAR_SCREEN) && $(CC) $(CFLAGS) -o $(BIN) $(SRC) $(C_LDLIBS)

run: build
	$(CLEAR_SCREEN)
	./$(BIN)

leaks: build
	$(CLEAR_SCREEN)
	$(VALGRIND) ./$(BIN)

clean:
	$(RM) $(BIN)
