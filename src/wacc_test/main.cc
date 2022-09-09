#include "config.hh"

#include <charconv>
#include <filesystem>
#include <gtest/gtest.h>

class WaccTest : public testing::TestWithParam<typename std::filesystem::recursive_directory_iterator::value_type>
{
  public:
};

class InvalidStageIterator
{
    std::filesystem::recursive_directory_iterator m_it;

    static bool is_implemented_stage(std::filesystem::path path)
    {
        if (std::filesystem::is_directory(path))
        {
            return false;
        }
        for (auto& p : std::filesystem::relative(path, PROJECT_SOURCE_DIR "/test"))
        {
            std::string name = p.string();
            if (name.starts_with("stage_"))
            {
                int stage;
                if (std::from_chars(name.data() + 6, name.data() + name.size(), stage).ec == std::errc{})
                {
                    return stage <= 1;
                }
            }
        }
        return false;
    }

  public:
    InvalidStageIterator(bool) : m_it{PROJECT_SOURCE_DIR "/test"}
    {
        do
        {
            ++m_it;
        } while (m_it != std::filesystem::recursive_directory_iterator{} && !is_implemented_stage(m_it->path()));
    }

    InvalidStageIterator() : m_it{}
    {
    }

    InvalidStageIterator& operator++()
    {
        do
        {
            ++m_it;
        } while (m_it != std::filesystem::recursive_directory_iterator{} && !is_implemented_stage(m_it->path()));
        return *this;
    }

    InvalidStageIterator operator++(int)
    {
        InvalidStageIterator tmp = *this;
        ++*this;
        return tmp;
    }

    bool operator==(const InvalidStageIterator& other) const
    {
        return m_it == other.m_it;
    }

    typename decltype(m_it)::value_type operator*() const
    {
        return *m_it;
    }
};

namespace std
{
template <> struct iterator_traits<InvalidStageIterator>
{
    using difference_type = std::ptrdiff_t;
    using value_type = std::filesystem::recursive_directory_iterator::value_type;
    using pointer = std::filesystem::recursive_directory_iterator::pointer;
    using reference = std::filesystem::recursive_directory_iterator::reference;
    using iterator_category = std::input_iterator_tag;
};
} // namespace std

TEST_P(WaccTest, CompileParityWithGcc)
{
    FAIL() << "Not implemented";
}

INSTANTIATE_TEST_SUITE_P(Invalid, WaccTest, testing::ValuesIn(InvalidStageIterator{true}, InvalidStageIterator{}));
