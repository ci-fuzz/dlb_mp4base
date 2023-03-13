#include "file_input.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdexcept>
#include <system_error>

#include "fuzz_utils.h"

namespace ci {

input_file::input_file(const uint8_t *data, size_t size)
    : file_name_(buf_to_file(data, size)) {
  // There's not much we can do when things go wrong here, and it doesn't make
  // sense for the user to continue.
  if (file_name_ == nullptr)
    throw std::runtime_error("failed to create the input file");
}

input_file::~input_file() {
  // There's no point in checking the return values: we shouldn't throw in a
  // destructor...

  delete_file(file_name_);
  if (fd_ != -1) close(fd_);
  if (c_file_ != nullptr) fclose(c_file_);
}

const char *input_file::name() const { return file_name_; }

int input_file::fd() const {
  if (fd_ != -1) return fd_;

  fd_ = open(file_name_, O_RDONLY);
  if (fd_ == -1) {
    throw std::system_error(
        errno, std::generic_category(),
        "failed to create a file descriptor for the input file");
  }

  return fd_;
}

FILE *input_file::c_file() const {
  if (c_file_ != nullptr) return c_file_;

  c_file_ = fopen(file_name_, "r");
  if (c_file_ == nullptr) {
    throw std::system_error(
        errno, std::generic_category(),
        "failed to create a C file stream for the input file");
  }

  return c_file_;
}

}  // namespace ci
