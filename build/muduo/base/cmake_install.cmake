# Install script for directory: /devdata/liuchuanyang/muduo/muduo/base

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/devdata/liuchuanyang/muduo/build/lib/libmuduo_base.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/muduo/base" TYPE FILE FILES
    "/devdata/liuchuanyang/muduo/muduo/base/Atomic.h"
    "/devdata/liuchuanyang/muduo/muduo/base/Condition.h"
    "/devdata/liuchuanyang/muduo/muduo/base/Copyable.h"
    "/devdata/liuchuanyang/muduo/muduo/base/CountDownLatch.h"
    "/devdata/liuchuanyang/muduo/muduo/base/CurrentThread.h"
    "/devdata/liuchuanyang/muduo/muduo/base/Date.h"
    "/devdata/liuchuanyang/muduo/muduo/base/Exception.h"
    "/devdata/liuchuanyang/muduo/muduo/base/FileUtil.h"
    "/devdata/liuchuanyang/muduo/muduo/base/Mutex.h"
    "/devdata/liuchuanyang/muduo/muduo/base/NonCopyable.h"
    "/devdata/liuchuanyang/muduo/muduo/base/ProcessInfo.h"
    "/devdata/liuchuanyang/muduo/muduo/base/Singleton.h"
    "/devdata/liuchuanyang/muduo/muduo/base/StringPiece.h"
    "/devdata/liuchuanyang/muduo/muduo/base/Thread.h"
    "/devdata/liuchuanyang/muduo/muduo/base/ThreadLocal.h"
    "/devdata/liuchuanyang/muduo/muduo/base/ThreadLocalSingleton.h"
    "/devdata/liuchuanyang/muduo/muduo/base/ThreadPool.h"
    "/devdata/liuchuanyang/muduo/muduo/base/TimeStamp.h"
    "/devdata/liuchuanyang/muduo/muduo/base/TimeZone.h"
    "/devdata/liuchuanyang/muduo/muduo/base/Types.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/devdata/liuchuanyang/muduo/build/muduo/base/tests/cmake_install.cmake")

endif()

