#ifndef OPENDDS_TRANSPORT_TEST_MSG_H
#define OPENDDS_TRANSPORT_TEST_MSG_H

#include "TestMsg_Export.h"

#include <dds/DCPS/Serializer.h>
#include <dds/DCPS/TypeSupportImpl.h>

struct TestMsg_Export TestMsg {
  ACE_CDR::ULong key;
  TAO::String_Manager value;

  TestMsg();
  TestMsg(ACE_CDR::ULong msgKey, TAO::String_Manager msgValue);
};

OPENDDS_BEGIN_VERSIONED_NAMESPACE_DECL
namespace OpenDDS {
namespace DCPS {

TestMsg_Export
bool max_serialized_size(const Encoding&, size_t&, const TestMsg&);

TestMsg_Export
void serialized_size(
  const Encoding& encoding, size_t& size, const TestMsg& stru);

TestMsg_Export
bool operator<<(Serializer& strm, const TestMsg& stru);

TestMsg_Export
bool operator>>(Serializer& strm, TestMsg& stru);

TestMsg_Export
bool max_serialized_size(
  const Encoding& encoding, size_t& size, KeyOnly<const TestMsg> stru);

TestMsg_Export
void serialized_size(
  const Encoding& encoding, size_t& size, KeyOnly<const TestMsg> stru);

TestMsg_Export
bool operator<<(Serializer& strm, KeyOnly<const TestMsg> stru);

TestMsg_Export
bool operator>>(Serializer& strm, KeyOnly<TestMsg> stru);

template <>
struct MarshalTraits<TestMsg> {
  static bool gen_is_bounded_size() { return false; }
  static bool gen_is_bounded_key_size() { return true; }
};

}
}
OPENDDS_END_VERSIONED_NAMESPACE_DECL

#endif /* OPENDDS_TRANSPORT_TEST_MSG_H */
