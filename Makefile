test_file_manager: test_file_manager.c file_manager.c block.c page.c
	gcc -Wall -o test_file_manager test_file_manager.c file_manager.c block.c page.c
test_block: test_block.c block.c
	gcc -Wall -o test_block test_block.c block.c