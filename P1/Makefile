main.elf:main.c lightTask.o tempTask.o notify.o logTask.o errorhandling.o packetdefinition.h tmp102.o adps9301.o i2c_wrap.o
	gcc -g -o $@ $^ -lpthread -lrt -lm

notify.o:notify.c
	gcc -c -o $@ $<

logTask.o:logTask.c messageQue.h packetdefinition.h
	gcc -c -o $@ $<  -lpthread

lightTask.o:lightTask.c messageQue.h packetdefinition.h
	gcc -c -o $@ $<  -lpthread -lrt

tempTask.o:tempTask.c messageQue.h packetdefinition.h
	gcc -c -o $@ $<  -lpthread -lrt

errorhandling.o:errorhandling.c messageQue.h packetdefinition.h
	gcc -c -o $@ $<  -lpthread -lrt

tmp102.o:tmp102.c i2c_wrap.o
	gcc -c -o $@ $<

adps9301.o:adps9301.c i2c_wrap.o
	gcc -c -o $@ $< -lm

i2c_wrap.o: i2c_wrap.c
	gcc -c -o $@ $<

clean:
	rm -f *.o *.elf logfile.txt
