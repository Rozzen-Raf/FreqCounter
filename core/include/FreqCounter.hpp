#pragma once

#include <unordered_map>
#include <vector>
#include <iostream>
#include <algorithm>
#include "SyncPolicy.hpp"
#include <queue>
#include <iostream>
#include "Trie.hpp"
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
        BadAlloc
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
    inline bool isAlpha(char ch) noexcept
    {
        return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
    }

    inline bool isUpper(char ch) noexcept
    {
        return 'A' <= ch && ch <= 'Z';
    }

    inline char toLower(char ch) noexcept
    {
        return ch - 'A' + 'a';
    }

    // ищет начало первого встречного слова
    inline void findNextWord(std::string_view str, size_t &start_word, size_t &end_word) noexcept
    {
        while (start_word < str.size() && !isAlpha(str[start_word]))
        {
            ++start_word;
        }
        end_word = start_word;
    }
    // для строки десериализация происходит с иными условиями
    template <typename INPUT_STREAM, typename SyncPolicy>
    freq::Result deserialize(
        INPUT_STREAM &stream,
        freq::Trie &trie,
        SyncPolicy &sync) noexcept
    {
        std::string str;
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
            try
            {
                str = std::string(length, ' ');
            }
            catch (...)
            {
                return {freq::Status::BadAlloc, "Bad alloc"};
            }
            stream.read(&str[0], length);
            size_t start_word = 0;
            size_t end_word = 0;

            // Подвинем индекс сразу на начало первого слова
            findNextWord(str, start_word, end_word);

            char ch = str[end_word];
            if (isUpper(ch))
            {
                str[end_word] = toLower(ch);
            }

            while (end_word < str.size())
            {
                auto ch = str[end_word];
                if (!isAlpha(ch))
                {
                    // Найден конец слова, необходимо выделить и поместить ответ
                    std::string word = std::move(str.substr(start_word, end_word - start_word));
                    if (!word.empty())
                    {
                        std::lock_guard<SyncPolicy> guard(sync);
                        ++trie[std::move(word)];
                    }
                    start_word = end_word;
                    // Найдем следующее слово
                    findNextWord(str, start_word, end_word);

                    ch = str[end_word];
                    if (isUpper(ch))
                    {
                        str[end_word] = toLower(ch);
                    }
                }
                else if (isUpper(ch))
                {
                    str[end_word] = toLower(ch);
                }
                ++end_word;
            }

            // Добавим последнее слово
            std::string word = std::move(str.substr(start_word, end_word - start_word));

            if (!word.empty())
            {
                std::lock_guard<SyncPolicy> guard(sync);
                ++trie[std::move(word)];
            }
        }
        return {freq::Status::Success};
    }

    // Сериализация счетчика частот в выходящий буфер с условием сортировки
    template <typename OUTPUT_STREAM, typename SyncPolicy>
    freq::Result serialize(
        OUTPUT_STREAM &stream,
        freq::Trie &trie,
        SyncPolicy &policy) noexcept
    {
        // Компаратор для построения кучи
        struct Comp
        {
            bool operator()(const std::pair<std::string, size_t> &l, const std::pair<std::string, size_t> &r)
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

        std::vector<std::pair<std::string, size_t>> v;
        {
            std::lock_guard<SyncPolicy> guard(policy);
            v = freq::TrieToVector(trie);
        }
        std::sort(v.begin(), v.end(), Comp());

        for (auto ref : v)
        {
            if (stream.fail())
            {
                return {freq::Status::StreamIsFail, "Stream is fail"};
            }
            else if (stream.bad())
            {
                return {freq::Status::StreamIsBad, "Stream is bad"};
            }
            stream << ref.second << " " << ref.first << std::endl;
        }
        return {freq::Status::Success};
    }
}

namespace freq
{
    // Подсчет частоты элементов
    template <typename SyncPolicy = freq::SingleThreadPolicy>
    class FreqCounter
    {
    public:
        // Подсчет частоты элементов из потока данных
        template <typename INPUT_STREAM>
        Result FromStream(INPUT_STREAM &stream)
        {
            return deserialize(stream, trie, sync_policy);
        }

        // Запись текущей частоты элементов в поток данных
        template <typename OUTPUT_STREAM>
        Result ToStream(OUTPUT_STREAM &stream)
        {

            return serialize(stream, trie, sync_policy);
        }

    private:
        // Стратегия синхронизации
        SyncPolicy sync_policy;
        Trie trie;
    };
}