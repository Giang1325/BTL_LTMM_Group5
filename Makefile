CC     = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2
INC    = -Isrc

ifeq ($(OS),Windows_NT)
    EXE = .exe
    RUN =
    CLEAN = del /Q /F
    NUL = 2>nul
else
    EXE =
    RUN = ./
    CLEAN = rm -f
    NUL =
endif

APP       = trivium$(EXE)
TEST      = test_vectors$(EXE)
TRACE     = trace$(EXE)
AVALANCHE = avalanche$(EXE)
IVREUSE   = iv_reuse$(EXE)

CORE = src/trivium.c

.PHONY: all app test trace avalanche ivreuse clean

all:
ifeq ($(wildcard src/main.c),)
	@echo Chua co src/main.c, chi bien dich thu src/trivium.c
	$(CC) $(CFLAGS) $(INC) -c $(CORE) -o trivium.o
else
	$(CC) $(CFLAGS) $(INC) -o $(APP) src/main.c src/hex.c $(CORE)
endif

app: all

# test_vectors tra ve ma loi khac 0 khi co FAIL.
test:
ifeq ($(wildcard tests/test_vectors.c),)
	@echo Chua co tests/test_vectors.c
else
	$(CC) $(CFLAGS) $(INC) -o $(TEST) tests/test_vectors.c $(CORE)
	$(RUN)$(TEST)
endif

trace:
ifeq ($(wildcard tools/trace.c),)
	@echo Chua co tools/trace.c
else
	$(CC) $(CFLAGS) $(INC) -o $(TRACE) tools/trace.c $(CORE)
	$(RUN)$(TRACE)
endif

avalanche:
ifeq ($(wildcard tools/avalanche.c),)
	@echo Chua co tools/avalanche.c
else
	$(CC) $(CFLAGS) $(INC) -o $(AVALANCHE) tools/avalanche.c $(CORE)
	$(RUN)$(AVALANCHE)
endif

ivreuse:
ifeq ($(wildcard tools/iv_reuse.c),)
	@echo Chua co tools/iv_reuse.c
else
	$(CC) $(CFLAGS) $(INC) -o $(IVREUSE) tools/iv_reuse.c $(CORE)
	$(RUN)$(IVREUSE)
endif

clean:
	-$(CLEAN) $(APP) $(TEST) $(TRACE) $(AVALANCHE) $(IVREUSE) *.o $(NUL)
