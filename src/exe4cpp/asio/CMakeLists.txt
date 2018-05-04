cmake_minimum_required (VERSION 3.8)

find_package (Threads)

set(asiopal_public_headers    
	./include/asiopal/BasicExecutor.h
	./include/asiopal/StrandExecutor.h
	./include/asiopal/ThreadPool.h
)

set(asiopal_src
	./src/ASIOTimer.h
	./src/BasicExecutor.cpp
    ./src/StrandExecutor.cpp
	./src/ThreadPool.cpp
)

find_package(Threads)

add_library(asiopal ${asiopal_public_headers} ${asiopal_src})
target_compile_features(asiopal PUBLIC cxx_std_14)
target_include_directories(asiopal PUBLIC ./include)
target_link_libraries(asiopal PUBLIC openpal Threads::Threads)

# Add asio dependency
if(TARGET asio)
	target_link_libraries(asiopal PUBLIC asio)
else()
	target_include_directories(asiopal PUBLIC ${CMAKE_SOURCE_DIR}/deps/asio/asio/include)
	target_compile_definitions(asiopal PUBLIC -DASIO_STANDALONE -DASIO_HAS_STD_SYSTEM_ERROR)
	if(WIN32)
	    target_compile_definitions(asiopal PUBLIC -D_WIN32_WINNT=0x0501)
	endif()
endif()

