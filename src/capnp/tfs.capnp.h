// Generated by Cap'n Proto compiler, DO NOT EDIT
// source: tfs.capnp

#ifndef CAPNP_INCLUDED_e56c22c5df3f0743_
#define CAPNP_INCLUDED_e56c22c5df3f0743_

#include <capnp/generated-header-support.h>

#if CAPNP_VERSION != 5002
#error "Version mismatch between generated code and library headers.  You must use the same version of the Cap'n Proto compiler and library."
#endif


namespace capnp {
namespace schemas {

CAPNP_DECLARE_SCHEMA(b9f7265c923a761f);
CAPNP_DECLARE_SCHEMA(c71c95286510ad5f);
CAPNP_DECLARE_SCHEMA(cb49fd0283e73abb);
CAPNP_DECLARE_SCHEMA(dc3e07e9f74d4e69);

}  // namespace schemas
}  // namespace capnp

namespace tableauFS {
namespace proto {

struct ReaddirReq {
  ReaddirReq() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(b9f7265c923a761f, 0, 1)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand = &schema->defaultBrand;
    #endif  // !CAPNP_LITE
  };
};

struct ReaddirResp {
  ReaddirResp() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(c71c95286510ad5f, 1, 1)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand = &schema->defaultBrand;
    #endif  // !CAPNP_LITE
  };
};

struct GetAttributesReq {
  GetAttributesReq() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(cb49fd0283e73abb, 0, 1)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand = &schema->defaultBrand;
    #endif  // !CAPNP_LITE
  };
};

struct GetAttributesResp {
  GetAttributesResp() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(dc3e07e9f74d4e69, 7, 0)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand = &schema->defaultBrand;
    #endif  // !CAPNP_LITE
  };
};

// =======================================================================================

class ReaddirReq::Reader {
public:
  typedef ReaddirReq Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand);
  }
