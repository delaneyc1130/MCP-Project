all: part1 part2 part3 part4

part1: 
		gcc -g part1.c -o part1.out

part2:
		gcc -g part2.c -o part2.out

part3:
		gcc -g part3.c -o part3.out

part4:
		gcc -g part4.c -o part4.out

clean:
		rm part1.out part2.out part3.out part4.out

