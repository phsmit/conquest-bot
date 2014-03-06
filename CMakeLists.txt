cmake_minimum_required(VERSION 2.8)
PROJECT(conquest)

#ADD_DEFINITIONS(
#    -std=c++0x # Or -std=c++0x
#    # Other flags
#)

add_executable( bot bot.cc util.cc io.cc )
