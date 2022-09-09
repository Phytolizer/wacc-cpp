#include "fs_ext/temporary_directory.hpp"

#include <random>

constexpr std::string_view CHARSET = "abcdefghijklmnopqrstuvwxyz"
                                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                     "0123456789";

fs_ext::TemporaryDirectory::TemporaryDirectory(
    std::string_view prefix, std::string_view suffix)
    : m_valid(true) {
  std::random_device rd;
  std::default_random_engine eng{rd()};

  constexpr std::size_t templateLen = 8;
  std::string templ =
      std::string{prefix} + std::string(templateLen, ' ') + std::string{suffix};
  auto dist = std::uniform_int_distribution<std::size_t>{0, CHARSET.size() - 1};

  for (std::size_t attempt = 0; attempt < TMP_MAX; attempt++) {
    for (std::size_t i = prefix.size(); i < prefix.size() + templateLen; ++i) {
      templ[i] = CHARSET[dist(eng)];
    }
    m_path = std::filesystem::temp_directory_path() / templ;
    std::error_code ec;
    std::filesystem::create_directories(m_path, ec);
    if (ec == std::errc{}) {
      return;
    }
  }

  throw std::runtime_error("Failed to create temporary directory");
}

fs_ext::TemporaryDirectory::~TemporaryDirectory() {
  if (m_valid) {
    std::filesystem::remove_all(m_path);
  }
}

fs_ext::TemporaryDirectory::TemporaryDirectory(TemporaryDirectory&& o)
    : m_path{std::move(o.m_path)}, m_valid{o.m_valid} {
  o.m_valid = false;
}

fs_ext::TemporaryDirectory& fs_ext::TemporaryDirectory::operator=(
    TemporaryDirectory&& o) {
  if (&o != this) {
    if (m_valid) {
      std::filesystem::remove_all(m_path);
    }
    m_path = std::move(o.m_path);
    m_valid = o.m_valid;
  }
  return *this;
}

const std::filesystem::path& fs_ext::TemporaryDirectory::path() const {
  return m_path;
}