#endif  // !CAPNP_LITE

  inline bool hasPath() const;
  inline  ::capnp::Text::Reader getPath() const;

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class ReaddirReq::Builder {
public:
  typedef ReaddirReq Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

  inline bool hasPath();
  inline  ::capnp::Text::Builder getPath();
  inline void setPath( ::capnp::Text::Reader value);
  inline  ::capnp::Text::Builder initPath(unsigned int size);
  inline void adoptPath(::capnp::Orphan< ::capnp::Text>&& value);
  inline ::capnp::Orphan< ::capnp::Text> disownPath();

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class ReaddirReq::Pipeline {
public:
  typedef ReaddirReq Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

class ReaddirResp::Reader {
public:
  typedef ReaddirResp Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand);
  }
#endif  // !CAPNP_LITE

  inline  ::int64_t getErr() const;

  inline bool hasEntries() const;
  inline  ::capnp::List< ::capnp::Text>::Reader getEntries() const;

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class ReaddirResp::Builder {
public:
  typedef ReaddirResp Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

  inline  ::int64_t getErr();
  inline void setErr( ::int64_t value);

  inline bool hasEntries();
  inline  ::capnp::List< ::capnp::Text>::Builder getEntries();
  inline void setEntries( ::capnp::List< ::capnp::Text>::Reader value);
  inline void setEntries(::kj::ArrayPtr<const  ::capnp::Text::Reader> value);
  inline  ::capnp::List< ::capnp::Text>::Builder initEntries(unsigned int size);
  inline void adoptEntries(::capnp::Orphan< ::capnp::List< ::capnp::Text>>&& value);
  inline ::capnp::Orphan< ::capnp::List< ::capnp::Text>> disownEntries();

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class ReaddirResp::Pipeline {
public:
  typedef ReaddirResp Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

class GetAttributesReq::Reader {
public:
  typedef GetAttributesReq Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand);
  }
#endif  // !CAPNP_LITE

  inline bool hasPath() const;
  inline  ::capnp::Text::Reader getPath() const;

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class GetAttributesReq::Builder {
public:
  typedef GetAttributesReq Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

  inline bool hasPath();
  inline  ::capnp::Text::Builder getPath();
  inline void setPath( ::capnp::Text::Reader value);
  inline  ::capnp::Text::Builder initPath(unsigned int size);
  inline void adoptPath(::capnp::Orphan< ::capnp::Text>&& value);
  inline ::capnp::Orphan< ::capnp::Text> disownPath();

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class GetAttributesReq::Pipeline {
public:
  typedef GetAttributesReq Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

class GetAttributesResp::Reader {
public:
  typedef GetAttributesResp Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand);
  }
#endif  // !CAPNP_LITE

  inline  ::int64_t getErr() const;

  inline  ::uint64_t getMtime() const;

  inline  ::int64_t getMode() const;

  inline  ::uint64_t getNlink() const;

  inline  ::uint64_t getSize() const;

  inline  ::uint64_t getBlocks() const;

  inline  ::uint64_t getBlksize() const;

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class GetAttributesResp::Builder {
public:
  typedef GetAttributesResp Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

  inline  ::int64_t getErr();
  inline void setErr( ::int64_t value);

  inline  ::uint64_t getMtime();
  inline void setMtime( ::uint64_t value);

  inline  ::int64_t getMode();
  inline void setMode( ::int64_t value);

  inline  ::uint64_t getNlink();
  inline void setNlink( ::uint64_t value);

  inline  ::uint64_t getSize();
  inline void setSize( ::uint64_t value);

  inline  ::uint64_t getBlocks();
  inline void setBlocks( ::uint64_t value);

  inline  ::uint64_t getBlksize();
  inline void setBlksize( ::uint64_t value);

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class GetAttributesResp::Pipeline {
public:
  typedef GetAttributesResp Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

// =======================================================================================

inline bool ReaddirReq::Reader::hasPath() const {
  return !_reader.getPointerField(0 * ::capnp::POINTERS).isNull();
}
inline bool ReaddirReq::Builder::hasPath() {
  return !_builder.getPointerField(0 * ::capnp::POINTERS).isNull();
}
inline  ::capnp::Text::Reader ReaddirReq::Reader::getPath() const {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(
      _reader.getPointerField(0 * ::capnp::POINTERS));
}
inline  ::capnp::Text::Builder ReaddirReq::Builder::getPath() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(
      _builder.getPointerField(0 * ::capnp::POINTERS));
}
inline void ReaddirReq::Builder::setPath( ::capnp::Text::Reader value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::set(
      _builder.getPointerField(0 * ::capnp::POINTERS), value);
}
inline  ::capnp::Text::Builder ReaddirReq::Builder::initPath(unsigned int size) {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::init(
      _builder.getPointerField(0 * ::capnp::POINTERS), size);
}
inline void ReaddirReq::Builder::adoptPath(
    ::capnp::Orphan< ::capnp::Text>&& value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::adopt(
      _builder.getPointerField(0 * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::capnp::Text> ReaddirReq::Builder::disownPath() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::disown(
      _builder.getPointerField(0 * ::capnp::POINTERS));
}

inline  ::int64_t ReaddirResp::Reader::getErr() const {
  return _reader.getDataField< ::int64_t>(
      0 * ::capnp::ELEMENTS);
}

inline  ::int64_t ReaddirResp::Builder::getErr() {
  return _builder.getDataField< ::int64_t>(
      0 * ::capnp::ELEMENTS);
}
inline void ReaddirResp::Builder::setErr( ::int64_t value) {
  _builder.setDataField< ::int64_t>(
      0 * ::capnp::ELEMENTS, value);
}

inline bool ReaddirResp::Reader::hasEntries() const {
  return !_reader.getPointerField(0 * ::capnp::POINTERS).isNull();
}
inline bool ReaddirResp::Builder::hasEntries() {
  return !_builder.getPointerField(0 * ::capnp::POINTERS).isNull();
}
inline  ::capnp::List< ::capnp::Text>::Reader ReaddirResp::Reader::getEntries() const {
  return ::capnp::_::PointerHelpers< ::capnp::List< ::capnp::Text>>::get(
      _reader.getPointerField(0 * ::capnp::POINTERS));
}
inline  ::capnp::List< ::capnp::Text>::Builder ReaddirResp::Builder::getEntries() {
  return ::capnp::_::PointerHelpers< ::capnp::List< ::capnp::Text>>::get(
      _builder.getPointerField(0 * ::capnp::POINTERS));
}
inline void ReaddirResp::Builder::setEntries( ::capnp::List< ::capnp::Text>::Reader value) {
  ::capnp::_::PointerHelpers< ::capnp::List< ::capnp::Text>>::set(
      _builder.getPointerField(0 * ::capnp::POINTERS), value);
}
inline void ReaddirResp::Builder::setEntries(::kj::ArrayPtr<const  ::capnp::Text::Reader> value) {
  ::capnp::_::PointerHelpers< ::capnp::List< ::capnp::Text>>::set(
      _builder.getPointerField(0 * ::capnp::POINTERS), value);
}
inline  ::capnp::List< ::capnp::Text>::Builder ReaddirResp::Builder::initEntries(unsigned int size) {
  return ::capnp::_::PointerHelpers< ::capnp::List< ::capnp::Text>>::init(
      _builder.getPointerField(0 * ::capnp::POINTERS), size);
}
inline void ReaddirResp::Builder::adoptEntries(
    ::capnp::Orphan< ::capnp::List< ::capnp::Text>>&& value) {
  ::capnp::_::PointerHelpers< ::capnp::List< ::capnp::Text>>::adopt(
      _builder.getPointerField(0 * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::capnp::List< ::capnp::Text>> ReaddirResp::Builder::disownEntries() {
  return ::capnp::_::PointerHelpers< ::capnp::List< ::capnp::Text>>::disown(
      _builder.getPointerField(0 * ::capnp::POINTERS));
}

inline bool GetAttributesReq::Reader::hasPath() const {
  return !_reader.getPointerField(0 * ::capnp::POINTERS).isNull();
}
inline bool GetAttributesReq::Builder::hasPath() {
  return !_builder.getPointerField(0 * ::capnp::POINTERS).isNull();
}
inline  ::capnp::Text::Reader GetAttributesReq::Reader::getPath() const {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(
      _reader.getPointerField(0 * ::capnp::POINTERS));
}
inline  ::capnp::Text::Builder GetAttributesReq::Builder::getPath() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(
      _builder.getPointerField(0 * ::capnp::POINTERS));
}
inline void GetAttributesReq::Builder::setPath( ::capnp::Text::Reader value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::set(
      _builder.getPointerField(0 * ::capnp::POINTERS), value);
}
inline  ::capnp::Text::Builder GetAttributesReq::Builder::initPath(unsigned int size) {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::init(
      _builder.getPointerField(0 * ::capnp::POINTERS), size);
}
inline void GetAttributesReq::Builder::adoptPath(
    ::capnp::Orphan< ::capnp::Text>&& value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::adopt(
      _builder.getPointerField(0 * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::capnp::Text> GetAttributesReq::Builder::disownPath() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::disown(
      _builder.getPointerField(0 * ::capnp::POINTERS));
}

inline  ::int64_t GetAttributesResp::Reader::getErr() const {
  return _reader.getDataField< ::int64_t>(
      0 * ::capnp::ELEMENTS);
}

inline  ::int64_t GetAttributesResp::Builder::getErr() {
  return _builder.getDataField< ::int64_t>(
      0 * ::capnp::ELEMENTS);
}
inline void GetAttributesResp::Builder::setErr( ::int64_t value) {
  _builder.setDataField< ::int64_t>(
      0 * ::capnp::ELEMENTS, value);
}

inline  ::uint64_t GetAttributesResp::Reader::getMtime() const {
  return _reader.getDataField< ::uint64_t>(
      1 * ::capnp::ELEMENTS);
}

inline  ::uint64_t GetAttributesResp::Builder::getMtime() {
  return _builder.getDataField< ::uint64_t>(
      1 * ::capnp::ELEMENTS);
}
inline void GetAttributesResp::Builder::setMtime( ::uint64_t value) {
  _builder.setDataField< ::uint64_t>(
      1 * ::capnp::ELEMENTS, value);
}

inline  ::int64_t GetAttributesResp::Reader::getMode() const {
  return _reader.getDataField< ::int64_t>(
      2 * ::capnp::ELEMENTS);
}

inline  ::int64_t GetAttributesResp::Builder::getMode() {
  return _builder.getDataField< ::int64_t>(
      2 * ::capnp::ELEMENTS);
}
inline void GetAttributesResp::Builder::setMode( ::int64_t value) {
  _builder.setDataField< ::int64_t>(
      2 * ::capnp::ELEMENTS, value);
}

inline  ::uint64_t GetAttributesResp::Reader::getNlink() const {
  return _reader.getDataField< ::uint64_t>(
      3 * ::capnp::ELEMENTS);
}

inline  ::uint64_t GetAttributesResp::Builder::getNlink() {
  return _builder.getDataField< ::uint64_t>(
      3 * ::capnp::ELEMENTS);
}
inline void GetAttributesResp::Builder::setNlink( ::uint64_t value) {
  _builder.setDataField< ::uint64_t>(
      3 * ::capnp::ELEMENTS, value);
}

inline  ::uint64_t GetAttributesResp::Reader::getSize() const {
  return _reader.getDataField< ::uint64_t>(
      4 * ::capnp::ELEMENTS);
}

inline  ::uint64_t GetAttributesResp::Builder::getSize() {
  return _builder.getDataField< ::uint64_t>(
      4 * ::capnp::ELEMENTS);
}
inline void GetAttributesResp::Builder::setSize( ::uint64_t value) {
  _builder.setDataField< ::uint64_t>(
      4 * ::capnp::ELEMENTS, value);
}

inline  ::uint64_t GetAttributesResp::Reader::getBlocks() const {
  return _reader.getDataField< ::uint64_t>(
      5 * ::capnp::ELEMENTS);
}

inline  ::uint64_t GetAttributesResp::Builder::getBlocks() {
  return _builder.getDataField< ::uint64_t>(
      5 * ::capnp::ELEMENTS);
}
inline void GetAttributesResp::Builder::setBlocks( ::uint64_t value) {
  _builder.setDataField< ::uint64_t>(
      5 * ::capnp::ELEMENTS, value);
}

inline  ::uint64_t GetAttributesResp::Reader::getBlksize() const {
  return _reader.getDataField< ::uint64_t>(
      6 * ::capnp::ELEMENTS);
}

inline  ::uint64_t GetAttributesResp::Builder::getBlksize() {
  return _builder.getDataField< ::uint64_t>(
      6 * ::capnp::ELEMENTS);
}
inline void GetAttributesResp::Builder::setBlksize( ::uint64_t value) {
  _builder.setDataField< ::uint64_t>(
      6 * ::capnp::ELEMENTS, value);
}

}  // namespace
}  // namespace

#endif  // CAPNP_INCLUDED_e56c22c5df3f0743_
