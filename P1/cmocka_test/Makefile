cmocka_tests: cmocka_tests.c ../tmp102.c ../adps9301.c ../i2c_wrap.c
		gcc -o $@ $^ -lcmocka -lm
clean:
	-rm -f *.o *.d
	-rm -f cmocka_tests
