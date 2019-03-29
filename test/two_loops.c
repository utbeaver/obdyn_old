/*This is a model to test the loop-finding for the following model

 bodies      0->1->2->3->0    1->4->5->6->2->1
 connections  1  2  3  7       4  5  6  8  1 

#include <stdio.h>
extern void print_a(int *g, int m, int n);
extern void process_loop(int*, int*, int, int);
main() {
 
    int b, i, j;
    int g[] = {
	0,  -1, 0,  0,  0, 1,
	0,  0,  1,  0,  0, 0,
	1,  0,  0,  0,  0, 0,
	-1, 1,  0,  0,  0, 0,
	0, -1,  1,  0,  0, 0,
	-1, 0,  0,  1,  0, 0,
	0,  0,  0, -1,  1, 0,
	0,  0,  0,  0, -1, 1

    };

	int h[] = {
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 1, 0,
	1, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,	
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0
	};

	process_loop(g, h, 8, 6);
	
	print_a(g, 8,6);
	print_a(h, 8,8);

	printf("\n");
}
