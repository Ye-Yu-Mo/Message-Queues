// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: msg.proto

#include "msg.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG

namespace _pb = ::PROTOBUF_NAMESPACE_ID;
namespace _pbi = _pb::internal;

namespace XuMQ {
PROTOBUF_CONSTEXPR BasicProperties::BasicProperties(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.id_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.routing_key_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.delivery_mode_)*/0
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct BasicPropertiesDefaultTypeInternal {
  PROTOBUF_CONSTEXPR BasicPropertiesDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~BasicPropertiesDefaultTypeInternal() {}
  union {
    BasicProperties _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 BasicPropertiesDefaultTypeInternal _BasicProperties_default_instance_;
PROTOBUF_CONSTEXPR Message_Payload::Message_Payload(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.body_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.properties_)*/nullptr
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct Message_PayloadDefaultTypeInternal {
  PROTOBUF_CONSTEXPR Message_PayloadDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~Message_PayloadDefaultTypeInternal() {}
  union {
    Message_Payload _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 Message_PayloadDefaultTypeInternal _Message_Payload_default_instance_;
PROTOBUF_CONSTEXPR Message::Message(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.valid_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.payload_)*/nullptr
  , /*decltype(_impl_.offset_)*/0u
  , /*decltype(_impl_.length_)*/0u
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct MessageDefaultTypeInternal {
  PROTOBUF_CONSTEXPR MessageDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~MessageDefaultTypeInternal() {}
  union {
    Message _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 MessageDefaultTypeInternal _Message_default_instance_;
}  // namespace XuMQ
static ::_pb::Metadata file_level_metadata_msg_2eproto[3];
static const ::_pb::EnumDescriptor* file_level_enum_descriptors_msg_2eproto[2];
static constexpr ::_pb::ServiceDescriptor const** file_level_service_descriptors_msg_2eproto = nullptr;

const uint32_t TableStruct_msg_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::XuMQ::BasicProperties, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::XuMQ::BasicProperties, _impl_.id_),
  PROTOBUF_FIELD_OFFSET(::XuMQ::BasicProperties, _impl_.delivery_mode_),
  PROTOBUF_FIELD_OFFSET(::XuMQ::BasicProperties, _impl_.routing_key_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::XuMQ::Message_Payload, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::XuMQ::Message_Payload, _impl_.properties_),
  PROTOBUF_FIELD_OFFSET(::XuMQ::Message_Payload, _impl_.body_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::XuMQ::Message, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::XuMQ::Message, _impl_.payload_),
  PROTOBUF_FIELD_OFFSET(::XuMQ::Message, _impl_.offset_),
  PROTOBUF_FIELD_OFFSET(::XuMQ::Message, _impl_.length_),
  PROTOBUF_FIELD_OFFSET(::XuMQ::Message, _impl_.valid_),
};
static const ::_pbi::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, -1, sizeof(::XuMQ::BasicProperties)},
  { 9, -1, -1, sizeof(::XuMQ::Message_Payload)},
  { 17, -1, -1, sizeof(::XuMQ::Message)},
};

static const ::_pb::Message* const file_default_instances[] = {
  &::XuMQ::_BasicProperties_default_instance_._instance,
  &::XuMQ::_Message_Payload_default_instance_._instance,
  &::XuMQ::_Message_default_instance_._instance,
};

const char descriptor_table_protodef_msg_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\tmsg.proto\022\004XuMQ\"]\n\017BasicProperties\022\n\n\002"
  "id\030\001 \001(\t\022)\n\rdelivery_mode\030\002 \001(\0162\022.XuMQ.D"
  "eliveryMode\022\023\n\013routing_key\030\003 \001(\t\"\244\001\n\007Mes"
  "sage\022&\n\007payload\030\001 \001(\0132\025.XuMQ.Message.Pay"
  "load\022\016\n\006offset\030\002 \001(\r\022\016\n\006length\030\003 \001(\r\022\r\n\005"
  "valid\030\004 \001(\t\032B\n\007Payload\022)\n\nproperties\030\001 \001"
  "(\0132\025.XuMQ.BasicProperties\022\014\n\004body\030\002 \001(\t*"
  "A\n\014ExchangeType\022\016\n\nUNKNOWTYPE\020\000\022\n\n\006DIREC"
  "T\020\001\022\n\n\006FANOUT\020\002\022\t\n\005TOPIC\020\003*:\n\014DeliveryMo"
  "de\022\016\n\nUNKNOWMODE\020\000\022\r\n\tUNDURABLE\020\001\022\013\n\007DUR"
  "ABLE\020\002b\006proto3"
  ;
static ::_pbi::once_flag descriptor_table_msg_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_msg_2eproto = {
    false, false, 414, descriptor_table_protodef_msg_2eproto,
    "msg.proto",
    &descriptor_table_msg_2eproto_once, nullptr, 0, 3,
    schemas, file_default_instances, TableStruct_msg_2eproto::offsets,
    file_level_metadata_msg_2eproto, file_level_enum_descriptors_msg_2eproto,
    file_level_service_descriptors_msg_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_msg_2eproto_getter() {
  return &descriptor_table_msg_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_msg_2eproto(&descriptor_table_msg_2eproto);
namespace XuMQ {
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ExchangeType_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_msg_2eproto);
  return file_level_enum_descriptors_msg_2eproto[0];
}
bool ExchangeType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
      return true;
    default:
      return false;
  }
}

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* DeliveryMode_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_msg_2eproto);
  return file_level_enum_descriptors_msg_2eproto[1];
}
bool DeliveryMode_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}


