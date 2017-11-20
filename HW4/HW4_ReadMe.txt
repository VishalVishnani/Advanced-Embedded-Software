SIMPLE CHARACTER DRIVER

Steps to run the character driver (for successfully running)- 
1. Unbind the existing led driver on Beaglebone green, do this as root - echo leds > /sys/class/leds/beaglebone\:green\:usr0/device/driver/unbind
2. Refer to the Makefile uploaded, do sudo make all, to compile the LKM for character driver
3. Insert the kernel module - insmod ./example.ko
4. Run command sudo mknod -m 666 /dev/simple_char_device c 240 0
5. Run the server side socket application beaglebone_server.c, which reads and writes to the driver. 
gcc beaglebone_server.c -o server.out
./server.out [port number]

6. Run the client side socket application client_ubuntu.c, which sends commands to server side.
gcc client_ubuntu.c -o client.out
./client.out 192.168.7.2 [port number]



