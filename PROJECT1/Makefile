VPATH=SOURCES

main.out : main.c queue.c i2c_light.c i2c_temp.c log.c decision.c i2c.c temp.c timer.c light.c thread.c led.c
	gcc $^ -I INCLUDES -o $@ -lrt -lpthread -g
