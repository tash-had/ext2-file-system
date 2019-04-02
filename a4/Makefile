
ext2_mkdir :  ext2_mkdir.o
	gcc -Wall -g -o sim $^

%.o : %.c pagetable.h sim.h
	gcc -Wall -g -c $<

clean :
	rm -f *.o *~
