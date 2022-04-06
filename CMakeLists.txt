cmake_minimum_required(VERSION 3.16)
project(anet)

set(CMAKE_CXX_STANDARD 17)

set(ANET_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(ANET_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(ANET_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

set(ASIO_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/asio/asio/include)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest)

add_library(anet)
target_sources(anet
        PUBLIC
            ${ANET_INC_DIR}/anet/base/concat.hpp
            ${ANET_INC_DIR}/anet/base/init.hpp
            ${ANET_INC_DIR}/anet/base/assert.hpp
            ${ANET_INC_DIR}/anet/base/noncopyable.hpp
            ${ANET_INC_DIR}/anet/util/chrono.hpp
            ${ANET_INC_DIR}/anet/util/io_context_pool.hpp
            ${ANET_INC_DIR}/anet/util/double_buffer.hpp
            ${ANET_INC_DIR}/anet/tcp/tcp_connection.hpp
            ${ANET_INC_DIR}/anet/tcp/tcp_connection_setter.hpp
            ${ANET_INC_DIR}/anet/tcp/tcp_server.hpp
            ${ANET_INC_DIR}/anet/tcp/tcp_client.hpp
            ${ANET_INC_DIR}/anet/anet.hpp
        PRIVATE
            ${ANET_SRC_DIR}/anet.cpp
        )
target_include_directories(anet
        PUBLIC
            ${ASIO_INC_DIR}
            ${ANET_INC_DIR}
        )

add_executable(anet_test
        ${ANET_TEST_DIR}/anet_test.hpp
        ${ANET_TEST_DIR}/anet_test.cpp
        ${ANET_TEST_DIR}/util/double_buffer_test.cpp
        )
target_link_libraries(anet_test
        PRIVATE
            anet
            gtest
            gmock)
target_include_directories(anet_test
        PRIVATE
            ${ANET_TEST_DIR})

add_subdirectory(example)
