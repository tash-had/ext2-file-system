
all: ext2_mkdir ext2_cp

ext2_mkdir : ext2_mkdir.o utils.o path_utils.o
	gcc -Wall -g -o ext2_mkdir $^

ext2_cp : ext2_cp.o utils.o path_utils.o
	gcc -Wall -g -o ext2_cp $^

*.o : *.c *.h
	gcc -Wall -g -c $<

clean :
	rm -f *.o ext2_mkdir  *~

