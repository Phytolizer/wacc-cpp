#pragma once

#include <filesystem>

namespace fs_ext {

class TemporaryDirectory {
  std::filesystem::path m_path;
  // false when moved from
  bool m_valid;

public:
  explicit TemporaryDirectory(
      std::string_view prefix = "", std::string_view suffix = "");
  ~TemporaryDirectory();
  TemporaryDirectory(const TemporaryDirectory&) = delete;
  TemporaryDirectory& operator=(const TemporaryDirectory&) = delete;
  TemporaryDirectory(TemporaryDirectory&&);
  TemporaryDirectory& operator=(TemporaryDirectory&&);

  [[nodiscard]] const std::filesystem::path& path() const;
};

} // namespace fs_ext
