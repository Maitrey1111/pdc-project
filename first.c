#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

#define TRUE 1
#define FALSE 0
#define Match 3
#define MissMatch -3
#define Gap -2

FILE *file_str_a, *file_str_b;
int k, l, m, n, length_str_a, length_str_b, box_val;
char dash = '-';

char str_a[5000];
char str_b[5000];
int string_matrix[5001][5001];

int max_box = 0;
int max_box_position[2];

char max_match_a[5000];
char max_match_b[5000];
char matched_string_a[5000];
char matched_string_b[5000];

int MaxAcounter = 1;
int MaxBcounter = 1;
int check;

int Max(int *top, int *left, int *diagonal)
{
	if (*top > *left)
		if (*top > *diagonal)
			return *top;
		else
			return *diagonal;
	else if (*left > *diagonal)
		return *left;
	else
		return *diagonal;
	;
}

int Align(int pos_str_a, int pos_str_b)
{
	int relmax = -1;
	int relmaxpos[2];
	int cont = TRUE;

	if (string_matrix[pos_str_a - 1][pos_str_b - 1] == 0)
		cont = FALSE;

	while (cont == TRUE)
	{
		for (int i = pos_str_a; i > 0; --i)
		{
			if (relmax < string_matrix[i - 1][pos_str_b - 1])
			{
				relmax = string_matrix[i - 1][pos_str_b - 1];
				relmaxpos[0] = i - 1;
				relmaxpos[1] = pos_str_b - 1;
			}
		}

		for (int j = pos_str_b; j > 0; --j)
		{
			if (relmax < string_matrix[pos_str_a - 1][j - 1])
			{
				relmax = string_matrix[pos_str_a - 1][j - 1];
				relmaxpos[0] = pos_str_a - 1;
				relmaxpos[1] = j - 1;
			}
		}

		if ((relmaxpos[0] == pos_str_a - 1) && (relmaxpos[1] == pos_str_b - 1))
		{
			max_match_a[MaxAcounter] = str_a[relmaxpos[0] - 1];
			++MaxAcounter;
			max_match_b[MaxBcounter] = str_b[relmaxpos[1] - 1];
			++MaxBcounter;
		}
		else
		{
			if ((relmaxpos[1] == pos_str_b - 1) && (relmaxpos[0] != pos_str_a - 1))
			{
				for (int i = pos_str_a - 1; i > relmaxpos[0] - 1; --i)
				{
					max_match_a[MaxAcounter] = str_a[i - 1];
					++MaxAcounter;
				}
				for (int j = pos_str_a - 1; j > relmaxpos[0]; --j)
				{
					max_match_b[MaxBcounter] = dash;
					++MaxBcounter;
				}
				max_match_b[MaxBcounter] = str_b[relmaxpos[1] - 1];
				++MaxBcounter;
			}

			if ((relmaxpos[0] == pos_str_a - 1) && (relmaxpos[1] != pos_str_b - 1))
			{
				for (int j = pos_str_b - 1; j > relmaxpos[1] - 1; --j)
				{
					max_match_b[MaxBcounter] = str_b[j - 1];
					++MaxBcounter;
				}

				for (int i = pos_str_b - 1; i > relmaxpos[1]; --i)
				{
					max_match_a[MaxAcounter] = dash;
					++MaxAcounter;
				}
				max_match_a[MaxAcounter] = str_a[relmaxpos[0] - 1];
				++MaxAcounter;
			}
		}

		Align(relmaxpos[0], relmaxpos[1]);
	}

	return (cont);
}

int main()
{
	double s, e;
	s = clock();

	file_str_a = fopen("s1.txt", "r");
	if (file_str_a == NULL)
	{
		printf("Error opening 's1.txt'\n");
		system("PAUSE");
		exit(1);
	}

	file_str_b = fopen("s2.txt", "r");
	if (file_str_b == NULL)
	{
		printf("Error opening 's2.txt'\n");
		system("PAUSE");
		exit(1);
	}

	fgets(str_a, sizeof(str_a), file_str_a);
	fclose(file_str_a);

	fgets(str_b, sizeof(str_b), file_str_b);
	fclose(file_str_b);

	length_str_a = strlen(str_a);
	length_str_b = strlen(str_b);

	for (int i = 0; i <= length_str_a; i++)
		string_matrix[0][i] = 0;

	for (int i = 0; i <= length_str_b; i++)
		string_matrix[i][0] = 0;

// parallelization of for loop to minimizethe time if done serially
	box_val = 0;
	#pragma omp parallel for
	for (int i = 1; i <= length_str_a; i++)
	{
		#pragma omp parallel for
		for (int j = 1; j <= length_str_b; j++)
		{
			int top = (string_matrix[i - 1][j]) + Gap;
			int left = (string_matrix[i][j - 1]) + Gap;
			int diagonal;
			if (str_a[i - 1] == str_b[j - 1])
				diagonal = string_matrix[i - 1][j - 1] + Match;
			else
				diagonal = string_matrix[i - 1][j - 1] + MissMatch;

			box_val = Max(&top, &left, &diagonal);

			if (box_val < 0)
				box_val = 0;

			string_matrix[i][j] = box_val;
			box_val = 0;
		}
	}

	#pragma omp parallel for
	for (int i = 0; i <= length_str_a; i++)
	{
		#pragma omp parallel for
		for (int j = 0; j <= length_str_b; j++)
		{
			if (string_matrix[i][j] > max_box)
			{
				max_box = string_matrix[i][j];
				max_box_position[0] = i;
				max_box_position[1] = j;
			}
		}
	}

	// maximum box_val found for this character 
	max_match_a[0] = str_a[max_box_position[0] - 1];

	// maximum box_val found for this character 
	max_match_b[0] = str_b[max_box_position[1] - 1];

	check = Align(max_box_position[0], max_box_position[1]);
	//position-0 is: 'i' i.e. position of string-A
	//position-1 is: 'j' i.e. position of string-B

	int k = 0;
	for (int i = strlen(max_match_a) - 1; i > -1; --i){
		matched_string_a[k] = max_match_a[i];
		++k;
	}

	k = 0;
	for (int j = strlen(max_match_b) - 1; j > -1; --j)
	{
		matched_string_b[k] = max_match_b[j];
		++k;
	}

	printf("\n%s\n%s", matched_string_a, matched_string_b);

	e = clock();
	printf("\n\n%lf", (double)(e - s) / (double)CLOCKS_PER_SEC);

	return 0;
}