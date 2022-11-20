obj-m += Encode_Decode_Module.o
obj-m += func.o

bulid:Encode_Decode_Module Application

Application:
	gcc Application.c -o Application.bin

Encode_Decode_Module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm Application.bin