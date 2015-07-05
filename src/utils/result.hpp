#pragma once

namespace tableauFS
{
  // Wrap the error code type
  using ErrorCode = int;

  enum { NO_ERR = 0 };

  // A POD wrapper for the result of a popssible error operation
  template <typename Val>
  struct Result {
    const ErrorCode err;
    // The value to store
    const Val value;

    bool ok() const { return err == NO_ERR; }
    bool failed() const { return err != NO_ERR; }
  };

  // A POD wrapper for the result of a popssible error operation
  template <>
  struct Result<void> {
    const ErrorCode err;

    bool ok() const { return err == NO_ERR; }
    bool failed() const { return err != NO_ERR; }
  };
}
