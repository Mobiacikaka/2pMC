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

# find_package(ABY QUIET)
# if(ABY)
# 	message(STATUS "Found ABY")
# elseif (NOT ABY AND NOT TARGET aby)
# 	message("ABY was not found")
# endif()

# find_package(ENCRYPTO_UTILS QUIET)
# if(ENCRYPTO_UTILS)
# 	message(STATUS "Found ENCRYPTO_UTILS")
# elseif (NOT ENCRYPTO_UTILS AND NOT TARGET encrypto_utils)
# 	message("ENCRYPTO_UTILS was not found")
# endif()

# find_package(OTEXTENSION QUIET)
# if(OTEXTENSION)
# 	message(STATUS "Found OTEXTENSION")
# elseif (NOT OTEXTENSION AND NOT TARGET otextension)
# 	message("OTEXTENSION was not found")
# endif()

# find_library(OT libotextension.a)
# if(OT)
# 	message(STATUS "Found OTEXTENSION")
# elseif(NOT OT AND NOT TARGET otextension)
# 	message("OTEXTENSION was not found")
# endif(OT)


# find_package(CRYPTO REQUIRED)
# if(CRYPTO)
# 	message(STATUS "Found CRYPTO")
# elseif (NOT CRYPTO AND NOT TARGET crypto)
# 	message("CRYPTO was not found")
# endif()

add_executable(millionnair millionaire_prob_test.cpp millionaire_prob.cpp)
target_link_libraries(
	millionnair
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
