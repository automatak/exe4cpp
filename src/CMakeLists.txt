set(exe4cpp_public_headers
    ./exe4cpp/IExecutor.h
    ./exe4cpp/ISteadyTimeSource.h
    ./exe4cpp/ITimer.h
    ./exe4cpp/MockExecutor.h
    ./exe4cpp/Timer.h
    ./exe4cpp/Typedefs.h
)

add_library(exe4cpp INTERFACE)
#target_sources(exe4cpp INTERFACE ${exe4cpp_public_headers})
target_compile_features(exe4cpp INTERFACE cxx_std_14)
target_include_directories(exe4cpp INTERFACE .)
