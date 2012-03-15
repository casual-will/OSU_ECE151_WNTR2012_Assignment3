/* Original Author: Will Kuhn (kuhnw)
 * File: kuhnw_hw2.h
 * Created: 2012 March 11, 15:23 by kuhnw
 * Last Modified: 2012 March 4, 17:00 by kuhnw
 * Uses code from Kevin McGrath (dmcgrath)
 *
 * \brief This file contains the functions for the assignment 3
 * project. Aka gradebook program
 * linked list implementation,
 * storing student records, and provides
 * descriptive statistics upon such records
 */

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include <math.h>

#ifndef KUHNW_HW2_H
#define KUHNW_HW2_H
//The file to be inputted
//#define FILE input_list
/*! \brief the maximum value for any string in the list */
#define MAX_STRING_LENGTH 100

/*!
 * \brief Holds the values which comprise the elements of an assignment pair.
 */
struct assignment{
	char *name;     /*!< \brief string to hold the name of the assignment */
	double value;	/*!< \brief hold the given score of the assignment, normalized */
};

/*!
 * \brief core struct for the linked list. Note the self referential pointers.
 */
struct node{
	/*! \brief self referential pointer to link to nodes ahead */
	struct node *next;
	/*! \brief self referential pointer to link to nodes behind */
	struct node *previous;

	/*! \brief string to hold the first name of the student */
	char *first_name;
	/*! \brief string to hold the last name of the student */
	char *last_name;

	/*! \brief pointer to the dynamic assignments array */
	struct assignment *assignments;
	/*! \brief keep a count of the number of assignments */
	long int num_assignments;

};

/*!
 * \brief struct to hold the descriptive statistics
 */
struct stats{
	double mean;                       /*!< \brief hold the statistical mean */
	double median;  	           /*!< \brief hold the statistical median */
	double stddev;	                   /*!< \brief statistical standard deviation */

};

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
 /*!< \brief macro definition of type independent max */
#define min(a,b) (((a) < (b)) ? (a) : (b))
 /*!< \brief macro definition of type independent min */
#endif


/********************************************PROTOTYPES*********************************************/
//main functionality
struct node *list_from_file(struct node *head, FILE *stream);
struct node* insert(struct node* head, char *given, char *family, 
                    struct assignment *assignments, long int num_assignments);
void print_list(struct node *head);

//Helper
struct node *location(struct node* head, char *family);
int location_compare(char *name, char *name2);
void populate_node(struct node *n, char *first_name, char *last_name,
                   struct assignment *assignments, long int num_assignments);
void place(char *family, struct node *cursor, struct node *tmp);
struct node* head_node(struct node *head);

/***************************************************************************************************/


/****MAIN****/

 /*!
 * \brief reads a well formatted file, and inserts all entries found into a list pointed to by head,
 * with the given sort order.
 * 
 * \param head - pointer to a list (possibly NULL)
 * \param stream - open file stream in read mode
 *
 * \return pointer to the head node
 */
struct node *list_from_file(struct node *head, FILE *stream){
	int number_records = 0;
	int number_pairs = 0;
	char first_name[MAX_STRING_LENGTH];
	char last_name[MAX_STRING_LENGTH];
	char assignment[MAX_STRING_LENGTH];
	double score;

	int matched;

	char name_format[MAX_STRING_LENGTH];
	char grade_format[MAX_STRING_LENGTH];

	struct assignment *assignments;

	/* generalize the formatting strings */
	snprintf(name_format, MAX_STRING_LENGTH, "%%%d[^\',\'],%%%d[^\',\']", MAX_STRING_LENGTH, MAX_STRING_LENGTH);
	snprintf(grade_format, MAX_STRING_LENGTH, ",%%%d[^\',\'],%%lf", MAX_STRING_LENGTH);
	
	matched = fscanf(stream, "%d,%d ", &number_records, &number_pairs);
	if(matched == 2){
		assignments = (struct assignment *)malloc(sizeof(struct assignment) * number_pairs);
		for(int i = 0; i < number_records; i++){
			matched = fscanf(stream, name_format, first_name, last_name);
			if (matched != 2){
				/* matching error */
				continue;
			}

			for(int j = 0; j < number_pairs; j++){
				matched = fscanf(stream, grade_format, assignment, &score);
				if (matched != 2){
					continue;
				}else{
					assignments[j].name = (char *)malloc(min(MAX_STRING_LENGTH, strlen(assignment)));
					strncpy(assignments[j].name, assignment, min(MAX_STRING_LENGTH, strlen(assignment)));
					assignments[j].value = score;
				}
			}
			head = insert(head, first_name, last_name, assignments, number_pairs);

			/* free up the assignments you malloc'd */
			for(int j = 0; j < number_pairs; j++){
				free(assignments[j].name);
			}
		}
		/* and then free up assignments */
		free(assignments);
	}
	return head;
}
/*!
 * \brief Get the length of the list
 *
 * \param head - the head of the list
 *
 * \return length of list pointed to by head
 */
int list_length(struct node *head){
	struct node *cursor = head_node(head);
	if (cursor == NULL) return 0;
	int length = 0;
	
	/* count the first value, then loop until you hit NULL */
	do{
		++length;
		cursor = cursor->previous;
	}while(cursor != NULL);
	
	return length;  
}
/*!
 * \brief Print the list, in order
 *
 * \param head - the head of the list
 */
