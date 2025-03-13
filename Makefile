#obj-m += src/hello/main.o
obj-m += src/simple-char/simple-char.o
obj-m += src/hello/hello.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean	
sync:
	rsync -avz  --exclude=.git -e "ssh -p 3022" ./  root@192.168.0.107:~/driver-dev
	#ssh -p 3022 root@192.168.0.107 3022 "cd driver-dev && make"
test:
	insmod src/hello/hello.ko
	dmesg |grep hello
	rmmod hello
	dmesg |grep hello
	
	insmod src/simple-char/simple-char.ko
	dmesg |grep hello
	rmmod simple-char
	dmesg |grep hello
	