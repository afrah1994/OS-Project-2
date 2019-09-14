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
//code for subset sum problem copied from http://www.martinbroadhurst.com/subset-sum-using-dynamic-programming-in-c.html
struct entry {
    unsigned int truth;
    int element;
    unsigned int count;
    struct entry *prev;
};
typedef struct entry entry;

unsigned int subset_sum(const unsigned int *weights, size_t len, unsigned int target,
        unsigned int **solution)
{
    entry **table;
    unsigned int i, j;
    entry *head;
    unsigned int count = 0;

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
        *solution = calloc(len, sizeof(unsigned int));
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
int main(void)
{
    unsigned int weights[] = {3, 34, 4, 12,5,2};
    unsigned int target = 9;
    const size_t len = sizeof(weights) / sizeof(weights[0]);
    unsigned int *solution;
    const unsigned int count = subset_sum(weights, len, target, &solution);
    if (count) {
        unsigned int i;
        printf("Found a solution\n");
        printf("Elements:\n");
        for (i = 0; i < len; i++) {
            if (solution[i]) {
                printf("%u ", weights[i]);
            }
        }
        putchar('\n');
    }
    else {
        printf("No solution\n");
    }
    free(solution);

    return 0;
}
