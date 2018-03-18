tests: tests.c tmp102.o adps9301.o i2c_wrap.o client_test.o
	gcc -o $@ $^ -lcmocka -lm -lrt -pthread

tmp102.o:../sensors/tmp102.c ../sensors/i2c_wrap.c
	gcc -c -o $@ $< -lcmocka

adps9301.o:../sensors/adps9301.c ../sensors/i2c_wrap.o
	gcc -c -o $@ $< -lcmocka -lm

i2c_wrap.o:../sensors/i2c_wrap.c
	gcc -c -o $@ $< -lcmocka

client_test.o:../socket_client/client_test.c
	gcc -c -o client_test.o ../socket_client/client_test.c -lrt -pthread


#client_test.o:client_test.c
#	gcc -c -o client_test.o client_test.c -lrt -pthread


clean:
	-rm -f *.o tests
