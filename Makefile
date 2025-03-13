obj-m += src/hello/main.o
obj-m += src/single-char-driver/main.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean	
sync:
	rsync -avz  --exclude=.git -e "ssh -p 3022" ./  root@192.168.0.107:~/driver-dev
	ssh -p 3022 root@192.168.0.107 3022 "cd driver-dev && make"
