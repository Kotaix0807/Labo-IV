CC := gcc
CFLAGS := -g -Wall -Wextra -Werror 
NO_ERROS := -g -Wall -Wextra
BIN := battle
SRC := player.c
ADMIN_BIN := admin
ADMIN_SRC := admin.c
SRC_TEST := test.c
C_LDLIBS := -lm -lncursesw -lSDL2 -lSDL2_mixer
CLEAR_SCREEN := printf '\033c'

VALGRIND_FLAGS := --leak-check=full --show-leak-kinds=all --track-origins=yes
VALGRIND := valgrind $(VALGRIND_FLAGS)

VALGRIND_FREE_FLAGS := valgrind --leak-check=full --show-leak-kinds=definite
VALGRIND_FREE := valgrind $(VALGRIND_FREE_FLAGS)

.PHONY: build run leaks leak free test clean admin

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

test:
	$(CLEAR_SCREEN) && $(CC) $(CFLAGS) -o $(BIN) $(SRC_TEST) $(C_LDLIBS)
	./$(BIN)

clean:
	$(RM) $(BIN)

admin: $(ADMIN_SRC)
	$(CC) $(CFLAGS) -o $(ADMIN_BIN) $(ADMIN_SRC)
