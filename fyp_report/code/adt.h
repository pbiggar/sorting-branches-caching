#define Item unsigned int
#define key(A) (A)
#define less(A,B) (key(A) < key(B))
#define exch(A, B)
do	{
	unsigned int t = (A);
 	(A) = (B);
	(B) = t;
}
while(0)
