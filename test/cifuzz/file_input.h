#ifndef FILE_INPUT_H
#define FILE_INPUT_H

#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace ci {

/// A temporary file to use as input for the tested library.
///
/// Initialize the object with the data and size parameters from the fuzz test
/// (or, at your choice, only a portion of the data). The initialization will
/// create a temporary file containing the provided data which is cleaned up
/// when the object goes out of scope. The file name, accessed via name(), can
/// be passed to the tested library.
class input_file {
 public:
  /// Create a new input file for the fuzz test containing the provided data.
  input_file(const uint8_t *data, size_t size);
  virtual ~input_file();

  /// Return the name of the temporary file, valid until this object goes out of
  /// scope.
  const char *name() const;

  /// Return a file descriptor for the temporary file, opened for reading. The
  /// descriptor will be cleaned up automatically when this object goes out of
  /// scope. Calling the function repeatedly will return the same descriptor
  /// each time.
  int fd() const;

  /// Return a C file stream for the temporary file, opened for reading. The
  /// file will be closed automatically when this object goes out of scope.
  /// Calling the function repeatedly will return the same file stream each
  /// time.
  FILE *c_file() const;

 private:
  const char *file_name_ = nullptr;

  // These are really just caches, so make them mutable. The const semantics of
  // this class are "the file is not modified".
  mutable int fd_ = -1;
  mutable FILE *c_file_ = nullptr;
};

}  // namespace ci

#endif

