set(base_SRCS
  TimeStamp.cc
  Date.cc
  TimeZone.cc
  CurrentThread.cc
  Exception.cc
  Condition.cc
  CountDownLatch.cc
  Thread.cc
  ThreadPool.cc
  FileUtil.cc
  ProcessInfo.cc
  LogStream.cc
  LogFile.cc
  Logging.cc
  AsyncLogging.cc
  )

add_library(muduo_base ${base_SRCS})
target_link_libraries(muduo_base pthread rt)


install(TARGETS muduo_base DESTINATION lib)
#install(TARGETS muduo_base_cpp11 DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/muduo/base)

add_subdirectory(tests)

