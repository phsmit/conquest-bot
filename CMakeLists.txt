cmake_minimum_required(VERSION 2.8)
PROJECT(conquest)

ADD_DEFINITIONS(
    -std=c++0x
)

add_executable( bot bot.cc util.cc )
