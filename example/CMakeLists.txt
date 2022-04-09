add_executable(tcp_echo_server tcp/echo_server.cpp)
target_link_libraries(tcp_echo_server anet pthread)
add_executable(tcp_echo_client tcp/echo_client.cpp)
target_link_libraries(tcp_echo_client anet pthread)

add_executable(http_server http/server.cpp)
target_link_libraries(http_server anet pthread)
add_executable(http_client http/client.cpp)
target_link_libraries(http_client anet pthread)

