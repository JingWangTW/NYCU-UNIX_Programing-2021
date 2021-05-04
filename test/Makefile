all:
	gcc normal_test.c -Wall -Werror -o normal
	gcc close_stderr_test.c -Wall -Werror -o close_stderr
	strip normal
	strip close_stderr

clean:
	rm -f normal
	rm -f close_stderr
	rm -f *out*
