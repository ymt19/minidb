main: main.c block.c buffer.c buffer_list.c buffer_manager.c field_list.c file_manager.c int_list.c layout.c log.c log_manager.c page.c record_id.c record_page.c recovery_manager.c schema.c string_int_list.c table_iterator.c table_manager.c transaction.c
	gcc -Wall -g -o tmp main.c block.c buffer.c buffer_list.c buffer_manager.c field_list.c file_manager.c int_list.c layout.c log.c log_manager.c page.c record_id.c record_page.c recovery_manager.c schema.c string_int_list.c table_iterator.c table_manager.c transaction.c

main2: main2.c block.c buffer.c buffer_list.c buffer_manager.c field_list.c file_manager.c int_list.c layout.c log.c log_manager.c page.c record_id.c record_page.c recovery_manager.c schema.c string_int_list.c table_iterator.c table_manager.c transaction.c
	gcc -Wall -g -o tmp2 main2.c block.c buffer.c buffer_list.c buffer_manager.c field_list.c file_manager.c int_list.c layout.c log.c log_manager.c page.c record_id.c record_page.c recovery_manager.c schema.c string_int_list.c table_iterator.c table_manager.c transaction.c

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

test_recovery_manager: test_recovery_manager.c recovery_manager.c log.c int_list.c transaction.c buffer_list.c buffer_manager.c buffer.c log_manager.c file_manager.c block.c page.c
	gcc -Wall -g -o test_recovery_manager test_recovery_manager.c recovery_manager.c log.c int_list.c transaction.c buffer_list.c buffer_manager.c buffer.c log_manager.c file_manager.c block.c page.c
	rm -rf tmp_test_recovery_manager

test_buffer_list: test_buffer_list.c buffer_list.c buffer_manager.c buffer.c log_manager.c file_manager.c block.c page.c
	gcc -Wall -g -o test_buffer_list test_buffer_list.c buffer_list.c buffer_manager.c buffer.c log_manager.c file_manager.c block.c page.c
	rm -rf tmp_test_buffer_list
