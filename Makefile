SRC:=src/*.c
INCLUDE:=include/
CFLAGS:=-g -Wall -pedantic
LFLAGS:=
BUILD:=build
OBJ:=obj

TEST_SRC:=src/test/main.c

ifeq ($(shell uname), Linux)
TARGET_TEST_EXT = 
else
TARGET_TEST_EXT = .exe
endif

all: clean test

clean:
	rm -f $(BUILD)/*
	rm -f $(OBJ)/*

liblist:
	gcc -c $(CFLAGS) -I$(INCLUDE) $(SRC)
	mv *.o $(OBJ)/
	ar rcs $(BUILD)/$@.a $(OBJ)/*.o
	
test: liblist
	gcc $(TEST_SRC) $(CFLAGS) -I$(INCLUDE) -o $(BUILD)/$@$(TARGET_TEST_EXT) $(LFLAGS) $(BUILD)/liblist.a
	$(BUILD)/$@$(TARGET_TEST_EXT)