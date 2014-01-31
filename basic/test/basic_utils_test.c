#include <stdio.h>
#include <string.h>
#include <basic_utils.h>

int main() {
	char test_str1[] = "abcd1234lask5oaiegfn30$*&^()_  (\n(*$\t345";
	char test_str2[] = "";
	char test_str3[] = "12345678abcdefgh87654321qwertyui";

	test_str1[4] = 3;
	test_str1[5] = 130;
	test_str1[10] = 31;
	test_str1[19] = 127;

	printf("\nNote: this is a manual (or visual) test, not an automatic one.\n");
	printf("\nTest string 1:\n");
	dump(test_str1, strlen(test_str1));
	printf("\nTest string 2:\n");
	dump(test_str2, strlen(test_str2));
	printf("\nTest string 3:\n");
	dump(test_str3, strlen(test_str3));
	printf("\nDone\n");

	return 0;
}