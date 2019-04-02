
ext2_mkdir : ext2_mkdir.o common.h utils.h path_utils.h
	gcc -Wall -g -o ext2_mkdir $^

*.o : *.c *.h
	gcc -Wall -g -c $<

clean :
	rm -f *.o ext2_mkdir  *~

