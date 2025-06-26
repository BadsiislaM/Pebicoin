#include <iostream>
#include <fstream>
#include <filesystem>

int main() {
    std::cout << "Testing file creation and write permissions" << std::endl;
    
    // Get current working directory
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::cout << "Current working directory: " << currentPath.string() << std::endl;
    
    // Create a test file
    std::string testFilePath = (currentPath / "test_file_creation.dat").string();
    std::ofstream testFile(testFilePath);
    
    if (testFile.is_open()) {
        testFile << "This is a test file to verify write permissions." << std::endl;
        testFile.close();
        std::cout << "Test file created successfully at: " << testFilePath << std::endl;
        
        // Verify the file exists
        if (std::filesystem::exists(testFilePath)) {
            std::cout << "File exists after creation." << std::endl;
        } else {
            std::cout << "ERROR: File does not exist after creation!" << std::endl;
        }
    } else {
        std::cout << "ERROR: Could not create test file at: " << testFilePath << std::endl;
    }
    
    return 0;
}