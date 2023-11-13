#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

void processRow(std::istringstream& stream, std::vector<std::string>& output) {
    std::string line;
    while (std::getline(stream, line)) {
        // Remove @key from the line
        size_t keyPos = line.find("@key");
        if (keyPos != std::string::npos) {
            line.erase(keyPos, 4); // Remove @key
        }

        // Replace string with std::string
        size_t stringPos = line.find("string");
        while (stringPos != std::string::npos) {
            line.replace(stringPos, 6, "std::string");
            stringPos = line.find("string", stringPos + 10); // Move ahead to avoid infinite loop
        }

        // Skip lines containing @topic, module, and the closing brace
        if (line.find("@topic") != std::string::npos ||
            line.find("module") != std::string::npos ||
            line.find("};") != std::string::npos) {
            continue;
        }

        output.push_back(line);
    }
}

void createHeaderFile(const std::string& fileName, const std::vector<std::string>& processedCode) {
    std::ofstream headerFile(fileName);
    if (headerFile.is_open()) {
        // Write the processed code to the header file
        headerFile << "#include <string>\n\n";
        for (const auto& processedLine : processedCode) {
            headerFile << processedLine << std::endl;
        }

        // Add the closing brace
        headerFile << "};" << std::endl;

        headerFile.close();
        std::cout << "Header file '" << fileName << "' created successfully." << std::endl;
    } else {
        std::cerr << "Unable to create the header file." << std::endl;
    }
}

int main() {
    std::string inputCode = "module Messenger {\n\n  @topic\n  struct Message {\n    string from;\n    string subject;\n    @key long subject_id;\n    string text;\n    long count;\n  };\n};";

    std::vector<std::string> processedCode;
    std::istringstream codeStream(inputCode);

    // Skip the first line (module Messenger {)
    std::string line;
    std::getline(codeStream, line);

    // Process the struct content
    processRow(codeStream, processedCode);

    // Create the header file
    createHeaderFile("Message.h", processedCode);

    return 0;
}
