/*
 * PrintingFile.c
 *
 *  Created on: Sep 9, 2019
 *      Author: afrah
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include<stdbool.h>
#include <ctype.h>
#include<sys/wait.h>
#include<signal.h>
struct entry {
    unsigned int truth;
    int element;
    unsigned int count;
    struct entry *prev;
};
bool alarm_flag=false;
typedef struct entry entry;

int subset_sum(const int *weights, size_t len, int target,int **solution,pid_t pID,char *output, int flag)
{
	clock_t t;
	double time_limit=1;
    t = clock();
    entry **table;
    int i, j;
    entry *head;
    int count = 0;
    flag=0;

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
        t = clock() - t;
        double time_taken = ((double)t)/CLOCKS_PER_SEC ;
        if(time_taken>time_limit)
           {
        	   printf("\nkilling the child process\n");
        	   *solution=NULL;
        	   free(table);
        	   FILE *fp;

                  fp = fopen(output, "a+");
                  if (fp == NULL)
                      {perror("Unable to open the output file:");}
                  else
                  {
                	  fprintf(fp,"\n%d: NO valid subset found after 1 second",getpid());
                  }
                  fclose(fp);
                  flag=1;
                  free(table);
                  //return 0;
                  break;
           }
    }

if(flag!=1)
{

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
else
    return count;
}

void forkingfunction(int size, int values[],int numarray[],char *output)
{
	int i,j,starting=1,temp=1,status;

	for(i=0;i<size;i++,temp++)
	    {
	    	printf("\nStarting fork for loop i=%d \n",i);

	    	 pid_t pID = vfork();

	    	   if (pID == 0)                // child
	    	   { //clock_t t;
	    	    //t = clock();
	    		   printf("\nchild pid %d\n",getpid());


	    		   int sum=values[starting];
	    		   printf("sum=%d\n",sum);
	    		   starting++;
	    		   int present_array[numarray[temp]];
	    		   printf("values for child:\n");
	    		   for(j=0;j<numarray[temp];j++)
	    		   {
	    			   present_array[j]=values[starting];
	    			   starting++;
	    			   printf("present_array[%d]=%d ",j,present_array[j]);
	    		   }
	    		   const size_t len = sizeof(present_array) / sizeof(present_array[0]);//lenth of present_array
	    		        int *solution;int flag=0;
	    		       const int count = subset_sum(present_array, len, sum, &solution,pID,output,flag);
	    		       if(flag==1)
	    		       {
	    		    	   printf("\nflag=1");
	    		    	   exit(EXIT_SUCCESS);
	    		    	    kill(pID,SIGKILL);
	    		       }

	    		       else
	    		       {
	    		       FILE *fp;

	    		       	    		             fp = fopen(output, "a+");
	    		       	    		             if (fp == NULL)
	    		       	    		                 {perror("Unable to open the output file:");}
	    		       if (count)
	    		       {
	    		           int i;


	    		            	 fprintf(fp,"\n%d:",getpid());

	    		            	 printf("\nlen=%lu\n",len);
	    		           for (i = 0; i < len; i++)
	    		           {
	    		               if (solution[i])
	    		               {
	    		            		   fprintf(fp," %u ", present_array[i]);

	    		               }
	    		           }
	    		           fprintf(fp,"=%d\n",sum);
	    		       }
	    		       else
	    		       {
	    		           fprintf(fp,"\n%d: No subset of numbers summed to %d\n",getpid(),sum);
	    		       }




	    		      exit(EXIT_SUCCESS);
	    		       kill(pID,SIGKILL);
	    		       }
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
void readingfile(char *input,char *output)
{
	 int tek;
	    int size,i,totalsize=0;

	    char c;
    // open file
    FILE *myFile = fopen(input, "r");
    // if opening file fails, print error message and exit 1
    if (myFile == NULL)
    {
        perror("Error: Failed to open file.");

    }
    fscanf(myFile, "%d", &size);
    printf("size %d",size);
int numarray[size];
memset(numarray,0,size*sizeof(int));//contains size of each line

for(i=0;i<=size;i++)
{
	numarray[i]=0;//initializing array values to 0
}
i=0;
    while ((c=getc(myFile)) != EOF)
    {
   	   	  // Increment character count if NOT new line or space
   		    if (c==' ')
   		    {
   		    	printf("\n");
   		    	numarray[i]=numarray[i]+1;
   		    	printf("numarray[%d]=%d",i,numarray[i]);

   		    }
   		    else if(c=='\n')
   		    	{
   		    	i++;
   		    	}

    }
    for(i=1;i<=size;i++)
    {
    	totalsize=totalsize+numarray[i];
    }
    totalsize=totalsize+size;
    printf("\ntotalsize=%d",totalsize);


    fseek(myFile, 0, SEEK_SET);//making the file pointer to beginning of the file

    // read values from file until EOF is returned by fscanf
printf("\n");
int values[totalsize];
memset( values, 0, totalsize*sizeof(int) );
for(i=0;i<totalsize;i++)
{
	values[i]=0;
}
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

    forkingfunction(size,values,numarray,output);

    // close file
    fclose(myFile);
}

int main(int argc, char **argv)
{
    int options;
    bool flagh=false,flagi=false,flago=false,flagt=false;
    char *inputfilename,*outputfilename;
    inputfilename="input.dat";
    outputfilename="output.dat";
    int t=10;
    while((options=getopt(argc,argv,"-:hi::o::t::"))!=-1)
    	    	{
    	    		switch(options)
    	    		{
    	    		case 'h':
    	    			    			printf("Please read readMe.txt to understand how to run the project\n");
    	    			    			flagh=true;
    	    			    			break;
    	    			    			exit(EXIT_SUCCESS);
    	    		case 'i':

    	    			    			flagi=true;
    	    			    			if(optarg)
    	    			    			{
    	    			    				inputfilename=optarg;
    	    			    			}
    	    			    			break;
    	    		case 'o':
    	    						flago=true;
    	    						if(optarg)
    	    						{
    	    							outputfilename=optarg;
    	    						}
    	    						break;
    	    		case 't':
    	    				flagt=true;
    	    				if(optarg)
    	    				{
    	    					t=atoi(optarg);
    	    				}
    	    				break;
    	    		}
    	    	}
    if(flagi||flago||flagt)
    {
    	readingfile(inputfilename,outputfilename);
    }




    return 0;
}
