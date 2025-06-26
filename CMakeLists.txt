cmake_minimum_required(VERSION 3.10)
project(Pebicoin VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automatic Qt processing
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find required packages
find_package(OpenSSL REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/wallet)
include_directories(${CMAKE_SOURCE_DIR}/gui_wallet)

# Source files for the core library
set(CORE_SOURCES
    src/block.cpp
    src/blockchain.cpp
    src/hasher.cpp
    src/miner.cpp
    src/utils.cpp
)

# Source files for the wallet library
set(WALLET_SOURCES
    wallet/wallet.cpp
    wallet/keys.cpp
    wallet/address.cpp
    wallet/transaction.cpp
    wallet/signer.cpp
    wallet/base58.cpp
    wallet/crypto_utils.cpp
)

# Source files for GUI wallet
set(GUI_WALLET_SOURCES
    gui_wallet/src/main.cpp
    gui_wallet/src/mainwindow.cpp
    gui_wallet/src/walletmodel.cpp
    gui_wallet/src/transactionmodel.cpp
    gui_wallet/src/mainwindow.h
    gui_wallet/src/walletmodel.h
    gui_wallet/src/transactionmodel.h
    gui_wallet/ui/mainwindow.ui
    gui_wallet/ui/resources.qrc
)

# Create the core library
add_library(pebicoin_core ${CORE_SOURCES})
target_link_libraries(pebicoin_core OpenSSL::SSL OpenSSL::Crypto)

# Create the wallet library
add_library(pebicoin_wallet ${WALLET_SOURCES})
target_link_libraries(pebicoin_wallet pebicoin_core OpenSSL::SSL OpenSSL::Crypto)

# Main CLI executable
add_executable(pebicoin src/main.cpp)
target_link_libraries(pebicoin pebicoin_core pebicoin_wallet)

# CLI Miner
add_executable(pebiminer pebiminer/main.cpp)
target_link_libraries(pebiminer pebicoin_core)

# GUI Wallet executable
add_executable(pebiwallet ${GUI_WALLET_SOURCES})
target_link_libraries(pebiwallet
    pebicoin_core
    pebicoin_wallet
    Qt5::Widgets
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Installation
install(TARGETS pebicoin pebiminer pebiwallet
        RUNTIME DESTINATION bin)
