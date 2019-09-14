/*
 * PrintingFile.c
 *
 *  Created on: Sep 9, 2019
 *      Author: afrah
 */

#include <stdio.h>
#include <string.h>
#include<stdlib.h>
#include<ctype.h>
#include<sys/wait.h>
#include<unistd.h>
struct entry {
    unsigned int truth;
    int element;
    unsigned int count;
    struct entry *prev;
};
typedef struct entry entry;
int subset_sum(const int *weights, size_t len, int target,int **solution)
{
    entry **table;
    int i, j;
    entry *head;
    int count = 0;

    table = malloc((target + 1) * sizeof(entry *));
    for (i = 0; i <= target; i++) {
        table[i] = malloc((len + 1 ) * sizeof(entry));
    }

    for (i = 0; i <= target; i++) {
        for (j = 0; j <= len; j++) {
            /* First row */
            if (i == 0) {
                table[i][j].truth = 1;
                table[i][j].element = -1;
                table[i][j].count = 0;
                table[i][j].prev = NULL;
            }
            else if (j == 0) {
                /* First column */
                table[i][j].truth = 0;
                table[i][j].element = -1;
                table[i][j].count = 0;
                table[i][j].prev = NULL;
            }
            else {
                /* Initialise for don't add element case */
                table[i][j].truth = table[i][j-1].truth;
                table[i][j].prev = &table[i][j - 1];
                table[i][j].element = -1;
                table[i][j].count = table[i][j - 1].count;
                if (i >= weights[j-1]) {
                    /* Can add element */
                    if (!table[i][j].truth) {
                        /* Add element */
                        table[i][j].truth = table[i - weights[j-1]][j-1].truth;
                        table[i][j].element = j - 1;
                        table[i][j].count = table[i - weights[j-1]][j-1].count + 1;
                        table[i][j].prev = &table[i - weights[j-1]][j-1];
                    }
                }
            }
        }
    }

    if (!table[target][len].truth) {
        /* No solution */
        *solution = NULL;
    }
    else {
        /* Solution */
        *solution = calloc(len, sizeof(int));
    }
    if (*solution) {
        /* Read back elements */
        count = table[target][len].count;
        for (head = &table[target][len]; head != NULL; head = head->prev) {
            if (head->element != -1) {
                (*solution)[head->element]++;
            }
        }
    }

    for (i = 0; i <= target; i++) {
        free(table[i]);
    }
    free(table);

    return count;
}
void forkingfunction(int size, int values[],int numarray[])
{
	int i,j,starting=1,temp=1,status;

	for(i=0;i<size;i++,temp++)
	    {
	    	printf("\nStarting fork for loop i=%d \n",i);
	    	 pid_t pID = vfork();

	    	   if (pID == 0)                // child
	    	   {
	    		   printf("\nchild pid %d\n",getpid());


	    		   int sum=values[starting];
	    		   printf("sum=%d\n",sum);
	    		   starting++;
	    		   int present_array[numarray[temp]];
	    		   printf("values for child:\n");
	    		   for(j=0;j<(numarray[temp]-1);j++)
	    		   {
	    			   present_array[j]=values[starting];
	    			   starting++;
	    			   printf("present_array[%d]=%d ",j,present_array[j]);
	    		   }
	    		   const size_t len = sizeof(present_array) / sizeof(present_array[0]);
	    		        int *solution;
	    		       const int count = subset_sum(present_array, len, sum, &solution);
	    		       if (count)
	    		       {
	    		           int i;
	    		           printf("Found a solution\n");
	    		           printf("Elements:\n");
	    		           for (i = 0; i < len; i++)
	    		           {
	    		               if (solution[i])
	    		               {
	    		                   printf("%u ", present_array[i]);
	    		               }
	    		           }
	    		           putchar('\n');
	    		       }
	    		       else
	    		       {
	    		           printf("No solution\n");
	    		       }


	    		       exit(EXIT_SUCCESS);
	    		       kill(pID,SIGKILL);
	    	   }
	    	   else if (pID < 0)            // failed to fork
	    	   {
	    		   perror("Failed to fork:");
	    	   }
	    	   else
	    	   {
	    		  // wait(NULL);
	    		   if ((pID = wait(&status)) == -1)
	    		   		    		   {
	    		   		    			   perror("Error in wait:");
	    		   		    		   }
	    	   }
	    }
}

int main(void)
{
    int tek;
    int size,i,totalsize=0;

    char c;



    // open file
    FILE *myFile = fopen("input.dat", "r");
    // if opening file fails, print error message and exit 1
    if (myFile == NULL)
    {
        perror("Error: Failed to open file.");
        return 1;
    }
    fscanf(myFile, "%d", &size);
    printf("size %d",size);
int values[size],numarray[size];
memset( values, 0, size*sizeof(int) );//contains array containing line numbers
memset(numarray,0,size*sizeof(int));//contains size of each line
for(i=0;i<=size;i++)
{
	values[i]=1;//initializing values in radica
	numarray[i]=0;//initializing array values to 0
}
i=0;
    while ((c=getc(myFile)) != EOF)
    {
   	   	  // Increment character count if NOT new line or space
   		    if (isdigit(c))
   		    {
   		    	numarray[i]=numarray[i]+1;
   		    	//printf("numarray[%d]=%d",i,numarray[i]);
   		    	totalsize=totalsize+numarray[i];
   		    }
   		    else if(c=='\n')
   		    	{
   		    	//printf("numarray[%d]=%d",i,numarray[i]);
   		    	//printf("starting new line");
   		    	i++;
   		    	}

    }
   // printf("numarray[2]=%d\n",numarray[2]);

    fseek(myFile, 0, SEEK_SET);//making the file pointer to beginning of the file
    //for(i=0;i<size;i++)
    // read values from file until EOF is returned by fscanf
printf("\n");
    for ( i = 0; i < totalsize; ++i)
    {

        // assign the read value to variable (tek), and enter it in array (radica)
        if (fscanf(myFile, "%d", &tek) == 1)
        {
            values[i] = tek;//overwriting values line values
            printf("values[%d]=%d ",i,values[i]);
        } else
        {
            // if EOF is returned by fscanf, or in case of error, break loop
            break;
        }
    }
    printf("\n");

    forkingfunction(size,values,numarray);

    // close file
    fclose(myFile);
    return 0;
}
