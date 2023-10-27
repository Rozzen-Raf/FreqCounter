#pragma once

#include <unordered_map>
#include <vector>
#include <iostream>
#include <algorithm>
#include "SyncPolicy.hpp"
#include <queue>

namespace freq
{
    // Статусы выполнения операций с алгоритмом
    enum class Status
    {
        Success,
        StreamNotOpen,
        StreamIsFail,
        StreamIsBad,
        StreamIsEmpty,
    };

    struct Result
    {
        Status status;
        std::string err_message;

        constexpr operator bool() const
        {
            return status == Status::Success;
        }
    };
}
namespace
{
    // Функции десериализации данных из потока данных
    template <typename INPUT_STREAM, typename T, typename SyncPolicy>
    freq::Result deserialize(
        INPUT_STREAM &stream,
        std::unordered_map<T, size_t> &frequency,
        SyncPolicy &sync)
    {
        while (!stream.eof())
        {
            if (stream.fail())
            {
                return {freq::Status::StreamIsFail, "Stream is fail"};
            }
            else if (stream.bad())
            {
                return {freq::Status::StreamIsBad, "Stream is bad"};
            }

            T value;
            stream >> value;
            std::lock_guard<SyncPolicy> guard(sync);
            ++frequency[value];
        }

        return {freq::Status::Success};
    }

    // для строки десериализация происходит с иными условиями
    template <typename INPUT_STREAM, typename SyncPolicy>
    freq::Result deserialize(
        INPUT_STREAM &stream,
        std::unordered_map<std::string, size_t> &frequency,
        SyncPolicy &sync)
    {
        // while (!stream.eof())
        {
            if (stream.fail())
            {
                return {freq::Status::StreamIsFail, "Stream is fail"};
            }
            else if (stream.bad())
            {
                return {freq::Status::StreamIsBad, "Stream is bad"};
            }

            stream.seekg(0, stream.end);
            size_t length = stream.tellg();
            stream.seekg(0, stream.beg);

            if (length == -1)
            {
                return {freq::Status::StreamIsEmpty, "Stream is empty"};
            }

            char *buffer = nullptr;
            try
            {
                buffer = new char[length];
                stream.read(buffer, length);
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << " " << length << '\n';
            }

            std::string str(buffer, length);

            size_t start_word = 0;
            size_t end_word = 0;

            // ищет начало первого встречного слова
            auto find_next_word = [](std::string &str, size_t &start_word, size_t &end_word)
            {
                while (start_word < str.size() && !std::isalpha(str[start_word]))
                {
                    ++start_word;
                }
                end_word = start_word;
            };

            // Подвинем индекс сразу на начало первого слова
            find_next_word(str, start_word, end_word);

            while (end_word < str.size())
            {
                if (!std::isalpha(str[end_word]))
                {
                    // Найден конец слова, необходимо выделить и поместить ответ
                    std::string word = std::move(str.substr(start_word, end_word - start_word));
                    if (!word.empty())
                    {
                        std::transform(word.begin(), word.end(), word.begin(),
                                       [](unsigned char c)
                                       { return std::tolower(c); });

                        std::lock_guard<SyncPolicy> guard(sync);
                        ++frequency[std::move(word)];
                    }
                    start_word = end_word;
                    // Найдем следующее слово
                    find_next_word(str, start_word, end_word);
                }
                ++end_word;
            }

            // Добавим последнее слово
            std::string word = std::move(str.substr(start_word, end_word - start_word));

            if (!word.empty())
            {
                std::transform(word.begin(), word.end(), word.begin(),
                               [](unsigned char c)
                               { return std::tolower(c); });

                std::lock_guard<SyncPolicy> guard(sync);
                ++frequency[std::move(word)];
            }

            delete[] buffer;
        }
        return {freq::Status::Success};
    }

    // Сериализация счетчика частот в выходящий буфер с условием сортировки
    template <typename OUTPUT_STREAM, typename T, typename SyncPolicy>
    freq::Result serialize(
        OUTPUT_STREAM &stream,
        const std::unordered_map<T, size_t> &freq,
        SyncPolicy &policy)
    {
        // Компаратор для построения кучи
        struct Comp
        {
            bool operator()(const std::pair<T, size_t> &l, const std::pair<T, size_t> &r)
            {
                if (l.second != r.second)
                {
                    return l.second > r.second;
                }
                else
                {
                    return l.first < r.first;
                }
            }
        };

        // куча необходима, чтобы вывести элементы в нужном порядке
        std::priority_queue<
            std::pair<T, int>,
            std::vector<std::pair<T, int>>,
            Comp>
            pq;
        {
            std::lock_guard<SyncPolicy> guard(policy);
            // Построение кучи происходит за O(n).
            pq = std::priority_queue<
                std::pair<T, int>,
                std::vector<std::pair<T, int>>,
                Comp>(freq.begin(),
                      freq.end());
        }

        // Выводим все элементы в нужном порядке. Так как куча в себе принимает все элементы хэш-таблицы,
        // То сложность будет O(nlog(n))
        while (!pq.empty())
        {
            if (stream.fail())
            {
                return {freq::Status::StreamIsFail, "Stream is fail"};
            }
            else if (stream.bad())
            {
                return {freq::Status::StreamIsBad, "Stream is bad"};
            }
            auto &ref = pq.top();
            stream << ref.second << " " << ref.first << std::endl;
            pq.pop();
        }
        return {freq::Status::Success};
    }
}

namespace freq
{
    // Подсчет частоты элементов
    template <typename T, typename SyncPolicy = freq::SingleThreadPolicy>
    class FreqCounter
    {
    public:
        // Подсчет частоты элементов из потока данных
        template <typename INPUT_STREAM>
        Result FromStream(INPUT_STREAM &stream)
        {
            return deserialize(stream, freq_map, sync_policy);
        }

        // Запись текущей частоты элементов в поток данных
        template <typename OUTPUT_STREAM>
        Result ToStream(OUTPUT_STREAM &stream)
        {

            return serialize(stream, freq_map, sync_policy);
        }

    private:
        // Подсчет количества элементов хранится в этой хэш-таблице
        std::unordered_map<T, size_t> freq_map;
        // Стратегия синхронизации
        SyncPolicy sync_policy;
    };
}