// generate_listener.cpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

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


void generateClassHeader(std::ostringstream& code) {
  code << "#ifndef DATAREADER_LISTENER_IMPL_H\n"
          "#define DATAREADER_LISTENER_IMPL_H\n"
          "\n"
          "#include <ace/Global_Macros.h>\n"
          "\n"
          "#include <dds/DdsDcpsSubscriptionC.h>\n"
          "#include <dds/DCPS/LocalObject.h>\n"
          "#include <dds/DCPS/Definitions.h>\n"
          "\n"
          "\n"
          "class "
          "DataReaderListenerImpl\n"
          "  : public virtual "
          "OpenDDS::DCPS::LocalObject<DDS::DataReaderListener> {\n"
          "public:\n";
}

void generateClassHeaderEnd(std::ostringstream& code) {
  code << "};"
          "\n"
          "\n"
          "#endif /* DATAREADER_LISTENER_IMPL_H */";
}

void generateSourceHeaders(std::ostringstream& code,std::vector<std::pair<std::string, std::string>>& moduleStructNames) 
{
     code << "#include <ace/Log_Msg.h>\n"
          "#include <ace/OS_NS_stdlib.h>\n"
          "\n"
          "#include \"DataReaderListenerImpl.h\"\n\n";
   for (const auto& pair : moduleStructNames) {
      std::cout << "Module: " << pair.first << ", Struct: " << pair.second
                  << std::endl;
      std::string moduleName = pair.first;
      std::string structName = pair.second;
         
      structName.erase(std::remove_if(structName.begin(), structName.end(), ::isspace), structName.end());
      moduleName.erase(std::remove_if(moduleName.begin(), moduleName.end(), ::isspace), moduleName.end());
      std::string structName_obj = structName;
      structName_obj[0] = std::tolower(structName_obj[0]);

   code <<"#include \"" << moduleName
        << "TypeSupportC.h\"\n"
          "#include \""
        << moduleName
        << "TypeSupportImpl.h\"\n\n";
   }
   code << "#include <iostream>\n"
          "\n";
}

void generateOnDataAvailSource(std::ostringstream & code,
                                   const std::string& moduleName,
                                   const std::string& structName) {
  std::string structName_obj = structName;
  structName_obj[0] = std::tolower(structName_obj[0]);


code   <<  "void\n"
       << "DataReaderListenerImpl::on_data_available(DDS::DataReader_ptr "
          "reader)\n"
          "{\n"
          "    "
       << moduleName << "::" << structName << "DataReader_var "
       << structName_obj
       << "_reader =\n"
          "      "
       << moduleName << "::" << structName
       << "DataReader::_narrow(reader);\n\n"
          "    if (!"
       << structName_obj
       << "_reader) {\n"
          "      ACE_ERROR((LM_ERROR,\n"
          "                 ACE_TEXT(\"ERROR: %N:%l: on_data_available() -\")\n"
          "                 ACE_TEXT(\" _narrow failed!\\n\")));\n"
          "      ACE_OS::exit(1);\n"
          "    }\n\n"
          "    "
       << moduleName << "::" << structName << " " << structName_obj
       << ";\n"
          "    DDS::SampleInfo info;\n\n"
          "const DDS::ReturnCode_t error = "
       << structName_obj
       << "_reader"
          "->take_next_sample("
       << structName_obj
       << ",info);\n\n"
          "if (error == DDS::RETCODE_OK) {\n"
          "      std::cout << \"SampleInfo.sample_rank = \" << "
          "info.sample_rank << std::endl;\n"
          "      std::cout << \"SampleInfo.instance_state = \" << "
          "OpenDDS::DCPS::InstanceState::instance_state_mask_string(info."
          "instance_state) << std::endl;\n\n"
          "      if (info.valid_data) {\n"
          "        std::cout << \"Message: subject    = \" << "
       << structName_obj
       << ".subject.in() << std::endl\n"
          "                  << \"         subject_id = \" << "
       << structName_obj
       << ".subject_id   << std::endl\n"
          "                  << \"         from       = \" << "
       << structName_obj
       << ".from.in()    << std::endl\n"
          "                  << \"         count      = \" << "
       << structName_obj
       << ".count        << std::endl\n"
          "                  << \"         text       = \" << "
       << structName_obj
       << ".text.in()    << std::endl;\n"
          "      }\n"
          "    } else {\n"
          "      ACE_ERROR((LM_ERROR,\n"
          "                 ACE_TEXT(\"ERROR: %N:%l: on_data_available() -\")\n"
          "                 ACE_TEXT(\" take_next_sample failed!\\n\")));\n"
          "    }\n"
          "}\n";
}