// ===================================================================

class BasicProperties::_Internal {
 public:
};

BasicProperties::BasicProperties(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:XuMQ.BasicProperties)
}
BasicProperties::BasicProperties(const BasicProperties& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  BasicProperties* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.id_){}
    , decltype(_impl_.routing_key_){}
    , decltype(_impl_.delivery_mode_){}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  _impl_.id_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.id_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_id().empty()) {
    _this->_impl_.id_.Set(from._internal_id(), 
      _this->GetArenaForAllocation());
  }
  _impl_.routing_key_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.routing_key_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_routing_key().empty()) {
    _this->_impl_.routing_key_.Set(from._internal_routing_key(), 
      _this->GetArenaForAllocation());
  }
  _this->_impl_.delivery_mode_ = from._impl_.delivery_mode_;
  // @@protoc_insertion_point(copy_constructor:XuMQ.BasicProperties)
}

inline void BasicProperties::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.id_){}
    , decltype(_impl_.routing_key_){}
    , decltype(_impl_.delivery_mode_){0}
    , /*decltype(_impl_._cached_size_)*/{}
  };
  _impl_.id_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.id_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  _impl_.routing_key_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.routing_key_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
}

BasicProperties::~BasicProperties() {
  // @@protoc_insertion_point(destructor:XuMQ.BasicProperties)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void BasicProperties::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  _impl_.id_.Destroy();
  _impl_.routing_key_.Destroy();
}

