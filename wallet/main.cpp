#include "wallet.h"
#include <iostream>
#include <filesystem>
#include <fstream>

int main() {
    std::cout << "Pebicoin Wallet - Test Application" << std::endl;
    std::cout << "===================================" << std::endl;
    
    try {
        // Get current working directory
        std::filesystem::path currentPath = std::filesystem::current_path();
        std::string walletFilePath = (currentPath / "test_wallet.dat").string();
        
        std::cout << "Current working directory: " << currentPath.string() << std::endl;
        std::cout << "Wallet file path: " << walletFilePath << std::endl;
        
        // Create a new wallet with absolute path
        Pebicoin::Wallet wallet(walletFilePath);
        
        // Generate a new address
        std::string address = wallet.generateNewAddress();
        
        std::cout << "Generated new address: " << address << std::endl;
        
        // Get all addresses
        std::vector<std::string> addresses = wallet.getAddresses();
        
        std::cout << "\nAll wallet addresses:" << std::endl;
        for (const auto& addr : addresses) {
            std::cout << "  " << addr << std::endl;
        }
        
        // Check if wallet file exists after operations
        if (std::filesystem::exists(walletFilePath)) {
            std::cout << "\nWallet file created successfully at: " << walletFilePath << std::endl;
            
            // Create a test file to verify write permissions
            std::string testFilePath = (currentPath / "test_write_permission.txt").string();
            std::ofstream testFile(testFilePath);
            if (testFile.is_open()) {
                testFile << "This is a test file to verify write permissions." << std::endl;
                testFile.close();
                std::cout << "Test file created successfully at: " << testFilePath << std::endl;
            } else {
                std::cout << "WARNING: Could not create test file at: " << testFilePath << std::endl;
                std::cout << "This might indicate permission issues." << std::endl;
            }
        } else {
            std::cout << "\nWARNING: Wallet file was not created at: " << walletFilePath << std::endl;
            std::cout << "Checking write permissions in the current directory..." << std::endl;
            
            // Create a test file to verify write permissions
            std::string testFilePath = (currentPath / "test_write_permission.txt").string();
            std::ofstream testFile(testFilePath);
            if (testFile.is_open()) {
                testFile << "This is a test file to verify write permissions." << std::endl;
                testFile.close();
                std::cout << "Test file created successfully at: " << testFilePath << std::endl;
                std::cout << "This indicates that the directory has write permissions." << std::endl;
                std::cout << "The issue might be with the wallet implementation." << std::endl;
            } else {
                std::cout << "WARNING: Could not create test file at: " << testFilePath << std::endl;
                std::cout << "This indicates that the directory does not have write permissions." << std::endl;
            }
        }
        
        std::cout << "\nWallet test completed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
