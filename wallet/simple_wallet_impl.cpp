#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

// Simple key pair structure
struct SimpleKeyPair {
    std::string privateKey;
    std::string publicKey;
    std::string address;
};

// Simple wallet class
class SimpleWallet {
private:
    std::string walletFile;
    std::vector<SimpleKeyPair> keyPairs;
    
    bool saveToFile() {
        std::ofstream file(walletFile);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open wallet file for writing: " << walletFile << std::endl;
            return false;
        }
        
        file << "# Simple Wallet File" << std::endl;
        file << "# Format: privateKey,publicKey,address" << std::endl;
        
        for (const auto& keyPair : keyPairs) {
            file << keyPair.privateKey << "," 
                 << keyPair.publicKey << ","
                 << keyPair.address << std::endl;
        }
        
        file.close();
        return true;
    }
    
    bool loadFromFile() {
        std::ifstream file(walletFile);
        if (!file.is_open()) {
            std::cerr << "Info: Wallet file does not exist or cannot be opened: " << walletFile << std::endl;
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Parse CSV format
            size_t pos1 = line.find(',');
            if (pos1 == std::string::npos) continue;
            
            size_t pos2 = line.find(',', pos1 + 1);
            if (pos2 == std::string::npos) continue;
            
            SimpleKeyPair keyPair;
            keyPair.privateKey = line.substr(0, pos1);
            keyPair.publicKey = line.substr(pos1 + 1, pos2 - pos1 - 1);
            keyPair.address = line.substr(pos2 + 1);
            
            keyPairs.push_back(keyPair);
        }
        
        file.close();
        return true;
    }
    
public:
    SimpleWallet(const std::string& filename) : walletFile(filename) {
        // Try to load existing wallet file
        if (!loadFromFile()) {
            std::cout << "Creating new wallet file: " << filename << std::endl;
        } else {
            std::cout << "Loaded existing wallet file: " << filename << std::endl;
            std::cout << "Found " << keyPairs.size() << " key pairs." << std::endl;
        }
    }
    
    ~SimpleWallet() {
        // Save wallet data when destroyed
        saveToFile();
    }
    
    // Generate a new address (simplified)
    std::string generateNewAddress() {
        SimpleKeyPair keyPair;
        
        // In a real implementation, these would be cryptographically generated
        keyPair.privateKey = "private_" + std::to_string(keyPairs.size() + 1);
        keyPair.publicKey = "public_" + std::to_string(keyPairs.size() + 1);
        keyPair.address = "address_" + std::to_string(keyPairs.size() + 1);
        
        keyPairs.push_back(keyPair);
        
        // Save after generating a new address
        saveToFile();
        
        return keyPair.address;
    }
    
    // Get all addresses
    std::vector<std::string> getAddresses() const {
        std::vector<std::string> addresses;
        for (const auto& keyPair : keyPairs) {
            addresses.push_back(keyPair.address);
        }
        return addresses;
    }
};

int main() {
    std::cout << "Simple Wallet Implementation Test" << std::endl;
    std::cout << "==============================" << std::endl;
    
    try {
        // Get current working directory
        std::filesystem::path currentPath = std::filesystem::current_path();
        std::string walletFilePath = (currentPath / "simple_wallet_impl.dat").string();
        
        std::cout << "Current working directory: " << currentPath.string() << std::endl;
        std::cout << "Wallet file path: " << walletFilePath << std::endl;
        
        // Create a new wallet
        SimpleWallet wallet(walletFilePath);
        
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
            
            // Read the wallet file contents
            std::ifstream walletFile(walletFilePath);
            if (walletFile.is_open()) {
                std::string line;
                std::cout << "\nWallet file contents:" << std::endl;
                while (std::getline(walletFile, line)) {
                    std::cout << line << std::endl;
                }
                walletFile.close();
            }
        } else {
            std::cout << "\nWARNING: Wallet file was not created at: " << walletFilePath << std::endl;
        }
        
        std::cout << "\nSimple wallet implementation test completed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}