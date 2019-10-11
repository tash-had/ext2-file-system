
all: ext2_mkdir ext2_cp ext2_ln ext2_rm ext2_checker ext2_restore

ext2_mkdir : ext2_mkdir.o utils.o path_utils.o
	gcc -Wall -g -o ext2_mkdir $^

ext2_cp : ext2_cp.o utils.o path_utils.o
	gcc -Wall -g -o ext2_cp $^

ext2_ln : ext2_ln.o utils.o path_utils.o
	gcc -Wall -g -o ext2_ln $^

ext2_rm : ext2_rm.o utils.o path_utils.o
	gcc -Wall -g -o ext2_rm $^

ext2_restore : ext2_restore.o utils.o path_utils.o
	gcc -Wall -g -o ext2_restore $^

ext2_checker : ext2_checker.o utils.o path_utils.o
	gcc -Wall -g -o ext2_checker $^

*.o : *.c *.h
	gcc -Wall -g -c $<

clean :
	rm -f *.o ext2_mkdir ext2_cp ext2_ln ext2_rm ext2_restore ext2_checker  *~

