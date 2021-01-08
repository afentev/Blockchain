cmake_minimum_required(VERSION 3.17)
project(BlockchainCpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

FIND_PACKAGE(Boost COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

add_subdirectory("cryptopp")

add_executable(BlockchainCpp main.cpp)

TARGET_LINK_LIBRARIES(BlockchainCpp LINK_PUBLIC ${Boost_LIBRARIES} OpenSSL::Crypto cryptopp-static)