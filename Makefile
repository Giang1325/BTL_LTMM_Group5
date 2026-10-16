# Makefile khung - Nhom 5, BTL Ly thuyet mat ma (Trivium)
# Cach dung:
#   make        -> build chuong trinh
#   make test   -> chay kiem thu
#   make clean  -> xoa cac file da build
# Tren Windows (MinGW) go mingw32-make thay cho make.

CC     = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2

# Nhan biet he dieu hanh de dung dung lenh xoa file
ifeq ($(OS),Windows_NT)
    EXE = .exe
    RM  = del /Q /F
else
    EXE =
    RM  = rm -f
endif

.PHONY: all test clean

all:
	@echo Chua co ma nguon de build - se bo sung o tuan 2-3

test:
	@echo Chua co test - se bo sung o tuan 2-3

clean:
	-$(RM) *.o trivium$(EXE) test_vectors$(EXE)