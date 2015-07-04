#pragma once

/// Macro to mark interfaces
#define MKZ_INTERFACE(name) \
 public:                    \
  virtual ~name() {}

/// Use MKZ_NON_COPYABLE( <Class> ) to mark a class non-copyable.
/// This macro tries to use the correct format for C++11 (clang) and
/// MSVC.
/// TODO: detect C++11 somehow instead of MSVC
#ifdef _MSC_VER
#define MKZ_NON_COPYABLE(className) \
  className(const className&);      \
  className& operator=(const className&);
#else
#define MKZ_NON_COPYABLE(className)     \
  className(const className&) = delete; \
  className& operator=(const className&) = delete;
#endif

#define MKZ_MOVABLE(className)            \
  className(className&& other) = default; \
  className& operator=(className&& other) = default;

/// Mark a parameter as unused and dont pop C4100
#define MKZ_UNUSED(expr) (void)(expr);
#define MKZ_UNUSED_EXPR(expr) \
  do {                        \
    (void)(expr);             \
  } while (0)

/// Macro to mark policy classes
#define MKZ_POLICY(name)  \
 private:                 \
  MKZ_NON_COPYABLE(name); \
                          \
 protected:
