obj-m += dt_gpio.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
dt: testoverlay.dts
	dtc -@ -I dts -O dtb -o testoverlay.dtbo testoverlay.dts
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
