all:
	gcc normal_test.c -Wall -Werror -o normal
	gcc close_stderr_test.c -Wall -Werror -o close_stderr
	gcc -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 normal_test.c -Wall -Werror -o test_64
	strip normal
	strip close_stderr
	strip test_64

clean:
	rm -f normal
	rm -f close_stderr
	rm -f test_64
	rm -f *out*
