cmake_minimum_required(VERSION 3.3...3.27)
project(opendds_devguide_messenger CXX)
enable_testing()

find_package(OpenDDS REQUIRED)
include(opendds_testing)

# David
# Add the -fPIC flag to the CMake compilation options
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# Make sure the MPC-generated headers are gone so the CMake build will use the
# right ones. This is not needed in a real project.
file(GLOB headers "*.h")
file(GLOB listener_header "DataReaderListenerImpl.h")
list(REMOVE_ITEM headers ${listener_header})
list(LENGTH headers header_count)
if(header_count GREATER 0)
  file(REMOVE ${headers})
endif()

# IDL TypeSupport Library
add_library(messenger_idl)
opendds_target_sources(messenger_idl PUBLIC "Messenger.idl")
target_link_libraries(messenger_idl PUBLIC OpenDDS::Dcps)

set(opendds_libs
  OpenDDS::Dcps # Core OpenDDS Library
  OpenDDS::InfoRepoDiscovery OpenDDS::Tcp # For run_test.pl
  OpenDDS::Rtps OpenDDS::Rtps_Udp # For run_test.pl --rtps
  messenger_idl
)

# Publisher
add_executable(publisher
  Publisher.cpp
)
target_link_libraries(publisher ${opendds_libs})

# Subscriber
add_executable(subscriber
  Subscriber.cpp
  DataReaderListenerImpl.cpp
)

add_library(passLVRefToSharedMem SHARED
  passLVRefToSharedMem.cpp
)
target_link_libraries(passLVRefToSharedMem ${opendds_libs} dl)
add_executable(testlv 
  testlv.cpp
)
target_link_libraries(testlv ${opendds_libs} dl passLVRefToSharedMem)
add_executable(semaphore_receiver 
  SemaphoreReceiver.cpp
)
target_link_libraries(semaphore_receiver ${opendds_libs} dl)
# add_library(subscriber SHARED
#   Subscriber.cpp
#   DataReaderListenerImpl.cpp
# )
target_include_directories(passLVRefToSharedMem PRIVATE ${CMAKE_HOME_DIRECTORY})
target_include_directories(passLVRefToSharedMem PRIVATE /home/admin/OpenDDS/ACE_wrappers/)  # No need for pre.h
target_include_directories(passLVRefToSharedMem PRIVATE /home/admin/OpenDDS/)
target_include_directories(passLVRefToSharedMem PRIVATE /home/admin/OpenDDS/DevGuideExamples/DCPS/Messenger/build/opendds_generated)
target_include_directories(passLVRefToSharedMem PRIVATE /home/admin/OpenDDS/ACE_wrappers/TAO/)
target_link_libraries(subscriber ${opendds_libs} dl)

# Testing
configure_file(run_test.pl . COPYONLY)
configure_file(rtps.ini . COPYONLY)
opendds_add_test(NAME info_repo COMMAND perl run_test.pl)
opendds_add_test(NAME rtps COMMAND perl run_test.pl --rtps)
