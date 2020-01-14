//
// Created by athena on 14/1/20.
//
int indexOfOuter = 0;

#include "Perm.h"
void printarray(int arr[], int size, int **setIter, int *outerIndex)
{
    int i,j;
    setIter[*outerIndex] = new int[size];

    for(i=0; i<size; i++)
    {
        printf("%d\t",arr[i]);
        setIter[*outerIndex][i] = arr[i];

    }
    (*outerIndex) =  (*outerIndex)+1;
    printf("\n");
}

//function to swap the variables
void swap(int *a, int *b)
{
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

//permutation function

void permutationCaller(int *arr, int start, int end, int **setIter) {
    int outerIter=0;
    permutation(arr, start, end, setIter, &outerIter);
}

void permutation(int *arr, int start, int end, int **setIter, int *outerIter) {
    if(start==end)
    {
        printarray(arr, end + 1, setIter, outerIter);

        return;
    }
    int i;
    for(i=start;i<=end;i++)
    {
        //swapping numbers
        swap((arr+i), (arr+start));
        //fixing one first digit
        //and calling permutation on
        //the rest of the digits
        permutation(arr, start + 1, end, setIter, outerIter);
        swap((arr+i), (arr+start));
    }
}
