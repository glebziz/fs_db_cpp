cmake_minimum_required(VERSION 3.22)
project(fs_db_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_program(_PROTOBUF_PROTOC protoc)
find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)

set(files "error" "store_service")
foreach (file IN LISTS files)
    get_filename_component(proto "proto/${file}.proto" ABSOLUTE)
    get_filename_component(proto_path ${proto} PATH)

    set(proto_srcs ${CMAKE_CURRENT_BINARY_DIR}/${file}.pb.cc)
    set(grpc_srcs ${CMAKE_CURRENT_BINARY_DIR}/${file}.grpc.pb.cc)

    add_custom_command(
            OUTPUT ${proto_srcs} ${grpc_srcs}
            COMMAND ${_PROTOBUF_PROTOC}
            ARGS --grpc_out ${CMAKE_CURRENT_BINARY_DIR}
            --cpp_out ${CMAKE_CURRENT_BINARY_DIR}
            -I ${proto_path}
            --plugin=protoc-gen-grpc=${_GRPC_CPP_PLUGIN_EXECUTABLE}
            --experimental_allow_proto3_optional
            ${proto}
            DEPENDS ${proto}
    )

    set(generated_files ${generated_files} ${proto_srcs} ${grpc_srcs})
endforeach ()

add_library(fs_db_cpp
        src/connection.cpp
        src/exception.cpp
        src/transaction.cpp
        src/store.cpp
        src/iso_level.cpp
        src/write_file.cpp
        src/read_file.cpp
        ${generated_files}
)
target_link_libraries(fs_db_cpp -lgrpc++ -lprotobuf)

add_executable(fs_db_cpp_example example/main.cpp)
target_link_libraries(fs_db_cpp_example PRIVATE fs_db_cpp)

add_executable(fs_db_c_example example/main.c)
target_link_libraries(fs_db_c_example PRIVATE fs_db_cpp)
