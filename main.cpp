#include <iostream>
#include <stdio.h>
#include <stdlib.h>
//pthread library
#include <pthread.h>
//for floor function in sort
#include <math.h>
using namespace std;

#define NUM_THREADS 3

//original array
int OG[] = {7,12,19, 15, 3,18,4,2,6,15, 22, 15, 35};
int size = 13;
int mid = floor((size-1)/2);
//merged array after sort
int merged[] = {};

//struct that helps sorting function identify which
//thread to re-direct to which sorting function
typedef struct _thread_data_t{
    int tid;
  }  thread_data_t;

//used in quick sort partition
void swap(int* a, int* b)
{
  int t = *a;
  *a = *b;
  *b = t;
}

//used for quick sort
int partition(int arr[], int low, int high){
  int pivot = arr[high];

  int i = low -1;

  for(int j = low; j<= high-1; j++)
  {
    if(arr[j] <= pivot)
    {
      i++;
      swap(&arr[i], &arr[j]);
    }
  }
  swap(&arr[i+1], &arr[high]);
  return (i+1);
}

//sorting algorithm for each half
void quickSort(int arr[], int low, int high){
  if(low<high){
    int pi = partition(arr, low, high);

    quickSort(arr, low, pi-1);
    quickSort(arr, pi+1, high);
  }
}

//sorting algorithm to merge the two halves
void merge(int OGarr[], int mergeArr[], int l, int m, int r){
  int i, j, k;
  int n1 = m-l+1;
  int n2 = r-m;

  int L[n1], R[n2];

  for(i = 0; i<n1; i++){
    L[i] = OGarr[l+i];
  }

  for(j = 0; j<n2; j++){
    R[j] = OGarr[m+1+j];
  }

  i = 0;
  j=0;
  k=l;

  while(i<n1 && j<n2)
  {
    if(L[i]<=R[j])
    {
      mergeArr[k] = L[i];
      i++;
    }
    else{
      mergeArr[k] = R[j];
      j++;
    }
    k++;
  }

  while(i<n1)
  {
    mergeArr[k] = L[i];
    i++;
    k++;
  }

  while(j<n2)
  {
    mergeArr[k] = R[j];
    j++;
    k++;
  }
}

//all threads get passed to this fxn and re-directed to apporpriate algorithm
void *sort(void *arg){
  thread_data_t *data = (thread_data_t *)arg;
  if(data->tid == 0){
    quickSort(OG, 0, mid);
  } else if (data->tid == 1){
    quickSort(OG, mid+1, size-1);
  } else if (data->tid == 2){
    merge(OG, merged, 0, mid, size-1);
  }
  pthread_exit(NULL);
}

int main(int argc, char **argv){
  //edge case
  if(size == 0){
    cout<<"no values in array \n";
    return 0;
  }else{
    cout<<"Original array: ";
    for(int x =0; x<size; x++){
      cout<<OG[x]<<" ";
    }
    cout<<"\n";
    cout<<"************\n";
    pthread_t thr[NUM_THREADS];
    int i;
    thread_data_t thr_data[NUM_THREADS];

    //create two sorting threads first
    for (i = 0; i<2; ++i){
      thr_data[i].tid = i;
      pthread_create(&thr[i], NULL, sort, &thr_data[i]);
    }

    for(i = 0; i<2; ++i){
      pthread_join(thr[i], NULL);
    }

    cout<<"Array with sorted halves (after sorting threads): "<<"\n";
    for(int x =0; x<size; x++){
      cout<<OG[x]<<" ";
    }
    cout<<"\n";
    cout<<"************\n";

    //then create merge thread
    thr_data[2].tid = 2;
    pthread_create(&thr[2], NULL, sort, &thr_data[2]);


    pthread_join(thr[2], NULL);
    cout<<"merged array: ";
    for(int x =0; x<size; x++){
      cout<<merged[x]<<" ";
    }
    cout<<"\n";
    return EXIT_SUCCESS;
  }
}
