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
    Trie::Trie() : root(getNode())
    {
    }

    Trie::~Trie()
    {
        deleteTrie(root);
    }

    // Возвращает новую ноду префиксного дерева
    TrieNode *Trie::getNode()
    {
        TrieNode *node = new TrieNode;

        node->is_end_of_word = false;

        return node;
    }

    // Вставка слова в префиксное дерево
    TrieNode *Trie::insert(std::string &&key)
    {
        TrieNode *node = root;

        for (int i = 0; i < key.length(); ++i)
        {
            int index = key[i] - 'a';
            if (!node->children[index])
                node->children[index] = getNode();

            node = node->children[index];
        }

        // Помечаем последний узел как лист
        node->is_end_of_word = true;
        node->count = 0;
        node->word = std::move(key);

        return node;
    }

    // Возвращает указатель на найденную ноду, если ноды нет, то возвращает nullptr
    TrieNode *Trie::search(const std::string &key)
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
    void Trie::deleteTrie(TrieNode *node)
    {
        if (!node)
            return;

        for (int i = 0; i < ALPHABET_SIZE; ++i)
        {
            if (!node->children[i])
                continue;

            deleteTrie(node->children[i]);
        }

        delete node;
    }

    const TrieNode *Trie::GetRoot() const noexcept
    {
        return root;
    }

    void Trie::Insert(std::string &&key)
    {
        insert(std::forward<std::string>(key));
    }

    size_t &Trie::operator[](std::string &&key)
    {
        auto node = search(key);

        if (!node)
            node = insert(std::forward<std::string>(key));

        return node->count;
    }

    // Преобразования дерева в вектор
    std::vector<std::pair<std::string, size_t>> TrieToVector(Trie &trie)
    {
        std::vector<std::pair<std::string, size_t>> v;
        fillVector(trie.GetRoot(), v);
        return std::move(v);
    }

    void fillVector(const TrieNode *node, std::vector<std::pair<std::string, size_t>> &vec)
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

            fillVector(node->children[i], vec);
        }
    }
}