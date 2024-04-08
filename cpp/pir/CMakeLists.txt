project(PIR)
find_package(SEAL 4.0 REQUIRED)
include_directories(include thirdparty)
set(PirSources thirdparty/pir.cpp thirdparty/seal_client.cpp thirdparty/seal_server.cpp src/pir_client.cpp src/pir_server.cpp)

add_library(pir SHARED ${PirSources})

#pir链接依赖
target_link_libraries(pir SEAL::seal)

add_executable(pir_main main.cpp)
target_link_libraries(pir_main pir)