void BasicProperties::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void BasicProperties::Clear() {
// @@protoc_insertion_point(message_clear_start:XuMQ.BasicProperties)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.id_.ClearToEmpty();
  _impl_.routing_key_.ClearToEmpty();
  _impl_.delivery_mode_ = 0;
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* BasicProperties::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // string id = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 10)) {
          auto str = _internal_mutable_id();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, "XuMQ.BasicProperties.id"));
        } else
          goto handle_unusual;
        continue;
      // .XuMQ.DeliveryMode delivery_mode = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 16)) {
          uint64_t val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          _internal_set_delivery_mode(static_cast<::XuMQ::DeliveryMode>(val));
        } else
          goto handle_unusual;
        continue;
      // string routing_key = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 26)) {
          auto str = _internal_mutable_routing_key();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, "XuMQ.BasicProperties.routing_key"));
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* BasicProperties::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:XuMQ.BasicProperties)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // string id = 1;
  if (!this->_internal_id().empty()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_id().data(), static_cast<int>(this->_internal_id().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "XuMQ.BasicProperties.id");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_id(), target);
  }

  // .XuMQ.DeliveryMode delivery_mode = 2;
  if (this->_internal_delivery_mode() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteEnumToArray(
      2, this->_internal_delivery_mode(), target);
  }

  // string routing_key = 3;
  if (!this->_internal_routing_key().empty()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_routing_key().data(), static_cast<int>(this->_internal_routing_key().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "XuMQ.BasicProperties.routing_key");
    target = stream->WriteStringMaybeAliased(
        3, this->_internal_routing_key(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:XuMQ.BasicProperties)
  return target;
}

size_t BasicProperties::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:XuMQ.BasicProperties)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string id = 1;
  if (!this->_internal_id().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_id());
  }

  // string routing_key = 3;
  if (!this->_internal_routing_key().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_routing_key());
  }

  // .XuMQ.DeliveryMode delivery_mode = 2;
  if (this->_internal_delivery_mode() != 0) {
    total_size += 1 +
      ::_pbi::WireFormatLite::EnumSize(this->_internal_delivery_mode());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData BasicProperties::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    BasicProperties::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*BasicProperties::GetClassData() const { return &_class_data_; }


void BasicProperties::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<BasicProperties*>(&to_msg);
  auto& from = static_cast<const BasicProperties&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:XuMQ.BasicProperties)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (!from._internal_id().empty()) {
    _this->_internal_set_id(from._internal_id());
  }
  if (!from._internal_routing_key().empty()) {
    _this->_internal_set_routing_key(from._internal_routing_key());
  }
  if (from._internal_delivery_mode() != 0) {
    _this->_internal_set_delivery_mode(from._internal_delivery_mode());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void BasicProperties::CopyFrom(const BasicProperties& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:XuMQ.BasicProperties)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool BasicProperties::IsInitialized() const {
  return true;
}

void BasicProperties::InternalSwap(BasicProperties* other) {
  using std::swap;
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.id_, lhs_arena,
      &other->_impl_.id_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.routing_key_, lhs_arena,
      &other->_impl_.routing_key_, rhs_arena
  );
  swap(_impl_.delivery_mode_, other->_impl_.delivery_mode_);
}

::PROTOBUF_NAMESPACE_ID::Metadata BasicProperties::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_msg_2eproto_getter, &descriptor_table_msg_2eproto_once,
      file_level_metadata_msg_2eproto[0]);
}

// ===================================================================

class Message_Payload::_Internal {
 public:
  static const ::XuMQ::BasicProperties& properties(const Message_Payload* msg);
};

const ::XuMQ::BasicProperties&
Message_Payload::_Internal::properties(const Message_Payload* msg) {
  return *msg->_impl_.properties_;
}
Message_Payload::Message_Payload(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:XuMQ.Message.Payload)
}
Message_Payload::Message_Payload(const Message_Payload& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  Message_Payload* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.body_){}
    , decltype(_impl_.properties_){nullptr}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  _impl_.body_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.body_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_body().empty()) {
    _this->_impl_.body_.Set(from._internal_body(), 
      _this->GetArenaForAllocation());
  }
  if (from._internal_has_properties()) {
    _this->_impl_.properties_ = new ::XuMQ::BasicProperties(*from._impl_.properties_);
  }
  // @@protoc_insertion_point(copy_constructor:XuMQ.Message.Payload)
}

inline void Message_Payload::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.body_){}
    , decltype(_impl_.properties_){nullptr}
    , /*decltype(_impl_._cached_size_)*/{}
  };
  _impl_.body_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.body_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
}

Message_Payload::~Message_Payload() {
  // @@protoc_insertion_point(destructor:XuMQ.Message.Payload)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void Message_Payload::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  _impl_.body_.Destroy();
  if (this != internal_default_instance()) delete _impl_.properties_;
}

