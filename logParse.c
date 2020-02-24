/*
 * PrintingFile.c
 *
 *  Created on: Sep 9, 2019
 *      Author: afrah
 */

# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <time.h>
# include <stdlib.h>
# include <dirent.h>
# include <stdio.h>
# include <string.h>
# include <getopt.h>
# include<stdbool.h>
# include <ctype.h>
# include<sys/wait.h>
# include<signal.h>
# include <sys/mman.h>
# include<sys/time.h>


struct entry {
    unsigned int truth;
    int element;
    unsigned int count;
    struct entry *prev;
};

typedef struct entry entry;

int length;
int PID[1000];//global variable containing PIDs of the child processes alive
static int *counterfork;//global variable shared among child processes and parent to keep track of counter in PID[]
int tclock=10;
FILE *fp;

//functions for the forking function
static int *start;//global variable shared among child processes and parent to keep track of counter in values[] containing all numbers from the file
int temp=1;//We need to start traversing numarray from 1 because line 0 contains number of lines to be processed
int flag=0;//flag for 1 second time out for child processes
void handler(int sig)//SIGTERM handler
{
	printf("\ntimed out");
	sleep(10);//Handler makes the program sleep till it finishes graceful termination
}
void catch_alarm (int sig)//SIGALRM handler
{
	int i;
	for(i=0;i<(length+1);i++)
	{
		if(PID[i]!=0)//if =0 then the child process has been already killed in forkingfunction()

		{   signal(SIGTERM,handler);
			kill (PID[i],SIGTERM);
		}


	}
}
//Part of code taken from http://www.martinbroadhurst.com/subset-sum-using-dynamic-programming-in-c.html
int subset_sum(const int *weights, size_t len, int target,int **solution,pid_t pID,char *output)
{
	clock_t t,end;
	double time_limit=1;//1 second time limit for each child process to finish execution
    t = clock();//start clock to time for 1 second
    entry **table;//2D matrix to fill with sum+1 columns and len+1 rows where len is size of array containing numbers
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
        //chcecking if 1 second has elapsed in filling the table
        end = clock();
         double time_taken = (double)(end-t)/CLOCKS_PER_SEC ;
      //   printf("\ntime_taken=%lf",time_taken);
        if(time_taken>time_limit)
           {
        	   //printf("\nkilling a child process because\n");
        	   *solution=NULL;// no solution after 1 second
        	   free(table);
        	   FILE *fp;

                  fp = fopen(output, "a+");
                  if (fp == NULL)
                      {perror("logParse: Unable to open the output file:");}
                  else
                  {
                	  fprintf(fp,"\n%d: NO valid subset found after 1 second",getpid());
                  }
                  fclose(fp);
                  flag=1;
                  break;
           }
    }

