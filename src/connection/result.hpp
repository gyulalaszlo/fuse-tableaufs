#pragma once

namespace tableauFS
{
  // Wrap the error code type
  using ErrorCode = int;

  enum { NO_ERR = 0 };

  struct ErrorResult {
    // the error code
    const ErrorCode err;

    bool ok() const { return err == NO_ERR; }
    bool failed() const { return err != NO_ERR; }
  };

  // A POD wrapper for the result of a popssible error operation
  template <typename Val>
  struct Result {
    ErrorResult status;
    // The value to store
    const Val value;
  };

  // A POD wrapper for the result of a popssible error operation
  template <>
  struct Result<void> {
    ErrorResult status;
  };
}
