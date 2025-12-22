cmake_minimum_required(VERSION 3.31)
project(MessageAPp_TLS C)

set(CMAKE_C_STANDARD 11)

add_executable(MessageAPp_TLS main.c
        Server.c
)
