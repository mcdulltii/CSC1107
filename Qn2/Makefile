FNAME=Q2_Group_7
CC=gcc

.PHONY: build run clean

all: build

%: %.c
	$(CC) $< -o $@

build: $(FNAME)

run: build
	./$(FNAME)

clean:
	$(RM) $(FNAME)

