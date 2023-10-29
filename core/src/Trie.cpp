#include "Trie.hpp"

namespace freq
{
    struct TrieNode
    {
        TrieNode *children[ALPHABET_SIZE] = {nullptr};

        // флаг конца слова
        bool is_end_of_word;
        size_t count = 0;
        std::string word;
    };
    Trie::Trie() : root(GetNode())
    {
    }

    Trie::~Trie()
    {
        DeleteTrie(root);
    }

    // Возвращает новую ноду префиксного дерева
    TrieNode *Trie::GetNode()
    {
        TrieNode *node = new TrieNode;

        node->is_end_of_word = false;

        return node;
    }

    // Вставка слова в префиксное дерево
    TrieNode *Trie::InsertToRoot(std::string &&key)
    {
        TrieNode *node = root;

        for (int i = 0; i < key.length(); ++i)
        {
            int index = key[i] - 'a';
            if (!node->children[index])
                node->children[index] = GetNode();

            node = node->children[index];
        }

        // Помечаем последний узел как лист
        node->is_end_of_word = true;
        node->count = 0;
        node->word = std::move(key);

        return node;
    }

    // Возвращает указатель на найденную ноду, если ноды нет, то возвращает nullptr
    TrieNode *Trie::Search(const std::string &key)
    {
        TrieNode *node = root;

        for (int i = 0; i < key.length(); ++i)
        {
            int index = key[i] - 'a';
            if (!node->children[index])
                return nullptr;

            node = node->children[index];
        }
        return node->is_end_of_word ? node : nullptr;
    }

    // Очищение префиксного дерева
    void Trie::DeleteTrie(TrieNode *node)
    {
        if (!node)
            return;

        for (int i = 0; i < ALPHABET_SIZE; ++i)
        {
            if (!node->children[i])
                continue;

            DeleteTrie(node->children[i]);
        }

        delete node;
        node = nullptr;
    }

    const TrieNode *Trie::GetRoot() const noexcept
    {
        return root;
    }

    void Trie::Insert(std::string &&key)
    {
        InsertToRoot(std::forward<std::string>(key));
    }

    size_t &Trie::operator[](std::string &&key)
    {
        auto node = Search(key);

        if (!node)
            node = InsertToRoot(std::forward<std::string>(key));

        return node->count;
    }

    // Преобразования дерева в вектор
    std::vector<std::pair<std::string, size_t>> TrieToVector(Trie &trie)
    {
        std::vector<std::pair<std::string, size_t>> v;
        FillVector(trie.GetRoot(), v);
        return std::move(v);
    }

    void FillVector(const TrieNode *node, std::vector<std::pair<std::string, size_t>> &vec)
    {
        if (!node)
            return;

        if (node->is_end_of_word)
        {
            vec.emplace_back(node->word, node->count);
        }

        for (int i = 0; i < ALPHABET_SIZE; ++i)
        {
            if (!node->children[i])
                continue;

            FillVector(node->children[i], vec);
        }
    }
}