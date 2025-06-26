cmake_minimum_required(VERSION 3.16)
project(PebiWallet VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Build parent project to get core libraries
add_subdirectory(${CMAKE_SOURCE_DIR}/.. ${CMAKE_BINARY_DIR}/pebicoin_build)

# Include OpenSSL
find_package(OpenSSL REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/../wallet
    ${CMAKE_SOURCE_DIR}/../src
    ${OPENSSL_INCLUDE_DIR}
)

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/walletmodel.cpp
    src/transactionmodel.cpp
)

# Header files
set(HEADERS
    src/mainwindow.h
    src/walletmodel.h
    src/transactionmodel.h
)

# UI
set(UI_FILES
    ui/mainwindow.ui
)

# Resources
set(RESOURCE_FILES
    ui/resources.qrc
)

# Output path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/gui_wallet)

# Executable
add_executable(PebiWallet ${SOURCES} ${HEADERS} ${UI_FILES} ${RESOURCE_FILES})
target_link_libraries(PebiWallet
    Qt5::Widgets
    OpenSSL::SSL
    OpenSSL::Crypto
    pebicoin_core
    pebicoin_wallet
)

# Message
message(STATUS "✅ GUI Wallet (PebiWallet) configured successfully")
