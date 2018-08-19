obj-m+=hello.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
	c++ test_indicator.cpp -o test
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean make -C 
