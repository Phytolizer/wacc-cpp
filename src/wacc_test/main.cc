#include "config.hh"
#include "wacc/run.hh"

#include <charconv>
#include <filesystem>
#include <gtest/gtest.h>
#include <subprocess.hpp>

static constexpr int IMPLEMENTED_STAGES = 1;

class StageIterator
{
    std::filesystem::recursive_directory_iterator m_it;

    static bool is_implemented_stage(const std::filesystem::path& path)
    {
        if (std::filesystem::is_directory(path))
        {
            return false;
        }
        for (auto& p : std::filesystem::relative(path, PROJECT_SOURCE_DIR "/test"))
        {
            std::string name = p.string();
            constexpr const char prefix[] = "stage_";
            if (name.starts_with(prefix))
            {
                int stage;
                if (std::from_chars(name.data() + sizeof(prefix) - 1, name.data() + name.size(), stage).ec ==
                    std::errc{})
                {
                    return stage <= IMPLEMENTED_STAGES;
                }
            }
        }
        return false;
    }

    static bool is_valid(const std::filesystem::path& path)
    {
        auto p = std::filesystem::relative(path, PROJECT_SOURCE_DIR "/test");
        return std::find_if(p.begin(), p.end(), [](const auto& p) { return p.string() == "valid"; }) != p.end();
    }

  public:
    struct Value
    {
        typename std::filesystem::recursive_directory_iterator::value_type data;
        bool is_valid;

        friend std::ostream& operator<<(std::ostream& os, const Value& value)
        {
            return os << value.data;
        }
    };

    StageIterator(bool) : m_it{PROJECT_SOURCE_DIR "/test"}
    {
        do
        {
            ++m_it;
        } while (m_it != std::filesystem::recursive_directory_iterator{} && !is_implemented_stage(m_it->path()));
    }

    StageIterator() : m_it{}
    {
    }

    StageIterator& operator++()
    {
        do
        {
            ++m_it;
        } while (m_it != std::filesystem::recursive_directory_iterator{} && !is_implemented_stage(m_it->path()));
        return *this;
    }

    StageIterator operator++(int)
    {
        StageIterator tmp = *this;
        ++*this;
        return tmp;
    }

    bool operator==(const StageIterator& other) const
    {
        return m_it == other.m_it;
    }

    Value operator*() const
    {
        return {*m_it, is_valid(m_it->path())};
    }
};

namespace std
{
template <> struct iterator_traits<StageIterator>
{
    using difference_type = std::ptrdiff_t;
    using value_type = StageIterator::Value;
    using pointer = std::filesystem::recursive_directory_iterator::pointer;
    using reference = std::filesystem::recursive_directory_iterator::reference;
    using iterator_category = std::input_iterator_tag;
};
} // namespace std

class WaccTest : public testing::TestWithParam<typename std::iterator_traits<StageIterator>::value_type>
{
};

TEST_P(WaccTest, CompileParityWithGcc)
{
    std::string input = GetParam().data.path().string();
    std::array args{"wacc", input.c_str(), "-o", "wacc.out"};
    std::ostringstream out;
    std::ostringstream err;
    int wacc_ret = wacc::run(args, out, err);
    if (GetParam().is_valid)
    {
        if (wacc_ret != 0)
        {
            std::cerr << err.str() << '\n';
        }
        EXPECT_EQ(wacc_ret, 0);
        subprocess::CompletedProcess gcc_process = subprocess::run({"gcc", input.c_str(), "-o", "gcc.out"});
        EXPECT_EQ(gcc_process.returncode, 0);

        subprocess::CompletedProcess wacc_out_process = subprocess::run({"./wacc.out"});
        subprocess::CompletedProcess gcc_out_process = subprocess::run({"./gcc.out"});
        EXPECT_EQ(wacc_out_process.returncode, gcc_out_process.returncode);
    }
    else
    {
        EXPECT_NE(wacc_ret, 0);
    }
}

INSTANTIATE_TEST_SUITE_P(Run,
    WaccTest,
    testing::ValuesIn(StageIterator{true}, StageIterator{}),
    [](const testing::TestParamInfo<typename std::iterator_traits<StageIterator>::value_type>& info) {
        std::string name = info.param.data.path().filename().string().replace(
            info.param.data.path().filename().string().find("."), 1, "_");
        return (info.param.is_valid ? "valid_" : "invalid_") + name;
    });
