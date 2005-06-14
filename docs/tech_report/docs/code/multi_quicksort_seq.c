
/* fill the lists, one pivot group at a time */
for(i = 0; i < N; i++)
{
	if ((i % (C_DIV_3) == 0) && (i != 0)) continue;

	v = a[i];
	l = 0;
	r = pivot_count+1;

	for(k = 0; k < pivot_count+1; k++)
	{
		if(v < pivots[k])
		{
			add_to_list(k, v);
			break;
		}
	}
}
