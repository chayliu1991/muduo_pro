add_executable(time_stamp_test time_stamp_test.cc)
target_link_libraries(time_stamp_test muduo_base)

add_executable(date_test date_test.cc)
target_link_libraries(date_test muduo_base)

add_executable(time_zone_test time_zone_test.cc)
target_link_libraries(time_zone_test muduo_base)


add_executable(current_thread_test current_thread_test.cc)
target_link_libraries(current_thread_test muduo_base)

add_executable(exception_test exception_test.cc)
target_link_libraries(exception_test muduo_base)

add_executable(atomic_test atomic_test.cc)
target_link_libraries(atomic_test muduo_base)

add_executable(thread_test thread_test.cc)
target_link_libraries(thread_test muduo_base)

add_executable(thread_local_test thread_local_test.cc)
target_link_libraries(thread_local_test muduo_base)

add_executable(singleton_thread_test singleton_thread_test.cc)
target_link_libraries(singleton_thread_test muduo_base)

add_executable(thread_pool_test thread_pool_test.cc)
target_link_libraries(thread_pool_test muduo_base)

add_executable(file_util_test file_util_test.cc)
target_link_libraries(file_util_test muduo_base)

add_executable(process_info_test process_info_test.cc)
target_link_libraries(process_info_test muduo_base)
