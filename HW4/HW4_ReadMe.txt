SIMPLE CHARACTER DRIVER

Steps to run 
1. Unbind the existing led driver on BBG as root 
cd /sys/class/leds/beaglebone\:green\:usr0/device/driver/
echo leds > unbind 
2. Run sudo make all to compile character driver
3. Insert the kernel module - insmod ./example.ko
4. Run sudo mknod -m 666 /dev/simple_char_device c 240 0
5. Run the server side socket application beaglebone_server.c, which reads and writes to the driver. 
gcc beaglebone_server.c -o server.out
./server.out [port number]

6. Run the client side socket application client_ubuntu.c, which sends commands to server side.
gcc client_ubuntu.c -o client.out
./client.out 192.168.7.2 [port number]