void Message_Payload::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void Message_Payload::Clear() {
// @@protoc_insertion_point(message_clear_start:XuMQ.Message.Payload)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.body_.ClearToEmpty();
  if (GetArenaForAllocation() == nullptr && _impl_.properties_ != nullptr) {
    delete _impl_.properties_;
  }
  _impl_.properties_ = nullptr;
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* Message_Payload::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // .XuMQ.BasicProperties properties = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 10)) {
          ptr = ctx->ParseMessage(_internal_mutable_properties(), ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // string body = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 18)) {
          auto str = _internal_mutable_body();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, "XuMQ.Message.Payload.body"));
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* Message_Payload::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:XuMQ.Message.Payload)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // .XuMQ.BasicProperties properties = 1;
  if (this->_internal_has_properties()) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(1, _Internal::properties(this),
        _Internal::properties(this).GetCachedSize(), target, stream);
  }

  // string body = 2;
  if (!this->_internal_body().empty()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_body().data(), static_cast<int>(this->_internal_body().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "XuMQ.Message.Payload.body");
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_body(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:XuMQ.Message.Payload)
  return target;
}

size_t Message_Payload::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:XuMQ.Message.Payload)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string body = 2;
  if (!this->_internal_body().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_body());
  }

  // .XuMQ.BasicProperties properties = 1;
  if (this->_internal_has_properties()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *_impl_.properties_);
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData Message_Payload::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    Message_Payload::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*Message_Payload::GetClassData() const { return &_class_data_; }


void Message_Payload::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<Message_Payload*>(&to_msg);
  auto& from = static_cast<const Message_Payload&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:XuMQ.Message.Payload)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (!from._internal_body().empty()) {
    _this->_internal_set_body(from._internal_body());
  }
  if (from._internal_has_properties()) {
    _this->_internal_mutable_properties()->::XuMQ::BasicProperties::MergeFrom(
        from._internal_properties());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void Message_Payload::CopyFrom(const Message_Payload& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:XuMQ.Message.Payload)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Message_Payload::IsInitialized() const {
  return true;
}

void Message_Payload::InternalSwap(Message_Payload* other) {
  using std::swap;
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.body_, lhs_arena,
      &other->_impl_.body_, rhs_arena
  );
  swap(_impl_.properties_, other->_impl_.properties_);
}

::PROTOBUF_NAMESPACE_ID::Metadata Message_Payload::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_msg_2eproto_getter, &descriptor_table_msg_2eproto_once,
      file_level_metadata_msg_2eproto[1]);
}

// ===================================================================

class Message::_Internal {
 public:
  static const ::XuMQ::Message_Payload& payload(const Message* msg);
};

const ::XuMQ::Message_Payload&
Message::_Internal::payload(const Message* msg) {
  return *msg->_impl_.payload_;
}
Message::Message(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:XuMQ.Message)
}
Message::Message(const Message& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  Message* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.valid_){}
    , decltype(_impl_.payload_){nullptr}
    , decltype(_impl_.offset_){}
    , decltype(_impl_.length_){}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  _impl_.valid_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.valid_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_valid().empty()) {
    _this->_impl_.valid_.Set(from._internal_valid(), 
      _this->GetArenaForAllocation());
  }
  if (from._internal_has_payload()) {
    _this->_impl_.payload_ = new ::XuMQ::Message_Payload(*from._impl_.payload_);
  }
  ::memcpy(&_impl_.offset_, &from._impl_.offset_,
    static_cast<size_t>(reinterpret_cast<char*>(&_impl_.length_) -
    reinterpret_cast<char*>(&_impl_.offset_)) + sizeof(_impl_.length_));
  // @@protoc_insertion_point(copy_constructor:XuMQ.Message)
}

inline void Message::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.valid_){}
    , decltype(_impl_.payload_){nullptr}
    , decltype(_impl_.offset_){0u}
    , decltype(_impl_.length_){0u}
    , /*decltype(_impl_._cached_size_)*/{}
  };
  _impl_.valid_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.valid_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
}

Message::~Message() {
  // @@protoc_insertion_point(destructor:XuMQ.Message)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void Message::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  _impl_.valid_.Destroy();
  if (this != internal_default_instance()) delete _impl_.payload_;
}

