CC := gcc
CFLAGS := -g -Wall -Wextra -Werror
NO_ERROS := -g -Wall -Wextra
BIN := battle
SRC := main.c
C_LDLIBS := -lm -lncurses
CLEAR_SCREEN := printf '\033c'

VALGRIND_FLAGS := --leak-check=full --show-leak-kinds=all --track-origins=yes
VALGRIND := valgrind $(VALGRIND_FLAGS)

VALGRIND_FREE_FLAGS := valgrind --leak-check=full --show-leak-kinds=definite
VALGRIND_FREE := valgrind $(VALGRIND_FREE_FLAGS)


.PHONY: build run leaks leak free clean

build:
	$(CLEAR_SCREEN) && $(CC) $(CFLAGS) -o $(BIN) $(SRC) $(C_LDLIBS)

run: build
	$(CLEAR_SCREEN)
	./$(BIN)

leaks: build
	$(CLEAR_SCREEN)
	$(VALGRIND) ./$(BIN)

leak: build
	$(CLEAR_SCREEN)
		$(VALGRIND_FREE) ./$(BIN)
free:
	$(CLEAR_SCREEN) && $(CC) $(NO_ERROS) -o $(BIN) $(SRC) $(C_LDLIBS)
	./$(BIN)

clean:
	$(RM) $(BIN)
