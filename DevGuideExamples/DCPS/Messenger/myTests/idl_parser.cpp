#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Function to parse the IDL file and extract module and struct names
void parseIDL(
    const std::string& idlFileName,
    std::vector<std::pair<std::string, std::string>>& moduleStructNames) {
  std::ifstream idlFile(idlFileName);

  if (!idlFile.is_open()) {
    std::cerr << "Error: Could not open IDL file: " << idlFileName << std::endl;
    return;
  }

  std::string line;
  std::string currentModule;
  while (std::getline(idlFile, line)) {
    // Remove leading and trailing whitespaces
    line.erase(line.find_last_not_of(" \t\r\n") + 1);

    // Skip empty lines and comments
    if (line.empty() || line.substr(0, 2) == "//") {
      continue;
    }

    // Check for the start of a module
    size_t modulePos = line.find("module");
    if (modulePos != std::string::npos) {
      size_t moduleNameStart = modulePos + 7;  // "module" has 6 characters
      size_t moduleNameEnd = line.find_first_of("{", moduleNameStart);
      if (moduleNameEnd != std::string::npos) {
        currentModule =
            line.substr(moduleNameStart, moduleNameEnd - moduleNameStart);
      }
    }

    // Check for the start of a struct
    size_t structPos = line.find("struct");
    if (structPos != std::string::npos) {
      size_t structNameStart = structPos + 7;  // "struct" has 6 characters
      size_t structNameEnd = line.find_first_of("{", structNameStart);
      if (structNameEnd != std::string::npos) {
        std::string structName =
            line.substr(structNameStart, structNameEnd - structNameStart);
        moduleStructNames.emplace_back(currentModule, structName);
      }
    }
  }

  idlFile.close();
}

int main() {
  std::vector<std::pair<std::string, std::string>> moduleStructNames;
  parseIDL("../Messenger2.idl", moduleStructNames);

  // Print the extracted module and struct names
  for (const auto& pair : moduleStructNames) {
    std::cout << "Module: " << pair.first << ", Struct: " << pair.second
              << std::endl;
  }

  return 0;
}