void generateOnDataAvailSource2(std::ostringstream & code,
                                std::vector<std::pair<std::string, std::string>>& moduleStructNames)
{

code   <<  "void\n"
       << "DataReaderListenerImpl::on_data_available(DDS::DataReader_ptr "
          "reader)\n"
          "{\n"
          "    ";

   for (const auto& pair : moduleStructNames) {
    std::cout << "Module: " << pair.first << ", Struct: " << pair.second
              << std::endl;
   std::string moduleName = pair.first;
   std::string structName = pair.second;
   
  structName.erase(std::remove_if(structName.begin(), structName.end(), ::isspace), structName.end());
  moduleName.erase(std::remove_if(moduleName.begin(), moduleName.end(), ::isspace), moduleName.end());
  std::string structName_obj = structName;
  structName_obj[0] = std::tolower(structName_obj[0]);

code   << "    "<< moduleName << "::" << structName << "DataReader_var "
       << structName_obj
       << "_reader =\n"
          "      "
       << moduleName << "::" << structName
       << "DataReader::_narrow(reader);\n\n"
          "    if (!"
       << structName_obj
       << "_reader) {\n"
          "      ACE_ERROR((LM_ERROR,\n"
          "                 ACE_TEXT(\"ERROR: %N:%l: on_data_available() -\")\n"
          "                 ACE_TEXT(\" _narrow failed!\\n\")));\n"
          "      ACE_OS::exit(1);\n"
          "    }\n\n"
          "    "
       << moduleName << "::" << structName << " " << structName_obj
       << ";\n"
          "    DDS::SampleInfo info;\n\n"
          "    const DDS::ReturnCode_t error = "
       << structName_obj
       << "_reader"
          "->take_next_sample("
       << structName_obj
       << ",info);\n\n"
          "    if (error == DDS::RETCODE_OK) {\n"
          "      std::cout << \"SampleInfo.sample_rank = \" << "
          "info.sample_rank << std::endl;\n"
          "      std::cout << \"SampleInfo.instance_state = \" << "
          "OpenDDS::DCPS::InstanceState::instance_state_mask_string(info."
          "instance_state) << std::endl;\n\n"
          "      if (info.valid_data) {\n"
          "        std::cout << \"Message: subject    = \" << "
       << structName_obj
       << ".subject.in() << std::endl\n"
          "                  << \"         subject_id = \" << "
       << structName_obj
       << ".subject_id   << std::endl\n"
          "                  << \"         from       = \" << "
       << structName_obj
       << ".from.in()    << std::endl\n"
          "                  << \"         count      = \" << "
       << structName_obj
       << ".count        << std::endl\n"
          "                  << \"         text       = \" << "
       << structName_obj
       << ".text.in()    << std::endl;\n"
          "      }\n"
          "    } else {\n"
          "      ACE_ERROR((LM_ERROR,\n"
          "                 ACE_TEXT(\"ERROR: %N:%l: on_data_available() -\")\n"
          "                 ACE_TEXT(\" take_next_sample failed!\\n\")));\n"
          "    }\n";
          
   }
   code << "}\n\n";
}

void generateOnDataAvailHeader(std::ostringstream& code){
  code << "  virtual void on_data_available(\n"
          "    DDS::DataReader_ptr reader);\n";
}

