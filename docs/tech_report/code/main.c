test_array(const char* description) {
	if (memcmp(sorted_array, random_array2,
				RANDOM_SIZE * sizeof(Item)) == 0)
		printf("%s is sorted\n\n", description);
	else {
		printf("Not sorted:\n");
		print_int_array(random_array2, RANDOM_SIZE,
				description, -1, -1, -1);
		printf("\n\nWhat it should be:\n");
		print_int_array(sorted_array, RANDOM_SIZE,
				"Sorted array", -1, -1, -1);
	}
}

time_sort(void sort(Item*, int), char* description) {
	memcpy(random_array2, random_array, 
			RANDOM_SIZE * sizeof(int));
	start_timer();
	sort(random_array2, RANDOM_SIZE);
	stop_timer();
	print_timer(description);
	test_array(description);
}

test_sort(void sort(Item*, int), char* description) {
	for(int i = 8; i < 256; i++) {
		int* a = malloc(i * sizeof(Item));
		int* b = malloc(i * sizeof(Item));
		fill_random_array(a, i, i);
		memcpy(b, a, i * sizeof(Item));
		qsort((void*)a, i, sizeof(Item), compare_ints);
		sort(b, i);
		if (memcmp(a, b, i * sizeof(Item)) != 0) {
			printf("%s is not sorted (size == %d):\n", 
					description, i);
			print_int_array(b, i, description, -1, -1, -1);
			printf("\n\nWhat it should be:\n");
			print_int_array(a, i, "Sorted array", -1, -1, -1);
			exit(-1);
		}
		free(a); free(b);
	}
	printf("Testing of %s complete\n", description);
}
test_sort(insertsort, "O(N squared) Insertion");
time_sort(insertsort, "O(N squared) Insertion");
