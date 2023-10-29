#include <fstream>
#include "Logger.hpp"
#include "Settings.hpp"
#include "FreqCounter.hpp"
#include <map>
#include "ThreadPool.hpp"
#include "SyncPolicy.hpp"
#include <chrono>
int main(int argc, char **argv)
{
    auto time1 = std::chrono::steady_clock::now();
    // парсим командную строку
    auto result = freq::settings_from_cli_arguments(argc, argv);

    if (!result || !result.value.has_value())
    {
        freq::LOG() << "Invalid command line arguments";
        return 0;
    }

    auto settings = std::move(result.value.value());

    // при создании FreqCounter указываем ему мультипоточную стратегию синхронизации
    freq::FreqCounter<> fc;

    // выполнение формирования счетчика частот из файла
    {
        std::ifstream is(settings.input_file, std::ios_base::in);
        auto res = fc.FromStream(is);
        if (!res)
        {
            freq::LOG() << res.err_message;
        }
    }

    // вывод отсортированного счетчика частот элементов в файл
    {
        std::ofstream of(settings.output_file, std::ios_base::out);

        auto res = fc.ToStream(of);
        if (!res)
        {
            freq::LOG() << res.err_message;
        }
    }

    auto time2 = std::chrono::steady_clock::now();

    freq::LOG() << std::chrono::duration_cast<std::chrono::milliseconds>((time2 - time1)).count();
    freq::LOG() << "Complete!";
    return 0;
}
