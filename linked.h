/*!
 * \author D. Kevin McGrath (dmcgrath)
 *
 * \file linked_list.h
 *
 * Created: 2009 October 2, 13:48 by dmcgrath
 *
 * Last Modified: 2009 October 13, 14:42 by dmcgrath
 * 
 * \brief This file contains a linked list implementation, storing student records, and provides
 * descriptive statistics upon such records
 */

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include <math.h>

#ifndef DMCGRATH_LINKED_LIST_H
#define DMCGRATH_LINKED_LIST_H

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

	/*! \brief store the sort key in every node */
	int sort_key;
	/*! \brief store the sort order in every node */
	int sort_order;
};

/*!
 * \brief struct to hold the descriptive statistics
 */
struct stats{
	double mean;                           /*!< \brief hold the statistical mean */
	double median;  	                   /*!< \brief hold the statistical median */
	double stddev;	                       /*!< \brief statistical standard deviation */

};

#define FAMILY 1                           /*!< \brief constant to indicate family name as sort key */
#define GIVEN 0                            /*!< \brief constant to indicate given name as sort key */
#define ASCEND 1                           /*!< \brief constant to indicate ascending sort order */
#define DESCEND (-1)                       /*!< \brief constant to indicate descending sort order */

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b)) /*!< \brief macro definition of type independent max */
#define min(a,b) (((a) < (b)) ? (a) : (b)) /*!< \brief macro definition of type independent min */
#endif

/********************************************PROTOTYPES*********************************************/
/* required, exposed functionality */
struct node *insert(struct node* head, char *given, char *family, struct assignment *assignments,
                    long int num_assignments, int name_order, int sort_order);
struct node *head_node(struct node *head);
struct node *tail_node(struct node *head);
struct node *nth_node(struct node *head, int location);
struct node *delete_nth(struct node *head, int location);
struct node *find_by_name(struct node *head, char *name, int name_order);
struct node *reverse_list(struct node *head);
struct node *sort_list(struct node *head, int name_order, int sort_order);
void print_list(struct node *head);
int list_length(struct node *head);
struct node *delete_list(struct node *head);
struct node *list_from_file(struct node *head, FILE *stream, int sort_key, int sort_order);
struct assignment *assignment_list(struct node *head, char *given, char *family, long int *length);
struct stats student_statistics(struct node *head, char *given, char *family);
struct stats class_statistics(struct node *head, char *assignment);

/* helper functions */
double stddev(double *list, long int length);
double mean(double *list, long int length);
int compare(const void *s, const void *t);
double median(double *list, long int length);
struct node *head_pointer(struct node *head);
struct node *tail_pointer(struct node *head);
struct node *location(struct node* head, char *given, char *family);
void place(char *given, char *family, struct node *cursor, struct node *tmp);
int location_compare(char *name, char *name2, int direction);
int place_compare(char *name, char *name2, int direction);
struct node* delete_nth(struct node *head, int location);

void populate_node(struct node *n, char *first_name, char *last_name,
                   struct assignment *assignments, long int num_assignments,
                   int sort_key, int sort_order);
/***************************************************************************************************/


/*!
 * \brief A funtion to add the value to a given list, with a specified sort order
 *
 * \param head - the head of the list
 * \param family - the family name to store in the list
 * \param given - the given name to store in the list
 * \param assignments - the list of assignments for the given student
 * \param num_assignments - the length of the assignments list
 * \param name_order - which of the 2 names to use
 * \param sort_order - the direction of the sort \n
 *                   1 for ascending \n
 *                   -1 for descending
 * 
 * \return pointer to the head node
 */
struct node* insert(struct node* head, char *given, char *family, struct assignment *assignments,
                    long int num_assignments, int name_order, int sort_order){
	struct node *tmp;
	struct node *cursor;
	struct node *cursor2;	
	
	cursor = head_pointer(head);

	tmp = (struct node*) malloc(sizeof(struct node));
	populate_node(tmp, given, family, assignments, num_assignments, name_order, sort_order);

	if (head == NULL){
		/* this means the list is empty, so just create a node, and add the value to it */
		head = tmp;
		
		/* error check */
		if (head == NULL){
			return head;
		} 
		
	}else{
		/* make sure the list is sorted the same way */
		if (head->sort_key != name_order || head->sort_order != sort_order){
			head = sort_list(head, name_order, sort_order);
		}
		
		/* find the new location */		
		if (sort_order == ASCEND){
			cursor = location(head, given, family);
		}else if (sort_order == DESCEND){
			cursor = location(head, given, family);
		}
		
		/* insert the new node in the correct location */
		
		/* error check */
		if (tmp == NULL) return tmp;		
		
		place(given, family, cursor, tmp);
		
		
	}
	return head_pointer(head);
}