void print_list_file(struct node *head, FILE *stream){
	struct node *cursor = head_node(head);
	int length = list_length(cursor);
	
	if (length == 0) return;
	if (cursor == NULL) return;
	fprintf(stream, "%d,%ld\n", length, head->num_assignments);
	printf("WE WIN\n");
	for (; length > 0; --length){
		fprintf(stream, "%s,%s", cursor->first_name, cursor->last_name);
		for (int i = 0; i < head->num_assignments; ++i){
			fprintf(stream, ",%s,%lf", cursor->assignments[i].name, cursor->assignments[i].value);
		}
		fprintf(stream, "\n");
		cursor = cursor->previous;
	}
	
	return;
}

/*!
 * \brief A funtion to add the value to a given list, with a specified sort order
 *
 * \param head - the nominal head of the list
 * \param family - the family name to store in the list
 * \param given - the given name to store in the list
 * \param assignments - the list of assignments for the given student
 * \param num_assignments - the length of the assignments list
 * 
 * \return pointer to the head node
 */
struct node* insert(struct node* head, char *given, char *family, 
                    struct assignment *assignments, long int num_assignments){
	struct node *tmp;
	struct node *cursor;
	struct node *cursor2;	
	
	cursor = head_node(head);

	tmp = (struct node*) malloc(sizeof(struct node));
	populate_node(tmp, given, family, assignments, 
		num_assignments);

	if (head == NULL){
	/* this means the list is empty, so just create a node, and add the value to it*/
		head = tmp;
		
		/* error check */
		if (head == NULL){
			return head;
		}
	}else{
		cursor = location(head, family);
		
		/* insert the new node in the correct location */
		/* error check */
		if (tmp == NULL) return tmp;		
		
		place(family, cursor, tmp);		
		
	}
	/* in case the head changes */
	return head_node(head);
}
/****END MAIN****/
/****START HELPER FUNCTIONS****/
/*!
 * \brief return the head pointer
 *
 * \param head - the head of the list
 *
 * \return pointer to the head node
 */
struct node* head_node(struct node *head){
	struct node* cursor = head;
	if (head != NULL){
		while (cursor->next != NULL){
			cursor = cursor->next;
		}
	}
	return cursor;
}

/*!
 * \brief A funtion to deal with special case tail logic
 *
 * \param tmp - the node to insert into the list
 * \param cursor - the node adjacent to where tmp needs to be inserted (should be the tail)
 * \param family - the family name to store in the list
 * \param given - the given name to store in the list
 */
void place(char *family, struct node *cursor, struct node *tmp){
	
	if (strcmp(cursor->last_name, family) > 0){
		/* does it go before the tail? */
		tmp->previous = NULL;
		cursor->previous = tmp;
		tmp->next = cursor;
	}else{
		/* or after? */
		tmp->previous = cursor;
		tmp->next = cursor->next;
		if (cursor->next != NULL){
			cursor->next->previous = tmp;
		}
		cursor->next = tmp;
	}
}
/*!
 * \brief A funtion to add the value to a given list, with a specified sort order
 * \param n - pointer to the node in question
 * \param last_name - the family name to store in the node
 * \param first_name - the given name to store in the node
 * \param assignments - the list of assignments for the given student
 * \param num_assignments - the length of the assignments list
 * 
 */
void populate_node(struct node *n, char *first_name, char *last_name,
                   struct assignment *assignments, long int num_assignments){
	int first_name_length = min(strlen(first_name) + 1, MAX_STRING_LENGTH);
	int last_name_length = min(strlen(last_name) + 1, MAX_STRING_LENGTH);
	int assignment_name_length = 0;
	
	if (n != NULL){
		/* sets the first name */
		n->first_name = (char *)malloc(first_name_length);
		strncpy(n->first_name, first_name, first_name_length);
		
		/* do the same for last name */
		n->last_name = (char *)malloc(last_name_length);
		strncpy(n->last_name, last_name, last_name_length);
		
		/* store the assignment count */
		n->num_assignments = num_assignments;
		
		/* malloc the assignment array */
		n->assignments = (struct assignment *)malloc(num_assignments * sizeof(struct assignment));
		
		if (assignments != NULL){
			for (int i = 0; i < num_assignments; ++i){
				/* determine the length of the assignment name */
				assignment_name_length = min(strlen(assignments[i].name), 
				                             MAX_STRING_LENGTH);
				/* and allocate storage for it */
				n->assignments[i].name = (char *)malloc(assignment_name_length);
				
				/* then copy it into its proper place */
				strncpy(n->assignments[i].name,
					assignments[i].name,
					assignment_name_length);
				n->assignments[i].value = assignments[i].value;
			} /* for */
		} /* if (assignments != NULL) */
		
		/* we don't know where these should point yet, so just NULL them out */
		n->next = NULL;
		n->previous = NULL;
	} /* if (n != NULL) */
				    
	return;
}

/*!
 * \brief A funtion to find the location to insert into the list
 *
 * \param head - the head of the list
 * \param family - the family name to look for
 *
 * \return pointer to the node AFTER where the insertion should occur
 */
struct node *location(struct node* head, char *family){
	struct node *cursor = head;
	/* error checking */
	if (cursor != NULL){
		while((strcmp(family, cursor->first_name) < 0)){
		//OR return (strcmp(name, name2) > 0);
			/* given name is greater/less than current node, so move */
			if (cursor->previous != NULL){
				cursor = cursor->previous;
			}
			else{
				break;
			}
		}
	}
	/* will still be null if it started out that way */
	return cursor;
}
/****END HELPER FUNCTIONS****/

#endif

