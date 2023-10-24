#include "Settings.hpp"
namespace freq
{
    ParseCLIResult<Settings> settings_from_cli_arguments(int argc, char **argv)
    {
        if (argc != Settings::FIELD_COUNT + 1)
        {
            return {{}, ParseCLIStatus::InvalidParams};
        }

        return {std::optional{Settings{argv[1], argv[2]}}, ParseCLIStatus::Success};
    }
}