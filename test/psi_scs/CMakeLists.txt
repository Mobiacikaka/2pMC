cmake_minimum_required(VERSION 3.0.0)

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
endif()

add_executable(psi_scs_test psi_scs.cpp common/sort_compare_shuffle.cpp common/WaksmanPermutation.cpp)

target_link_libraries(
	psi_scs_test 
	aby 
	otextension
	encrypto_utils 
	crypto
	relic_s
	pthread
	ssl
	gmp
	gmpxx
	boost_system
)
