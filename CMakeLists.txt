SET (CMAKE_C_COMPILER             "clang")
SET (CMAKE_CXX_COMPILER            "clang++")
set(CMAKE_CXX_FLAGS "-Wall -std=c++11  ${EXTRA_FLAGS}")

cmake_minimum_required(VERSION 3.0)

project(duppatest)
   

set(SOURCES
	src/main.cpp
	src/ErrorMgr.cpp
	src/I2C.cpp
	src/DuppaEncoder.cpp
	)

 
add_executable(duppatest ${SOURCES})


set_target_properties(duppatest PROPERTIES
				CXX_STANDARD 17
				CXX_EXTENSIONS OFF
				)
 
set_target_properties(duppatest PROPERTIES
			OUTPUT_NAME "duppatest"
			)


target_link_libraries(duppatest
	 PRIVATE
   	${EXTRA_LIBS}
 	)

target_include_directories(duppatest
	PRIVATE
	src
)

set(CMAKE_BINARY_DIR "bin")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

a 