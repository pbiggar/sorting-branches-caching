#include <stdio.h>
#include <sys/types.h>
#include <sys/times.h>
#include <math.h>
#define maxN 1000000

typedef long itemType;

#define randomLong lrand48()

#define less(A, B)  (A < B)
#define exch(A, B) { itemType t = A; A = B; B = t; } 

#define IENN 8

void showItem(itemType x)
  { printf("%11d ", x); }  
void showArray(itemType a[], int l, int r)
  { int i;
    for (i = l; i <= r; i++)
      if ((r-l < IENN) || (i-l+1 < IENN/2) || (r-i+1 < IENN/2)) 
        showItem(a[i]); 
      else if (i == l+IENN/2-1) printf ("... ");  
    printf("\n");  
  }  

void randArray(itemType a[], int N)
  { int i;
    for (i = 0; i < N; i++) a[i] = randomLong;
  }

void copyArray(itemType a[], itemType b[], int N)
  { int i;
    for (i = 0; i < N; i++) { a[i] = b[i]; } 
  }

void shellsortSh(itemType a[], int l, int r)
  { int i, j, h; 
    for (h = 1; h <= (r-l)/4; h = 2*h) ;
    for ( ; h > 0; h /= 2)
      for (i = l+h; i <= r; i++)
        { int j = i; itemType v = a[i]; 
          while (j >= l+h && less(v, a[j-h]))
            { a[j] = a[j-h]; j -= h; }
          a[j] = v; 
        } 
  }

void shellsortKn(itemType a[], int l, int r)
  { int i, j, h; 
    for (h = 1; h <= (r-l)/9; h = 3*h+1) ;
    for ( ; h > 0; h /= 3)
      for (i = l+h; i <= r; i++)
        { int j = i; itemType v = a[i]; 
          while (j >= l+h && less(v, a[j-h]))
            { a[j] = a[j-h]; j -= h; }
          a[j] = v; 
        } 
  }

void shellsortGo(itemType a[], int l, int r)
  { int i, j, h; 
    for (h = r-l+1; h > 0; h = ((h > 1) && (h < 5)) ? 1 : 5*h/11)
      for (i = l+h; i <= r; i++)
        { int j = i; itemType v = a[i]; 
          while (j >= l+h && less(v, a[j-h]))
            { a[j] = a[j-h]; j -= h; }
          a[j] = v; 
        } 
  }

void shellsortSe(itemType a[], int l, int r)
  { 
    int i, j, h, t; itemType v;

    for (t = 1; 4*t*t <= (r-l); t += t) ;

    for ( h = (r-l)/4; t > 0; t /= 2, h = t*t - (3*t)/2 + 1)
      for (i = l+h; i <= r; i++)
        { 
          v = a[i]; j = i;
          while (j >= h && less(v, a[j-h]))
            { a[j] = a[j-h]; j -= h; }
          a[j] = v; 
        } 
  }

void shellsortPr(itemType a[], int l, int r)
  { 
    int incs[28] = { 262144, 229376, 200704, 175616, 153664, 134456, 117649,
                     32768, 28672, 25088, 21952, 19208, 16807,
                     4096, 3584, 3136, 2744, 2401, 512, 448, 392, 343, 
                     64, 56, 49, 8, 7, 1};
    int i, j, k, h, t; itemType v;

    for ( k = 0; k < 28; k++)
      for (h = incs[k], i = l+h; i <= r; i++)
        { 
          v = a[i]; j = i;
          while (j >= h && less(v, a[j-h]))
            { a[j] = a[j-h]; j -= h; }
          a[j] = v; 
        } 
  }


void shellsortIS(itemType a[], int l, int r)
  { 
    int incs[16] = { 1391376, 463792, 198768, 86961, 33936, 13776, 
                     4592, 1968, 861, 336, 112, 48, 21, 7, 3, 1 };
    int i, j, k, h, t; itemType v;

    for ( k = 0; k < 16; k++)
      for (h = incs[k], i = l+h; i <= r; i++)
        { 
          v = a[i]; j = i;
          while (j >= h && less(v, a[j-h]))
            { a[j] = a[j-h]; j -= h; }
          a[j] = v; 
        } 
  }

// Driver code
struct tms buffer; 

long doit(void (*sortprog)(), itemType a[], int l, int r)
  {
    int i; long t;
    times(&buffer);
    t = buffer.tms_utime + buffer.tms_stime;
    (*sortprog)(a, l, r);
    times(&buffer);
    for (i = l; i < r; i++)
      if (less(a[i+1], a[i])) return -1;
    return buffer.tms_utime + buffer.tms_stime - t;
  }

main(int argc, char *argv[])
  { itemType *a, *b;
    int N; int debug = atoi(argv[--argc]);
    int c1, c2, c3, c4, c5, c6;

    a = malloc(maxN*sizeof(itemType));
    b = malloc(maxN*sizeof(itemType));

    printf("    N       O      K      G      S      P      I     \n");
    for (N = 12500; N <= 800000; N *= 2)
      {
        randArray(b, N);
        if (N > 200000) c1 = 0; else
          { copyArray(a, b, N); c1 = doit(shellsortSh, a, 0, N-1); }
        copyArray(a, b, N); c2 = doit(shellsortKn, a, 0, N-1);
        copyArray(a, b, N); c3 = doit(shellsortGo, a, 0, N-1);
        copyArray(a, b, N); c4 = doit(shellsortSe, a, 0, N-1);
        copyArray(a, b, N); c5 = doit(shellsortPr, a, 0, N-1);
        copyArray(a, b, N); c6 = doit(shellsortIS, a, 0, N-1);
        printf("%6d %6d %6d %6d %6d %6d %6d\n", N, c1, c2, c3, c4, c5, c6);
      }

    printf("\n");
    printf("           O:  Shell's original\n");
    printf("           K:  Knuth\n");
    printf("           G:  Gonnet\n");
    printf("           S:  Sedgewick\n");
    printf("           P:  Pratt\n");
    printf("           I:  Incerpi-Sedgewick\n");
  }
