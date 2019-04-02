
ext2_mkdir : ext2_mkdir.o utils.o path_utils.o
	gcc -Wall -g -o ext2_mkdir $^

*.o : *.c *.h
	gcc -Wall -g -c $<

clean :
	rm -f *.o ext2_mkdir  *~

