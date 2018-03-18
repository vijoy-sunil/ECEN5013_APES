# Makefile for all the files
# AUTHOR : PRAVEEN GNANASEKARAN
# DATE : 03/03/2018 

main.elf:main.c LightTask.o TemperatureTask.o notify.o LoggerTask.o errorhandling.o socketTask.o  i2c_wrap.o tmp102.o adps9301.o
	gcc -g -o $@ $^ -lpthread -lrt -lm

notify.o:notify.c
	gcc -c -o $@ $<

LoggerTask.o:LoggerTask.c msgque.h includes.h
	gcc -c -o $@ $<  -lpthread

LightTask.o:LightTask.c msgque.h includes.h
	gcc -c -o $@ $<  -lpthread -lrt

TemperatureTask.o:TemperatureTask.c msgque.h includes.h
	gcc -c -o $@ $<  -lpthread -lrt

errorhandling.o:errorhandling.c msgque.h includes.h
	gcc -c -o $@ $<  -lpthread -lrt

socketTask.o:socketTask.c includes.h
	gcc -c -o $@ $<  -lpthread

tmp102.o:tmp102.c i2c_wrap.c
	gcc -c -o $@ $<

adps9301.o:adps9301.c i2c_wrap.o
	gcc -c -o $@ $< -lm

i2c_wrap.o:i2c_wrap.c
	gcc -c -o $@ $<

clean:
	rm -f *.o *.elf logfile.txt ./remote_request/*.o ./tests_cmocka/*.o
	rm -f ./remote_request/*.elf ./tests_cmocka/tests
