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
        TrieNode *GetNode();
        // Вставка слова в префиксное дерево
        TrieNode *InsertToRoot(std::string &&key);
        // Возвращает указатель на найденную ноду, если ноды нет, то возвращает nullptr
        TrieNode *Search(const std::string &key);
        // Очищение префиксного дерева
        void DeleteTrie(TrieNode *node);

    public:
        Trie();
        ~Trie();

        // Вставка слова в дерево
        void Insert(std::string &&key);

        size_t &operator[](std::string &&key);

        // Возвращает корень дерева
        const TrieNode *GetRoot() const noexcept;

    private:
        TrieNode *root;
    };

    // Преобразования дерева в вектор
    std::vector<std::pair<std::string, size_t>> TrieToVector(Trie &trie);
    void FillVector(const TrieNode *node, std::vector<std::pair<std::string, size_t>> &vec);
}