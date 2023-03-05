#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<omp.h>
#include<time.h>

#define TRUE 1
#define FALSE 0

#define Match 3
#define MissMatch -3
#define Gap -2
	
FILE *ptr_file_1, *ptr_file_2;	
int i,j,k, len_str_a,len_str_b, box_val;
char dash = '-';

char str_a[5000];			
char str_b[5000];
int max_box_val = 0;
int max_box_val_pos[2];		
int str_matrix[5001][5001];	

char matches_a[5000];
char matches_b[5000];
char corrected_a[5000];
char corrected_b[5000];

int str_a_counter = 1;	
int str_b_counter = 1;	
int cont = TRUE;
int check;

int Align(int PosA,int PosB)
{
	int relmax = -1;		
	int relmaxpos[2];
	if(str_matrix[PosA-1][PosB-1] == 0)
		cont = FALSE;

	while(cont == TRUE)
	{	
		for(i=PosA; i>0; --i)
		{
			if(relmax < str_matrix[i-1][PosB-1])
			{
				relmax = str_matrix[i-1][PosB-1];
				relmaxpos[0]=i-1;
				relmaxpos[1]=PosB-1;
			}
		}
	
		for(j=PosB; j>0; --j)
		{

			if(relmax < str_matrix[PosA-1][j-1])
			{
				relmax = str_matrix[PosA-1][j-1];
				relmaxpos[0]=PosA-1;
				relmaxpos[1]=j-1;
			}
		}

		if((relmaxpos[0] == PosA-1) && (relmaxpos[1] == PosB-1))
		{

			matches_a[str_a_counter] = str_a[relmaxpos[0]-1];
			++str_a_counter;
			matches_b[str_b_counter] = str_b[relmaxpos[1]-1];
			++str_b_counter;

		}
		else
		{
			if((relmaxpos[1] == PosB-1) && (relmaxpos[0] != PosA-1))
			{		
				for(i=PosA-1; i>relmaxpos[0]-1; --i)
				{	
					matches_a[str_a_counter]= str_a[i-1];
					++str_a_counter;
				}
				
				for(j=PosA-1; j>relmaxpos[0]; --j)
				{
					matches_b[str_b_counter] = dash;
					++str_b_counter;
				}

				matches_b[str_b_counter] = str_b[relmaxpos[1]-1];	
				++str_b_counter;
			}

			if((relmaxpos[0] == PosA-1) && (relmaxpos[1] != PosB-1))
			{	
			
				for(j=PosB-1; j>relmaxpos[1]-1; --j)
				{	
					matches_b[str_b_counter] = str_b[j-1];
					++str_b_counter;
				}
			
				for(i=PosB-1; i>relmaxpos[1]; --i)
				{		
					matches_a[str_a_counter] = dash;
					++str_a_counter;
				}
				matches_a[str_a_counter] = str_a[relmaxpos[0]-1];
				++str_a_counter;
			}
		}

		Align(relmaxpos[0],relmaxpos[1]);
	}
	return(cont);
}


int main()
{
    double s,e;
	s=clock();
    
    ptr_file_1 = fopen("s2.txt", "r");
    
    if(ptr_file_1 == NULL)
    {
        printf("Error opening 's2.txt'\n");
        system("PAUSE");
        exit(1);
    }
   
    ptr_file_2 = fopen("s1.txt", "r");
   
    if(ptr_file_2 == NULL)
    {
        printf("Error opening 's1.txt'\n");
        system("PAUSE");
        exit(1);
    }
 
    fgets(str_a, sizeof(str_a), ptr_file_1);
    fgets(str_b, sizeof(str_b), ptr_file_2);
    fclose(ptr_file_1);
    fclose(ptr_file_2);
	len_str_a = strlen(str_a);
	len_str_b = strlen(str_b);

	for(i=0;i<=len_str_a;++i)
	{
		str_matrix[0][i]=0;
	}
	for(i=0;i<=len_str_b;++i)
	{
		str_matrix[i][0]=0;
	}

	box_val = 0;
	//parallelization of for loop to minimizethe time if done serially 
	#pragma omp parallel for 
	for(i = 1; i <= len_str_a; ++i)
	{	
	    #pragma omp parallel for 
	    for(j = 1; j <= len_str_b; ++j)
	    {	
			if(str_a[i-1] == str_b[j-1])
			{				
				box_val = (str_matrix[i-1][j-1] + Match);	

			}
			if(box_val < ((str_matrix[i-1][j]) + Gap))
			{	    
				box_val = ((str_matrix[i-1][j]) + Gap);		
			}
			

			if(box_val<(str_matrix[i][j-1]+Gap))
			{	
			    box_val=((str_matrix[i][j-1])+Gap);   
			}
		

			if(box_val < 0)
			{
				box_val = 0;
			}

			if(str_a[i-1] != str_b[j-1])
			{					

				if(box_val < (str_matrix[i-1][j-1] + MissMatch))
				{	
					box_val = str_matrix[i-1][j-1] + MissMatch;
                		}

				if(box_val < ((str_matrix[i-1][j]) + Gap))
				{	   

					box_val = ((str_matrix[i-1][j]) + Gap);	
				}
	

				if(box_val < ((str_matrix[i][j-1]) + Gap))
				{	

					box_val = ((str_matrix[i][j-1]) + Gap);    
				}
				if(box_val < 0)
				{
					box_val = 0;
				}
			}
			str_matrix[i][j] = box_val;
			box_val = 0;
		}
	}

	for(i=0; i<=len_str_a; ++i)
	{
	    for(j=0; j<=len_str_b; ++j)
	    {
	    	if(str_matrix[i][j] > max_box_val)
	        {
			max_box_val = str_matrix[i][j];
			max_box_val_pos[0]=i;
			max_box_val_pos[1]=j;
		}
	    }
	}
	
	matches_a[0] = str_a[max_box_val_pos[0]-1];
	matches_b[0] = str_b[max_box_val_pos[1]-1];

	check = Align(max_box_val_pos[0],max_box_val_pos[1]);

	k=0;
	for(i = strlen(matches_a)-1; i > -1; --i)
	{
		corrected_a[k] = matches_a[i];
		++k;
	}

	k=0;
	for(j=strlen(matches_b)-1; j > -1; --j)
	{
		corrected_b[k] = matches_b[j];
		++k;
	}

	printf("%s\n%s	",corrected_a,corrected_b);
	e=clock();
	printf("\n\n%lf",(double)(e-s)/(double)CLOCKS_PER_SEC);
	return(0);
}