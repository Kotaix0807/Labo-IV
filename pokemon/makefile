CC := gcc
CFLAGS := -g -Wall -Wextra -Werror -Iinclude
NO_ERROS := -g -Wall -Wextra -Iinclude

# Binarios
BATTLE_BIN := battle
ADMIN_BIN := admin
SRC_BATTLE := main.c $(wildcard src/*.c)
SRC_ADMIN := admin.c src/structs.c src/tools.c
SRC_TEST := test.c

LDLIBS_BATTLE := -lm -lncursesw -lSDL2 -lSDL2_mixer
LDLIBS_ADMIN := -lm -lncursesw

CLEAR_SCREEN := printf '\033c'

VALGRIND_FLAGS := --leak-check=full --show-leak-kinds=all --track-origins=yes
VALGRIND := valgrind $(VALGRIND_FLAGS)

VALGRIND_FREE_FLAGS := valgrind --leak-check=full --show-leak-kinds=definite
VALGRIND_FREE := valgrind $(VALGRIND_FREE_FLAGS)

.PHONY: build run run_admin leaks leak free test clean

build: $(BATTLE_BIN) $(ADMIN_BIN)

$(BATTLE_BIN): $(SRC_BATTLE)
	$(CLEAR_SCREEN) && $(CC) $(CFLAGS) -o $@ $^ $(LDLIBS_BATTLE)

$(ADMIN_BIN): $(SRC_ADMIN)
	$(CLEAR_SCREEN) && $(CC) $(CFLAGS) -o $@ $^ $(LDLIBS_ADMIN)

run: $(BATTLE_BIN)
	$(CLEAR_SCREEN)
	./$(BATTLE_BIN)

run_admin: $(ADMIN_BIN)
	$(CLEAR_SCREEN)
	./$(ADMIN_BIN)

leaks: $(BATTLE_BIN)
	$(CLEAR_SCREEN)
	$(VALGRIND) ./$(BATTLE_BIN)

leak: $(BATTLE_BIN)
	$(CLEAR_SCREEN)
	$(VALGRIND_FREE) ./$(BATTLE_BIN)

free: $(BATTLE_BIN)
	$(CLEAR_SCREEN) && $(CC) $(NO_ERROS) -o $(BATTLE_BIN) $(SRC_BATTLE) $(LDLIBS_BATTLE)
	./$(BATTLE_BIN)

test:
	$(CLEAR_SCREEN) && $(CC) $(CFLAGS) -o $(BATTLE_BIN) $(SRC_TEST) $(LDLIBS_BATTLE)
	./$(BATTLE_BIN)

clean:
	$(RM) $(BATTLE_BIN) $(ADMIN_BIN)