void generateCppMethod(
    std::ostream& code, const std::string& methodName,
    const std::string& DDSNamespaceName) {
  code << "void DataReaderListenerImpl::" << methodName
       << "(DDS::DataReader_ptr /*reader*/, const DDS::" << DDSNamespaceName
       << "& /*status*/)\n"
          "{\n"
          "}\n\n";
}

void generateHeaderMethod(std::ostream& code,
                         const std::string& methodName,
                          const std::string& DDSNamespaceName) {
  code << "virtual void " << methodName
       << "(DDS::DataReader_ptr reader, \n const DDS::" << DDSNamespaceName
       << "& status);\n\n";
}

void generateOnDataAvailHeader(std::ostream& code,
                               const std::string& methodName){
  code << "  virtual void on_data_available(\n"
          "DDS::DataReader_ptr reader);\n";
}

//int main(int argc, char* argv[]) {
int main() {
//   if (argc != 3) {
//     std::cerr << "Usage: generate_listener <module_name> <struct_name>"
//               << std::endl;
//     return 1;
//   }
  std::vector<std::pair<std::string, std::string>> moduleStructNames;
  parseIDL("../Messenger2.idl", moduleStructNames);

  const std::string CPP_FILE_NAME = "DataReaderListenerImpl.cpp";
  const std::string HEADER_FILE_NAME = "DataReaderListenerImpl.h";

//   const std::string moduleName = argv[1];
//   const std::string structName = argv[2];

  std::ostringstream cppConstruct;
  std::ostringstream headerConstruct;
  //generateClassHeader(generatedHeaderCode, structName);
  //generateClassImplementation(generatedSourceCode, moduleName, structName);

  std::ofstream cppFile(CPP_FILE_NAME);
  std::ofstream headerFile(HEADER_FILE_NAME);
  // Generate method implementations in the .cpp file
  generateSourceHeaders(cppConstruct, moduleStructNames);
  generateCppMethod(cppConstruct, "on_requested_deadline_missed",
                    "RequestedDeadlineMissedStatus");
  generateCppMethod(cppConstruct, "on_requested_incompatible_qos",
                    "RequestedIncompatibleQosStatus");
  generateCppMethod(cppConstruct, "on_sample_rejected", "SampleRejectedStatus");
  generateCppMethod(cppConstruct, "on_liveliness_changed",
                    "LivelinessChangedStatus");
  //generateOnDataAvailSource(cppConstruct, moduleName, structName);
  generateOnDataAvailSource2(cppConstruct,moduleStructNames);
  generateCppMethod(cppConstruct, "on_subscription_matched",
                    "SubscriptionMatchedStatus");
  generateCppMethod(cppConstruct, "on_sample_lost", "SampleLostStatus ");

  // Generate method declarations in the .h file
  generateClassHeader(headerConstruct);
  generateHeaderMethod(headerConstruct, "on_requested_deadline_missed",
                    "RequestedDeadlineMissedStatus");
  generateHeaderMethod(headerConstruct, "on_requested_incompatible_qos",
                    "RequestedIncompatibleQosStatus");
  generateHeaderMethod(headerConstruct, "on_sample_rejected", "SampleRejectedStatus");
  generateHeaderMethod(headerConstruct, "on_liveliness_changed",
                    "LivelinessChangedStatus");
  generateOnDataAvailHeader(headerConstruct);
  generateHeaderMethod(headerConstruct, "on_subscription_matched",
                       "SubscriptionMatchedStatus");
  generateHeaderMethod(headerConstruct, "on_sample_lost", "SampleLostStatus ");
  generateClassHeaderEnd(headerConstruct);


  cppFile << cppConstruct.str();
  headerFile << headerConstruct.str();

  std::cout << "Generated "
            << "DataReaderListenerImpl.cpp and DataReaderListenerImpl.h"
            << std::endl;

  return 0;
}
