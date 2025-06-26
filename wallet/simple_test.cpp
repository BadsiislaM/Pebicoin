#include <iostream>
#include <filesystem>
#include <fstream>

int main() {
    std::cout << "Simple Wallet Test Application" << std::endl;
    std::cout << "============================" << std::endl;
    
    try {
        // Get current working directory
        std::filesystem::path currentPath = std::filesystem::current_path();
        std::string walletFilePath = (currentPath / "simple_test_wallet.dat").string();
        
        std::cout << "Current working directory: " << currentPath.string() << std::endl;
        std::cout << "Test wallet file path: " << walletFilePath << std::endl;
        
        // Create a test wallet file
        std::ofstream walletFile(walletFilePath);
        if (walletFile.is_open()) {
            walletFile << "This is a test wallet file." << std::endl;
            walletFile.close();
            std::cout << "\nWallet file created successfully at: " << walletFilePath << std::endl;
        } else {
            std::cout << "\nWARNING: Could not create wallet file at: " << walletFilePath << std::endl;
            std::cout << "This might indicate permission issues." << std::endl;
        }
        
        // Check if wallet file exists after operations
        if (std::filesystem::exists(walletFilePath)) {
            std::cout << "Wallet file exists after creation." << std::endl;
            
            // Read the wallet file
            std::ifstream readWalletFile(walletFilePath);
            if (readWalletFile.is_open()) {
                std::string line;
                std::cout << "\nWallet file contents:" << std::endl;
                while (std::getline(readWalletFile, line)) {
                    std::cout << line << std::endl;
                }
                readWalletFile.close();
            } else {
                std::cout << "WARNING: Could not read wallet file." << std::endl;
            }
            
            // Create a test file to verify write permissions
            std::string testFilePath = (currentPath / "simple_test_permission.txt").string();
            std::ofstream testFile(testFilePath);
            if (testFile.is_open()) {
                testFile << "This is a test file to verify write permissions." << std::endl;
                testFile.close();
                std::cout << "\nTest file created successfully at: " << testFilePath << std::endl;
            } else {
                std::cout << "WARNING: Could not create test file at: " << testFilePath << std::endl;
                std::cout << "This might indicate permission issues." << std::endl;
            }
        } else {
            std::cout << "\nWARNING: Wallet file was not created at: " << walletFilePath << std::endl;
            std::cout << "Checking write permissions in the current directory..." << std::endl;
            
            // Create a test file to verify write permissions
            std::string testFilePath = (currentPath / "simple_test_permission.txt").string();
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
        
        std::cout << "\nSimple wallet test completed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}