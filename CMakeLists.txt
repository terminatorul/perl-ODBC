cmake_minimum_required(VERSION 3.26)

project(sqlodbc VERSION 0.1 DESCRIPTION "Portable ODBC library and tools" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(ODBCXX_HEADERS
    "include/odbc++/Exports.h"
    "include/odbc++/SQLDiagnosticException.hh"
    "include/odbc++/Handle.hh"
    "include/odbc++/Environment.hh"
    "include/odbc++/Connection.hh")

set(ODBCXX_SOURCES
    "src/odbc++/Handle.cc"
    "src/odbc++/Environment.cc"
    "src/odbc++/Connection.cc")

add_library(odbc++ SHARED ${ODBCXX_HEADERS} ${ODBCXX_SOURCES})
target_include_directories(odbc++ PUBLIC "include")
set_target_properties(odbc++ PROPERTIES MSVC_DEBUG_INFORMATION_FORMAT "EditAndContinue" COMPILE_PDB_NAME "odbc++.compile")

if(WIN32)
    target_link_libraries(odbc++ PUBLIC ODBC32)
endif()

set(SQLODBC_HEADERS
    "src/dbcmd/Context.hh"
    "src/dbcmd/CommandHandler.hh"
    "src/dbcmd/HandlerFunctor.hh"
    "src/dbcmd/browseConnect.hh"
    "src/dbcmd/Drivers.hh"
    "src/dbcmd/ShellCommand.hh")

set(SQLODBC_SOURCES
    "src/sqlodbc.cc"
    "src/dbcmd/CommandHandler.cc"
    "src/dbcmd/browseConnect.cc"
    "src/dbcmd/Drivers.cc"
    "src/dbcmd/ShellCommand.cc") # "src/dbcmd/HandlerFunctor.cc"

add_executable(sqlodbc ${SQLODBC_HEADERS} ${SQLODBC_SOURCES})
target_link_libraries(sqlodbc odbc++)
set_target_properties(sqlodbc PROPERTIES MSVC_DEBUG_INFORMATION_FORMAT "EditAndContinue" COMPILE_PDB_NAME "sqlodbc.compile")
