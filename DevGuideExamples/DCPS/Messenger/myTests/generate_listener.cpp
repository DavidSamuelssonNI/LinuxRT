// generate_listener.cpp

#include <iostream>
#include <fstream>
#include <sstream>


void generateClassHeader(std::ostringstream& code, const std::string& structName) {
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

void generateSourceHeaders(std::ostringstream& code,
                           const std::string& moduleName) {
  code << "#include <ace/Log_Msg.h>\n"
          "#include <ace/OS_NS_stdlib.h>\n"
          "\n"
          "#include \"DataReaderListenerImpl.h\"\n"
          "#include \""
       << moduleName
       << "TypeSupportC.h\"\n"
          "#include \""
       << moduleName
       << "TypeSupportImpl.h\"\n\n"
          "#include <iostream>\n"
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

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: generate_listener <module_name> <struct_name>"
              << std::endl;
    return 1;
  }

  const std::string CPP_FILE_NAME = "DataReaderListenerImpl.cpp";
  const std::string HEADER_FILE_NAME = "DataReaderListenerImpl.h";

  const std::string moduleName = argv[1];
  const std::string structName = argv[2];

  std::ostringstream cppConstruct;
  std::ostringstream headerConstruct;
  //generateClassHeader(generatedHeaderCode, structName);
  //generateClassImplementation(generatedSourceCode, moduleName, structName);

  std::ofstream cppFile(CPP_FILE_NAME);
  std::ofstream headerFile(HEADER_FILE_NAME);
  // Generate method implementations in the .cpp file
  generateSourceHeaders(cppConstruct, moduleName);
  generateCppMethod(cppConstruct, "on_requested_deadline_missed",
                    "RequestedDeadlineMissedStatus");
  generateCppMethod(cppConstruct, "on_requested_incompatible_qos",
                    "RequestedIncompatibleQosStatus");
  generateCppMethod(cppConstruct, "on_sample_rejected", "SampleRejectedStatus");
  generateCppMethod(cppConstruct, "on_liveliness_changed",
                    "LivelinessChangedStatus");
  generateOnDataAvailSource(cppConstruct, moduleName, structName);
  generateCppMethod(cppConstruct, "on_subscription_matched",
                    "SubscriptionMatchedStatus");
  generateCppMethod(cppConstruct, "on_sample_lost", "SampleLostStatus ");

  // Generate method declarations in the .h file
  generateClassHeader(headerConstruct, structName);
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
