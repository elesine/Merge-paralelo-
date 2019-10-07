#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include<mutex>
using namespace std;
mutex io_mutex;

#include <ctime> 
 
unsigned t0, t1;

bool orden(vector<int> arrays)
{
    int tam=arrays.size();
    for (int i=0; i<tam-1 ; ++i)
        if (arrays[i] > arrays [i+1])
            return false;
    return true;
}

void mezcla (vector<int> &A, int i, int m, int f)
{
    int* tmp = new int[f-i+1];
    int p1 = i, p2 = m+1, pos = 0;

    while(p1 <= m && p2 <=f)
    {
        if(A[p1] > A[p2])
            tmp[pos++] = A[p2++];
        else
            tmp[pos++] = A[p1++];
    }

    if(p1 == m+1)
        while(p2 <= f)
            tmp[pos++] = A[p2++];
    else
        while(p1 <= m)
            tmp[pos++] = A[p1++];
    pos = 0;
    while(i<=f)
        A[i++] = tmp[pos++];

    delete[] tmp;
}
void mergeSort(vector<int> &vec, int ini, int fin)
{
    if(ini < fin)
    {
        int m = (ini + fin) >> 1;
        mergeSort(vec, ini, m);
        mergeSort(vec, m+1, fin);
        mezcla(vec, ini, m, fin);
    }
}
void mergeSortParalelo1(vector<int> &A, int ini, int fin)
{
    int NTHREADS = thread::hardware_concurrency(); // numero maximo de threads que tiene el computador = 12
    thread* vec_ths = new thread[NTHREADS];

    int i,j;
    int bloques=(fin+1)/NTHREADS;
    for (i = ini,  j = 0 ; j < NTHREADS-1 ; i = i + bloques, j++)
	vec_ths[j] = thread(mergeSort, ref(A), i, i+bloques-1);
    vec_ths[NTHREADS-1] = thread(mergeSort, ref(A), i, fin);
    for (i = 0; i < NTHREADS; i++)
	vec_ths[i].join();

    int bloquex2;
    bloquex2=bloques<<1;

    int k=0;
    for ( k; k < 5; k++)
    {
        vec_ths[k]=thread(mezcla, ref(A), (k*bloquex2), (k*bloquex2)+bloques -1,  (k*bloquex2)+bloquex2-1);
    }
    vec_ths[k]=thread(mezcla, ref(A), (k*bloquex2), (k*bloquex2)+bloques-1, fin);

    for (i = 0; i < 6; i++)
	vec_ths[i].join();	

    k=0;
    bloques=bloquex2;
    bloquex2=bloques<<1;
    for ( k; k < 2; k++)
    {
        mezcla(A, (k*bloquex2), (k*bloquex2)+bloques -1,  (k*bloquex2)+bloquex2-1);
    }
    mezcla(A, (k*bloquex2), (k*bloquex2)+bloques-1, fin);
    bloques=bloquex2;
    bloquex2=bloques<<1;
    k=0;
    mezcla(A, 0, bloques -1, bloquex2-1);

    mezcla(A, 0, (k*bloquex2)+bloquex2-1, fin);
}

void mergeSortParalelo2(vector<int> &vec, int ini, int fin)
{
    if(ini < fin)
    {
        int m = (ini + fin) >> 1;
        thread th(mergeSort, ref(vec), ini, m);
        thread th1(mergeSort, ref(vec), m+1, fin);
 	if(th.joinable()&&th1.joinable()) {
 	       th.join();
 	       th1.join();
	    }

        mezcla(vec, ini, m, fin);
    }
}
void llenarVectorAleatorio(vector<int> &A, int n)
{
    for( int i=1 ; i <= n ; i++)
        A.push_back(i);
    random_shuffle ( A.begin(), A.end() );
}
int main()
{
    double time; 
    vector<int> A;
    llenarVectorAleatorio(A, 100);
    for(auto r:A)
        cout<<r<<"  ";
    t0=clock();
    mergeSort(A,0,A.size()-1);
    t1 = clock();
    time = (double(t1-t0)/CLOCKS_PER_SEC);
    cout << "Execution Time:mergesort normal " << time << endl;
    cout <<"ordenado por mergesort"<<endl;
  //  for(auto r:A)
  //      cout<<r<<"  ";

    random_shuffle ( A.begin(), A.end() ); //aleatorio
    t0=clock();
    mergeSortParalelo1(A,0,A.size()-1);
    t1 = clock();
    time = (double(t1-t0)/CLOCKS_PER_SEC);
    cout << "Execution Time:mergeSortParalelo1 " << time << endl;
    cout <<"ordenado por mergeSort Paralelo 1 forma: "<<endl;
  //  for(auto r:A)
  //      cout<<r<<"  ";


    random_shuffle ( A.begin(), A.end() ); //aleatorio
    t0=clock();
    mergeSortParalelo2(A,0,A.size()-1);
    t1 = clock();
    time = (double(t1-t0)/CLOCKS_PER_SEC);
    cout << "Execution Time:mergeSortParalelo2 " << time << endl;
    cout <<"ordenado mergesort paralelo 2 forma:"<<endl;
  //  for(auto r:A)
  //      cout<<r<<"  ";

    return 0;
}
