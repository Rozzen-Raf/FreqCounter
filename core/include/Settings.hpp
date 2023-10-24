#pragma once

#include <string>
#include <optional>
namespace freq
{
    struct Settings
    {
        std::string input_file;
        std::string output_file;

        static const int FIELD_COUNT = 2;
    };

    enum class ParseCLIStatus
    {
        Success,
        InvalidParams
    };

    template <typename T>
    struct ParseCLIResult
    {
        std::optional<T> value;
        ParseCLIStatus status;

        constexpr operator bool() const { return status == ParseCLIStatus::Success; }
    };

    ParseCLIResult<Settings> settings_from_cli_arguments(int argc, char **argv);
}