/*!
 * \brief return the head pointer
 *
 * \param head - the head of the list
 *
 * \return pointer to the head node
 */
struct node* head_node(struct node *head){
	return head_pointer(head);
}


/*!
 * \brief return the tail pointer
 *
 * \param head - the head of the list
 *
 * \return pointer to the tail node
 */
struct node* tail_node(struct node *head){
	return tail_pointer(head);
}


/*!
 * \brief Return the nth value in the list, or tail if n > list_length
 *
 * \param head - head of the list to consider 
 * \param location - the location to store the value:
 *                   location > 0 for forwards FROM HEAD
 *
 * \return pointer to node n
 */
struct node *nth_node(struct node *head, int location){
	if (head == NULL){
		return NULL;
	}
	
	struct node *cursor = head;
	int i;
	if (location > 0){
		for (i = 0; i < location; ++i){
			if (cursor->previous != NULL){
				cursor = cursor->previous;	
			}else{
				/* if n is larger than length(list), return tail */
				break;
			}
		}
	}
	
	return cursor;
}


/*!
 * \brief search the list for the given name, based on which name to look at
 *
 * \param head - the head of the list
 * \param name - the name to search for
 * \param name_order - whether to search on given or family name
 *
 * \return pointer to the node containing string we search for, or null if not found
 */
struct node* find_by_name(struct node *head, char *name, int name_order){
	struct node *cursor = head_pointer(head);
	
	/* I hate special cases, but this one is unavoidable in this situation */
	if (name_order == GIVEN){
		while(strcmp(name, cursor->first_name) < 0){
			if (cursor->previous != NULL){
				cursor = cursor->previous;
			}
		}
		if (strcmp(name, cursor->first_name) > 0){
			return NULL;
		}else{
			/* not <, not >, so must be == */
			return cursor;
		}
	}
	else if (name_order == FAMILY){
		while(strcmp(name, cursor->last_name) < 0){
			if (cursor->previous != NULL){
				cursor = cursor->previous;
			}
		}
		if (strcmp(name, cursor->last_name) > 0){
			return NULL;
		}else{
			/* not <, not >, so must be == */
			return cursor;
		}
	}
	
	return NULL;
}


/*!
 * \brief reverses the list, returning the new head pointer
 *
 * \param head - the head of the list
 *
 * \return pointer to the head node
 */
struct node* reverse_list(struct node *head){
	if (head == NULL) return head;
	
	struct node *cursor = head_pointer(head);
	struct node *cursor2;
	struct node *lag;
	
	if (list_length(head) == 1) return head;
	
	/* cursor->previous exists */
	while(cursor != NULL){
		/* swap the sort order */
		cursor->sort_order *= -1;
		
		/* set the lag pointer to current value */
		lag = cursor;
		
		/* swap the nodes around */
		cursor2 = cursor->previous;
		cursor->previous = cursor->next;
		cursor->next = cursor2;
		
		cursor = cursor2;
	}
	
	/* lag now points at what USED TO BE the tail, but is now the head */
	return lag;
}


/*!
 * \brief sorts the list based on passed parameters. Does so intelligently, in that only fully
 * resorts as needed, reversing or leaving alone as possible
 *
 * \param head - the head of the list
 * \param name_order - which of the 2 names to use
 * \param sort_order - the direction of the sort
 *                   1 for ascending
 *                   -1 for descending
 *
 * \return pointer to the head node
 */
struct node* sort_list(struct node *head, int name_order, int sort_order){
	
	struct node* new_head = NULL;
	struct node* cursor = head_pointer(head);
	
	if(head != NULL){
		if (head->sort_key == name_order){
			/* sort key is same, so check for reversal or not */
			if (head->sort_order == sort_order){
				/* no change */
				new_head = head;
			}
			else{
				/* resets the internal sort representation, and reverses list */
				new_head = reverse_list(head);
			}
		}
		else{
			/* sort key is changed, so just create a new list */
			while (cursor != NULL){
				new_head = insert(new_head, cursor->first_name, cursor->last_name, cursor->assignments, 
				                  cursor->num_assignments, name_order, sort_order);
				cursor = cursor->previous;
			}
			
			/* get rid of the original list */
			delete_list(head);
		}
	}
	
	return head_pointer(new_head);
}


/*!
 * \brief Print the list, in order
 *
 * \param head - the head of the list
 */