void Message::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void Message::Clear() {
// @@protoc_insertion_point(message_clear_start:XuMQ.Message)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.valid_.ClearToEmpty();
  if (GetArenaForAllocation() == nullptr && _impl_.payload_ != nullptr) {
    delete _impl_.payload_;
  }
  _impl_.payload_ = nullptr;
  ::memset(&_impl_.offset_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&_impl_.length_) -
      reinterpret_cast<char*>(&_impl_.offset_)) + sizeof(_impl_.length_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* Message::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // .XuMQ.Message.Payload payload = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 10)) {
          ptr = ctx->ParseMessage(_internal_mutable_payload(), ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // uint32 offset = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 16)) {
          _impl_.offset_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // uint32 length = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 24)) {
          _impl_.length_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // string valid = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 34)) {
          auto str = _internal_mutable_valid();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, "XuMQ.Message.valid"));
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* Message::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:XuMQ.Message)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // .XuMQ.Message.Payload payload = 1;
  if (this->_internal_has_payload()) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(1, _Internal::payload(this),
        _Internal::payload(this).GetCachedSize(), target, stream);
  }

  // uint32 offset = 2;
  if (this->_internal_offset() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteUInt32ToArray(2, this->_internal_offset(), target);
  }

  // uint32 length = 3;
  if (this->_internal_length() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteUInt32ToArray(3, this->_internal_length(), target);
  }

  // string valid = 4;
  if (!this->_internal_valid().empty()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_valid().data(), static_cast<int>(this->_internal_valid().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "XuMQ.Message.valid");
    target = stream->WriteStringMaybeAliased(
        4, this->_internal_valid(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:XuMQ.Message)
  return target;
}

size_t Message::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:XuMQ.Message)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string valid = 4;
  if (!this->_internal_valid().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_valid());
  }

  // .XuMQ.Message.Payload payload = 1;
  if (this->_internal_has_payload()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *_impl_.payload_);
  }

  // uint32 offset = 2;
  if (this->_internal_offset() != 0) {
    total_size += ::_pbi::WireFormatLite::UInt32SizePlusOne(this->_internal_offset());
  }

  // uint32 length = 3;
  if (this->_internal_length() != 0) {
    total_size += ::_pbi::WireFormatLite::UInt32SizePlusOne(this->_internal_length());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData Message::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    Message::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*Message::GetClassData() const { return &_class_data_; }


void Message::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<Message*>(&to_msg);
  auto& from = static_cast<const Message&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:XuMQ.Message)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (!from._internal_valid().empty()) {
    _this->_internal_set_valid(from._internal_valid());
  }
  if (from._internal_has_payload()) {
    _this->_internal_mutable_payload()->::XuMQ::Message_Payload::MergeFrom(
        from._internal_payload());
  }
  if (from._internal_offset() != 0) {
    _this->_internal_set_offset(from._internal_offset());
  }
  if (from._internal_length() != 0) {
    _this->_internal_set_length(from._internal_length());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void Message::CopyFrom(const Message& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:XuMQ.Message)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Message::IsInitialized() const {
  return true;
}

void Message::InternalSwap(Message* other) {
  using std::swap;
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.valid_, lhs_arena,
      &other->_impl_.valid_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(Message, _impl_.length_)
      + sizeof(Message::_impl_.length_)
      - PROTOBUF_FIELD_OFFSET(Message, _impl_.payload_)>(
          reinterpret_cast<char*>(&_impl_.payload_),
          reinterpret_cast<char*>(&other->_impl_.payload_));
}

::PROTOBUF_NAMESPACE_ID::Metadata Message::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_msg_2eproto_getter, &descriptor_table_msg_2eproto_once,
      file_level_metadata_msg_2eproto[2]);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace XuMQ
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::XuMQ::BasicProperties*
Arena::CreateMaybeMessage< ::XuMQ::BasicProperties >(Arena* arena) {
  return Arena::CreateMessageInternal< ::XuMQ::BasicProperties >(arena);
}
template<> PROTOBUF_NOINLINE ::XuMQ::Message_Payload*
Arena::CreateMaybeMessage< ::XuMQ::Message_Payload >(Arena* arena) {
  return Arena::CreateMessageInternal< ::XuMQ::Message_Payload >(arena);
}
template<> PROTOBUF_NOINLINE ::XuMQ::Message*
Arena::CreateMaybeMessage< ::XuMQ::Message >(Arena* arena) {
  return Arena::CreateMessageInternal< ::XuMQ::Message >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>