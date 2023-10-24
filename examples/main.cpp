#include <fstream>
#include "Logger.hpp"
#include "Settings.hpp"
#include "FreqCounter.hpp"
#include <map>
#include "ThreadPool.hpp"
#include "SyncPolicy.hpp"

int main(int argc, char **argv)
{
    // парсим командную строку
    auto result = freq::settings_from_cli_arguments(argc, argv);

    if (!result || !result.value.has_value())
    {
        freq::LOG() << "Invalid command line arguments";
        return 0;
    }

    auto settings = std::move(result.value.value());

    freq::ThreadPool pool(8);

    // при создании FreqCounter указываем ему мультипоточную стратегию синхронизации
    freq::FreqCounter<std::string, freq::MultiThreadPolicy<>> fc;

    // выполнение формирования счетчика частот из файла
    {
        std::ifstream is(settings.input_file, std::ios_base::in);

        auto future = pool.enqueue([&fc, &is]()
                                   { return fc.FromStream(is); });

        auto res = future.get();
        if (!res)
        {
            freq::LOG() << res.err_message;
        }
    }

    // вывод отсортированного счетчика частот элементов в файл
    {
        std::ofstream of(settings.output_file, std::ios_base::out);

        auto future = pool.enqueue([&fc, &of]()
                                   { return fc.ToStream(of); });
        auto res = future.get();
        if (!res)
        {
            freq::LOG() << res.err_message;
        }
    }

    return 0;
}