if(flag!=1)//If 1 second has not elapsed we continue
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
            if (head->element != -1)
            {
                (*solution)[head->element]++;
            }
            //checking if 1 second has elapsed in backtracking for the answer
            end = clock();
                     double time_taken = (double)(end-t)/CLOCKS_PER_SEC ;
                  //   printf("\ntime_taken=%lf",time_taken);
                    if(time_taken>time_limit)
                       {
                    	  // printf("\nkilling the child process because of 1 second time out\n");
                    	   *solution=NULL;
                    	   free(table);
                    	   FILE *fp;

                              fp = fopen(output, "a+");
                              if (fp == NULL)
                                  {perror("logParse: Unable to open the output file:");}
                              else
                              {
                            	  fprintf(fp,"\n%d: NO valid subset found after 1 second",getpid());
                              }
                              fclose(fp);
                              flag=1;
                              break;
                       }
        }//end for
    }//endif
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

	 start = mmap(NULL, sizeof *start, PROT_READ | PROT_WRITE,  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	 counterfork = mmap(NULL, sizeof *counterfork, PROT_READ | PROT_WRITE,  MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	 *start=1;
	 *counterfork=0;

	int i,j,status;
	fpos_t pos, pos1;


	      PID[*counterfork]=getpid();//Storing parent pid in global PID[]
	      	    		   *counterfork=*counterfork+1;


	for(i=0;i<size;i++,temp++)
	    {

	    	 pid_t pID = fork();

	    	   if (pID == 0)                // child
	    	   {
	    		  // printf("\nchild pid %d\n",getpid());

	    		   PID[*counterfork]=getpid();
	    		   *counterfork=*counterfork+1;
	    		   int sum=values[*start];
	    		   //printf("sum=%d\n",sum);
	    		   *start=*start+1;
	    		   int present_array[numarray[temp]];
	    		  // printf("values for child:\n");
	    		   for(j=0;j<numarray[temp];j++)
	    		   {
	    			   present_array[j]=values[*start];
	    			   *start=*start+1;
	    			 //  printf("present_array[%d]=%d ",j,present_array[j]);
	    		   }

	    		   const size_t len = sizeof(present_array) / sizeof(present_array[0]);//lenth of present_array
	    		        int *solution;
	    		       const int count = subset_sum(present_array, len, sum, &solution,pID,output);

	    		      if(flag==1)//the child already took 1 second for processing so we exit
	    		      {
	    		    	  int temp=(*counterfork)-1;
	    		    	  PID[temp]=0;

	    		    	  exit(EXIT_SUCCESS);
	    		    	   kill(pID,SIGKILL);
	    		      }

	    		else if(flag==0)//if child has not finished 1 second
	    		{
	    		       FILE *fp;
	    		       fp = fopen(output, "a+");
	    		             if (fp == NULL)
	    		                 {perror("logParse: Unable to open the output file:");}

	    		       if (count)
	    		       {
	    		           int i;


	    		            	 fprintf(fp,"\n%d:",getpid());

	    		            	// printf("\nlen=%lu\n",len);
	    		           for (i = 0; i < len; i++)
	    		           {
	    		               if (solution[i])
	    		               {
	    		            		   fprintf(fp," %u ", present_array[i]);

	    		               }
	    		           }

	    		           fprintf(fp," =%d\n",sum);


	    		       }
	    		       else
	    		       {
	    		           fprintf(fp,"\n%d: No subset of numbers summed to %d\n",getpid(),sum);
	    		       }


	    		       int temp=*counterfork-1;
	    		       PID[temp]=0;//child has finished so we remove the PID and set it to 0

	    		      exit(EXIT_SUCCESS);
	    		       kill(pID,SIGKILL);
	    		}
	    	   }
	    	   else if (pID < 0)            // failed to fork
	    	   {
	    		   perror("logParse: Failed to fork:");
	    	   }
	    	   else if(pID>0)
	    	   {
	    		    int corpse;
		            while ((corpse = waitpid(pID, &status, 0)) != pID && corpse != -1)
		    		   {

		    			char pmsg[64];
		    			snprintf(pmsg, sizeof(pmsg), "logParse: PID %d exited with status 0x%.4X", corpse, status);
		    			perror(pmsg);
		    		   }
	    	   }
	    }//end of for
	printf("\nOutput file updated.");
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
        perror("logParse: Error: Failed to open file.");

    }
    fscanf(myFile, "%d", &size);//size = number of forked children (subset sum problems to be solved)
   // printf("size %d",size);
    length=size;
int numarray[size];// will contain number of integers in each line
memset(numarray,0,size*sizeof(int));//initializing 0s
memset(PID,0,size*sizeof(int));
for(i=0;i<=size;i++)
{
	numarray[i]=0;//initializing array values to 0
}
i=0;
    while ((c=getc(myFile)) != EOF)
    {
   		    if (c==' ')
   		    {
   		    	numarray[i]=numarray[i]+1; //increment counter for each number in a line
   		    }
   		    else if(c=='\n')
   		    {
   		    	i++;//start counting for next line
   		    }

    }
    for(i=1;i<=size;i++)
    {
    	totalsize=totalsize+numarray[i];//total number of integers in the file
    }
    totalsize=totalsize+size;//Example:3 spaces= 4 numbers ,adds 1 for each line which is = size
   // printf("\ntotalsize=%d",totalsize);


    fseek(myFile, 0, SEEK_SET);//making the file pointer to beginning of the file

    // read values from file until EOF is returned by fscanf
//printf("\n");
int values[totalsize];//array cointaining all the numbers in the file
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
           // printf("values[%d]=%d ",i,values[i]);
        } else
        {
            // if EOF is returned by fscanf, or in case of error, break loop
            break;
        }
    }
    //printf("\n");

    forkingfunction(size,values,numarray,output);

    // close file
    fclose(myFile);
}

int main(int argc, char **argv)
{
    int options;
    bool flagh=false,flagi=false,flago=false,flagt=false,df=false;

    char *inputfilename,*outputfilename;
    inputfilename="input.dat";
    outputfilename="output.dat";

    while((options=getopt(argc,argv,"-:hi::o::t::"))!=-1)
    	    	{
    	    		switch(options)
    	    		{
    	    		case 'h':
    	    			    			printf("These are the following commands that the project can do:");
    	    			    			printf("\n-h\n-i , -iinputfilename\n-o , -ooutputfilename\n-t, -t20");
    	    			    			printf("\nIf we do want to specify an input/output file name or time limit (in seconds) "
    	    			    					"there can be no space between the the command (-i/-o/-t) and the file name/time value "
    	    			    					"as shown in the example above If no values are passed along with -i/-o/-t "
    	    			    					"the default values input.dat, output.dat, 10(seconds) will be used\n"
    	    			    					"The program will notify once the output file has been updated with a message or if any error arises.");

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
    	    					tclock=atoi(optarg);//stores time our program should run for default is 10
    	    				}
    	    				break;
    	    		default: 	df=true;
    	    				//	printf("Default case\n");
    	    					break;
    	    		}
    	    	}
    signal (SIGALRM, catch_alarm);
        alarm (tclock);
      //  printf("df flag is set");
    if(flagh==false)
    {
    	if( access( inputfilename, F_OK ) != -1 ) {
    	    fp = fopen(outputfilename, "w+");
    	    if (fp == NULL)
    	        {
    	        	perror("oss: Unable to open the output file:");
    	        }
    	    else
    	    {
    	    	//printf("Yay I opened the o/p file\n");
    	    }

    	    fclose(fp);
    	    fp = fopen(inputfilename, "r+");
    	    if (fp == NULL)
    	        {
    	        	perror("oss: Unable to open the input file:");
    	        }
    	    else
    	    {
    	    	//printf("Yay I opened the i/p file\n");
    	    }

    	    fclose(fp);

        	readingfile(inputfilename,outputfilename);
    	} else {
    	    perror("Input file does not exist!");
    	    exit(0);
    	}

    }
    sleep(2);


    return 0;
}
