/* Original Author: Will Kuhn (kuhnw)
 * File: kuhnw_hw1.txt
 * Created: 2012 March 3, 15:23 by kuhnw
 * Last Modified: 2012 March 4, 17:00 by kuhnw
 * 
 * This file contains the main function  for the assignment 3
 * project. Aka gradebook program
 */

#include "kuhnw_hw2.h"
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include <math.h>


 
int main(int argc, char **argv){
	FILE *fp = fopen("input_list.txt", "r");
	list_from_file(NULL, fp);
	FILE *paste = fopen("output_list.txt", "w");
	print_list_file(NULL, paste);
	return 0;
}

