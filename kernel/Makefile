obj-m+=indicator.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
	c++ indicator_data_provider.cpp -o indicator_data_provider
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean make -C 
