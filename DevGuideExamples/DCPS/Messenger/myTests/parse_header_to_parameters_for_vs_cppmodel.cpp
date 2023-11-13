#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <vector>

// Define the Parameters struct
typedef struct {
  double a[2][2];
  double b11[5];
  double c12;
  double idleRPM[10];
  double redlineRPM;
  double temperature_timeConstant;
  double temperature_roomTemp;
  double temperature_operatingTempDelta;
  double temperature_redlineTempDelta;
} Parameters;

// Function to generate code for NI_Parameter and rtParamAttribs
std::string generateParameterCode(
    const std::vector<std::pair<std::string, size_t>>& variablesWithCounts) {
  std::stringstream code;
  code << "int32_t ParameterSize DataSection(\".NIVS.paramlistsize\") = "
       << variablesWithCounts.size() << ";\n";
  code
      << "NI_Parameter rtParamAttribs[] DataSection(\".NIVS.paramlist\") = {\n";

  size_t offset = 0;
  for (const auto& pair : variablesWithCounts) {
    code << std::setw(4) << "{ 0, \"engine/" << pair.first << "\", "
         << "offsetof(Parameters, " << pair.first << "), "
         << "rtDBL, " << pair.second << ", 2, " << offset << ", 0},\n";
    offset += 2;
  }

  code << "};";

  return code.str();
}

// Function to extract variable names and count total elements
std::vector<std::pair<std::string, size_t>> extractVariablesWithCounts(
    const std::string& structDefinition) {
  std::vector<std::pair<std::string, size_t>> variablesWithCounts;
  std::regex pattern(R"(\bdouble\b\s+(\w+)(\[(\d+)\])?(?:\[(\d+)\])?;)");

  std::smatch matches;
  auto it = structDefinition.cbegin();

  while (std::regex_search(it, structDefinition.cend(), matches, pattern)) {
    std::string variableName = matches[1].str();
    size_t totalElements = 1;

    // Check if the variable is an array and calculate total elements
    if (matches[2].matched) {
      size_t dimension1 = std::stoi(matches[3].str());
      totalElements *= dimension1;
    }

    if (matches[4].matched) {
      size_t dimension2 = std::stoi(matches[4].str());
      totalElements *= dimension2;
    }

    variablesWithCounts.emplace_back(variableName, totalElements);
    it = matches.suffix().first;
  }

  return variablesWithCounts;
}

int main() {
  // Read the header file into a stringstream
  std::ifstream headerFile(
      "parameters.h");  // Replace "parameters.h" with your actual header file
  std::stringstream headerStream;
  headerStream << headerFile.rdbuf();

  // Extract variable names and count total elements
  std::string structDefinition = headerStream.str();
  std::vector<std::pair<std::string, size_t>> variablesWithCounts =
      extractVariablesWithCounts(structDefinition);

  // Generate and print the NI_Parameter and rtParamAttribs code
  std::cout << generateParameterCode(variablesWithCounts) << "\n";

  return 0;
}
