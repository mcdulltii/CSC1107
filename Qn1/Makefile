CC=gcc
FPRE=CSC1107_7_
FKERNEL=$(FPRE)kernel
FNAME=$(FPRE)user
obj-m += $(FKERNEL).o

all: build

%: %.c
	$(CC) $< -o $@ -lssl -lcrypto

build: $(FNAME)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	$(RM) $(FNAME)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
