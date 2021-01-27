test_file_manager: test_file_manager.c file_manager.c block.c page.c
	gcc -Wall -g -o test_file_manager test_file_manager.c file_manager.c block.c page.c
	rm -rf tmp_test_log_manager

test_block: test_block.c block.c
	gcc -Wall -g -o test_block test_block.c block.c
	rm -rf tmp_test_log_manager

test_log_manager: test_log_manager.c log_manager.c file_manager.c block.c page.c
	gcc -Wall -g -o test_log_manager test_log_manager.c log_manager.c file_manager.c block.c page.c
	rm -rf tmp_test_log_manager

test_buffer: test_buffer.c buffer.c log_manager.c file_manager.c block.c page.c
	gcc -Wall -g -o test_buffer test_buffer.c buffer.c log_manager.c file_manager.c block.c page.c
	rm -rf tmp_test_buffer

test_buffer_manager: test_buffer_manager.c buffer_manager.c buffer.c log_manager.c file_manager.c block.c page.c
	gcc -Wall -g -o test_buffer_manager test_buffer_manager.c buffer_manager.c buffer.c log_manager.c file_manager.c block.c page.c
	rm -rf tmp_test_buffer_manager