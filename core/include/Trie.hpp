#pragma once

#include <string>
#include <vector>

namespace freq
{
    const int ALPHABET_SIZE = 26;

    // Нода префиксного дерева
    struct TrieNode;

    // Префиксное дерево
    class Trie
    {
        // Возвращает новую ноду префиксного дерева
        TrieNode *getNode();
        // Вставка слова в префиксное дерево
        TrieNode *insert(std::string &&key);
        // Возвращает указатель на найденную ноду, если ноды нет, то возвращает nullptr
        TrieNode *search(const std::string &key);
        // Очищение префиксного дерева
        void deleteTrie(TrieNode *node);

    public:
        Trie();
        ~Trie();
        void Insert(std::string &&key);

        size_t &operator[](std::string &&key);

        const TrieNode *GetRoot() const noexcept;

    private:
        TrieNode *root;
    };

    // Преобразования дерева в вектор
    std::vector<std::pair<std::string, size_t>> TrieToVector(Trie &trie);
    void fillVector(const TrieNode *node, std::vector<std::pair<std::string, size_t>> &vec);
}