void print_list_file(struct node *head, FILE *stream){
	struct node *cursor = head_pointer(head);
	int length = list_length(cursor);
	
	if (length == 0) return;
	
	if (cursor == NULL) return;
	
	fprintf(stream, "%d,%ld\n", length, head->num_assignments);

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
 * \brief Print the list, in order
 *
 * \param head - the head of the list
 */
void print_list(struct node *head){
	struct node *cursor = head_pointer(head);
	int length = list_length(cursor);
	
	if (length == 0) return;
	
	if (cursor == NULL) return;
	
	for (; length > 0; --length){
		printf("%s %s%s", cursor->first_name, cursor->last_name, (cursor->previous==NULL) ? "\n" : ", ");
		cursor = cursor->previous;
	}
	
	return;
}


/*!
 * \brief Get the length of the list
 *
 * \param head - the head of the list
 *
 * \return length of list pointed to by head
 */
int list_length(struct node *head){
	struct node *cursor = head_pointer(head);
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
 * \brief delete the whole list, making use of delete_nth
 *
 * \param head - the head of the list
 *
 * \return pointer to NULL, if successful
 */
struct node* delete_list(struct node *head){
	while(head != NULL){
		head = delete_nth(head, 0);
	}
	
	return head;
}


/*!
 * \brief reads a well formatted file, and inserts all entries found into a list pointed to by head,
 * with the given sort order.
 * 
 * \param head - pointer to a list (possibly NULL)
 * \param stream - open file stream in read mode
 * \param sort_key - whether to sort on first or last name
 * \param sort_order - whether to sort ascending or descending
 *
 * \return pointer to the head node
 */
struct node *list_from_file(struct node *head, FILE *stream, int sort_key, int sort_order){
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

	int match_count = 2;

	/* generalize the formatting strings */
 	snprintf(name_format, MAX_STRING_LENGTH, "%%%d[^\',\'],%%%d[^\',\']", MAX_STRING_LENGTH, MAX_STRING_LENGTH);
	snprintf(grade_format, MAX_STRING_LENGTH, ",%%%d[^\',\'],%%lf", MAX_STRING_LENGTH);

	matched = fscanf(stream, "%d,%d ", &number_records, &number_pairs);
	if(matched == match_count){
		assignments = (struct assignment *)malloc(sizeof(struct assignment) * number_pairs);
		for(int i = 0; i < number_records; i++){
			matched = fscanf(stream, name_format, first_name, last_name);

			if (matched != match_count){
				/* matching error */
				continue;
			}

			for(int j = 0; j < number_pairs; j++){
				matched = fscanf(stream, grade_format, assignment, &score);
				if (matched != match_count){
					continue;
				}else{
					assignments[j].name = (char *)malloc(min(MAX_STRING_LENGTH, strlen(assignment)));
					strncpy(assignments[j].name, assignment, min(MAX_STRING_LENGTH, strlen(assignment)));
					assignments[j].value = score;
				}
			}
			
			head = insert(head, first_name, last_name, assignments, number_pairs, sort_key, sort_order);

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
 * \brief searches the list for a given student using the current sort key of the list, and returns
 * a pointer to their assignment list
 * 
 * \param head - head of the list
 * \param given - given name in question
 * \param family - family name in question
 * \param length - output parameter to hold the number of assignments
 *
 * \return pointer to the assignment list of student searched for
 */
struct assignment *assignment_list(struct node *head, char *given, char *family, long int *length){
	struct node* cursor;
	char *names[] = {given, family};

	if (head != NULL){
		/* find by using the primary sort key as the passed name, and use the list to determine */
		cursor = find_by_name(head, names[head->sort_key], head->sort_key);
	}

	/* assign the output parameter */
	*length = cursor->num_assignments;
	
	return cursor->assignments;
}


/*!
 * \brief returns overall descriptive statistics for a given student. Returns a struct containing
 * the mean, median, and standard deviation of their grade list.
 * 
 * \param head - head of the list
 * \param given - given name of student 
 * \param family - family name of student 
 *
 * \return stats struct containing descriptive statistics
 */
struct stats student_statistics(struct node *head, char *given, char *family){
	struct stats tmp;

	long int length;
	struct assignment *assignments = assignment_list(head, given, family, &length);
	double list[length];

	for(int i = 0; i < length; ++i){
		list[i] = assignments[i].value;
	}

	tmp.median = median(list, length);
	tmp.mean = mean(list, length);
	tmp.stddev = stddev(list, length);

	return tmp;
}


/*!
 * \brief descriptive statistics over the whole class for a given assignment name. long ints are
 * used to allow for obscene numbers of assignments
 * 
 * \param head - pointer into the list
 * \param assignment - assignment in question
 *
 * \return stats struct containing descriptive statistics
 */
struct stats class_statistics(struct node *head, char *assignment){
	/* traverse the list, pulling out the assignments list from each node you traverse */

	struct node *cursor = head_pointer(head);
	struct assignment *assignments = NULL;

	struct stats tmp;

	/* length is necessary for several things */
	long int length = list_length(head);
	/* including the number of students we care about */
	double *list = malloc(length * sizeof(double));
	long int entry = 0;

	/* declare this loop variable externally to loop to test termination condition */
	long int j;

	for(long int i = 0; i < length && cursor != NULL; cursor = cursor->previous, ++i){
		assignments = cursor->assignments;
		
		/* if there are assignments */
		if (assignments != NULL){

			/* walk down the list of them */
			for (j = 0; j < cursor->num_assignments; ++j){

				/* pulling out the values you care about*/
				if(strcmp(assignments[i].name, assignment) == 0){
					list[entry++] = assignments[i].value;
					break;
				}
			}

			/* if it isn't there, just assume they got a zero on it*/
			if (j == cursor->num_assignments){
				list[entry++] = 0.0;
			}
		}
	}

	/* calculate and store the values we care about */
	tmp.mean = mean(list, length);
	tmp.stddev = stddev(list, length);
	tmp.median = median(list, length);


	free(list);
	/* and copy them back to the user -- no harm in copying, as struct is small and all automatic 
	   storage */
	return tmp;
}


/*!
 * \brief Calculates the standard deviation of the value list.
 *
 * \param list - list of values we are interested in
 * \param length - how many values there are
 *
 * \return standard deviation of list, as double
 */
double stddev(double *list, long int length){
	double mu = mean(list, length);
	double sigma_2 = 0.0;
	double square = 2.0;
	
	/* sum the deviants (squares of the differences from the average) */
	for (int i = 0; i < length; ++i){
		sigma_2 += pow(list[i] - mu, square);
	}
	
	sigma_2 /= (length - 1);
	
	return sqrt(sigma_2);
}


/*!
 * \brief Returns the statistical mean of the list of values
 *
 * \param list - list of values we are interested in
 * \param length - how many values there are
 *
 * \return statistical mean, as double
 */
double mean(double *list, long int length){
	double total = 0.0;
	
	for (long int i = 0; i < length; ++i){
		total += list[i];
	}
	
	return total / length;
} 

/*!
 * \brief compare function used by qsort
 *
 * \param s - double value, passed as void*
 * \param t - double value, passed as void*
 *
 * \return an integer less than, equal to, or greater than zero if the first argument is considered
 * to be respectively less than, equal to, or greater than the second.
 */
int compare(const void *s, const void *t){
	int comp = 1;
	
	double is = *(double *)s;
	double it = *(double *)t;
	
	if (is < it){
		comp = -1;
	}else if (is == it){
		comp = 0;
	}
	
	return comp;
}



/*!
 * \brief returns the statistical median of the values passed in
 *
 * \param list - list of values we are interested in
 * \param length - how many values there are
 *
 * \return statistical mean as double
 */
double median(double *list, long int length){
	double med = 0.0;
	
	/* use the built in quicksort function to sort the list to extract the median */
	/* requires the use of a function pointer */
	qsort(list, length, sizeof(double), compare);
	
	if (length % 2 == 0){
		/* if the list is an even length, return average of middle 2 elements*/
		med = list[length/2 - 1] + list[length/2];
		med /= 2.0;
	}else{
		/* otherwise return the middle element (correct due to 0 offset) */
		med = list[length/2];
	}
	
	return med;
}


/*!
 * \brief return the head pointer
 *
 * \param head - the head of the list
 *
 * \return pointer to the head node
 */
struct node* head_pointer(struct node *head){
	struct node* cursor = head;
	
	if (head != NULL){
		while (cursor->next != NULL){
			cursor = cursor->next;
		}
	}
	
	return cursor;
}

/*!
 * \brief return the tail pointer
 *
 * \param head - the head of the list
 *
 * \return pointer to the tail node
 */
struct node* tail_pointer(struct node *head){
	struct node* cursor = head;
	
	if (head != NULL){
		while (cursor->previous != NULL){
			cursor = cursor->previous;
		}
	}
	
	return cursor;
}


/*!
 * \brief A funtion to find the location to insert into the list
 *
 * \param head - the head of the list
 * \param family - the family name to look for
 * \param given - the given name to look for
 *
 * \return pointer to the node AFTER where the insertion should occur
 */
struct node *location(struct node* head, char *given, char *family){
	struct node *cursor = head;
	
	/* error checking */
	if (cursor != NULL){
		
		/* decide which sort key to use */
		if (cursor->sort_key == GIVEN){
			while(location_compare(given, cursor->first_name, cursor->sort_order)){
				/* given name is greater/less than current node, so move */
				if (cursor->previous != NULL){
					cursor = cursor->previous;
				}
				else{
					break;
				}
			}
		}else{
			while(location_compare(family, cursor->last_name, cursor->sort_order)){
				/* family name is greater/less than current node, so move */
				if (cursor->previous != NULL){
					cursor = cursor->previous;
				}
				else{
					break;
				}
			}
		}
	}
	
	/* will still be null if it started out that way */
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
void place(char *given, char *family, struct node *cursor, struct node *tmp){
	
	/* I hate special cases, so I work around them */
	/* this works because I carefully chose the values to be used for sort_key */
	char *names[] = {given, family};
	char *node_names[] = {cursor->first_name, cursor->last_name};
	
	if (place_compare(node_names[cursor->sort_key], names[cursor->sort_key], cursor->sort_order)){
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

/* next 2 functions are used to remove special case logic from location/place functions */
/*!
 * \brief remove special case logic from location, by abstracting it out
 * 
 * \param name - name in node to insert 
 * \param name2 - name in list node to compare
 * \param direction - which ordering to use 
 *
 * \return result of comparison between \a name and \a name2, based on \a direction
 */
int location_compare(char *name, char *name2, int direction){
	if (direction == DESCEND){
		return (strcmp(name, name2) < 0);
	}else{
		return (strcmp(name, name2) > 0);
	}
}

/*!
 * \brief remove special case logic from place, by abstracting it out
 * 
 * \param name - name in node to insert 
 * \param name2 - name in list node to compare
 * \param direction - which ordering to use
 *
 * \return result of comparison between \a name and \a name2, based on \a direction
 */
int place_compare(char *name, char *name2, int direction){
	if (direction == ASCEND){
		return (strcmp(name, name2) < 0);
	}else{
		return (strcmp(name, name2) > 0);
	}
}


/*!
 * remove the nth element from the list and return the pointer to the (possibly new) head
 *
 * \param head - the head of the list
 * \param location - the location to remove
 *
 * \return pointer to the head node
 */
struct node* delete_nth(struct node *head, int location){
	
	if (head != NULL){

	
		struct node *cursor = head_pointer(head);
		int i;

		if (location >= 0){
			for (i = 0; i < location; ++i){
				if (cursor->previous != NULL){
					cursor = cursor->previous;	
				}
			}
		}
	
		/* remove the node */
		if (list_length(head) > 1){
			/* if the length of the list is greater than 1, remove the node */
			if (cursor->next != NULL) {
				cursor->next->previous = cursor->previous;	
			}else{
				/* is the head, so we need to define a new head */
				head = cursor->previous;
				if (head->previous != NULL){
					head->previous->next = NULL;
				}
			}
			if (cursor->previous != NULL){
				cursor->previous->next = cursor->next;	
			}else{
				/* is the tail, and next exists */
				cursor->next->previous = NULL;
			}
		} else{
			/* otherwise it's just the head */
			head = NULL;
		}  
	
	
		/* free up the names */
		free(cursor->first_name);
		free(cursor->last_name);
		/* don't forget to free up each of the assignment names */
		if (cursor->assignments != NULL){
			for (int i = 0; i < cursor->num_assignments; ++i){
				free(cursor->assignments[i].name);
			}
		}
		/* then the assignment list itself */
		free(cursor->assignments);
		/* and finally the node itself */
		free(cursor);
	}
	
	return head;
}


/*!
 * \brief A funtion to add the value to a given list, with a specified sort order
 * \param n - pointer to the node in question
 * \param last_name - the family name to store in the node
 * \param first_name - the given name to store in the node
 * \param assignments - the list of assignments for the given student
 * \param num_assignments - the length of the assignments list
 * \param sort_key - which of the 2 names to use
 * \param sort_order - the direction of the sort
 *                   1 for ascending
 *                   -1 for descending
 */
void populate_node(struct node *n, char *first_name, char *last_name,
                   struct assignment *assignments, long int num_assignments, 
                   int sort_key, int sort_order){
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
		
		/* store the sorting information */
		n->sort_order = sort_order;
		n->sort_key = sort_key;
		
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
#endif