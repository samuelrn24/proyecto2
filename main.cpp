#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

using namespace std;

static int globalNodeId = 0;

// Nodo del árbol de Huffman
struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;
    char min_char;
    int id;

    Node(char c, int f)
        : ch(c), freq(f), left(nullptr), right(nullptr), min_char(c), id(globalNodeId++) {}

    Node(Node* l, Node* r)
        : ch('\0'), freq(l->freq + r->freq), left(l), right(r),
          min_char((l->min_char < r->min_char) ? l->min_char : r->min_char),
          id(globalNodeId++) {}

    bool isLeaf() const { return left == nullptr && right == nullptr; }
};

// Min-heap por frecuencia; en empate, por min_char
struct CompareNodes {
    bool operator()(const Node* a, const Node* b) const {
        if (a->freq != b->freq) return a->freq > b->freq;
        return a->min_char > b->min_char;
    }
};

// Recorre árbol para longitudes y códigos del árbol
static void traverseTree(const Node* node,
                         const string& code,
                         int depth,
                         unordered_map<char, int>& codeLengths,
                         unordered_map<char, string>& treeCodes) {
    if (!node) return;
    if (node->isLeaf()) {
        if (depth == 0) { // caso de un único símbolo
            codeLengths[node->ch] = 1;
            treeCodes[node->ch] = "0";
        } else {
            codeLengths[node->ch] = depth;
            treeCodes[node->ch] = code;
        }
        return;
    }
    traverseTree(node->left, code + '0', depth + 1, codeLengths, treeCodes);
    traverseTree(node->right, code + '1', depth + 1, codeLengths, treeCodes);
}

static string toBitString(uint64_t value, int length) {
    string bits;
    bits.reserve(length);
    for (int i = length - 1; i >= 0; --i) bits.push_back(((value >> i) & 1ULL) ? '1' : '0');
    return bits;
}

// Códigos canónicos a partir de longitudes
static unordered_map<char, string> buildCanonicalCodes(
    const unordered_map<char, int>& codeLengths) {
    vector<pair<char, int>> symbols;
    symbols.reserve(codeLengths.size());
    for (const auto& kv : codeLengths) symbols.emplace_back(kv.first, kv.second);

    sort(symbols.begin(), symbols.end(), [](const auto& a, const auto& b) {
        if (a.second != b.second) return a.second < b.second;
        return a.first < b.first;
    });

    unordered_map<char, string> canonical;
    if (symbols.empty()) return canonical;

    uint64_t currentCode = 0;
    int prevLength = symbols[0].second;
    canonical[symbols[0].first] = toBitString(currentCode, prevLength);

    for (size_t i = 1; i < symbols.size(); ++i) {
        currentCode++;
        int length = symbols[i].second;
        if (length > prevLength) currentCode <<= (length - prevLength);
        canonical[symbols[i].first] = toBitString(currentCode, length);
        prevLength = length;
    }
    return canonical;
}

// Representación textual del árbol (preorden)
static void buildTreeString(const Node* node, string& out) {
    if (!node) return;
    if (node->isLeaf()) {
        if (node->ch == ' ') out += "<sp>";
        else out += node->ch;
    } else {
        out += '(';
        buildTreeString(node->left, out);
        buildTreeString(node->right, out);
        out += ')';
    }
}

// Bits a hexadecimal (padding a nibble)
static string bitsToHex(const string& bits) {
    static const char* hexDigits = "0123456789ABCDEF";
    string hex;
    int n = static_cast<int>(bits.size());
    for (int i = 0; i < n; i += 4) {
        int val = 0;
        for (int j = 0; j < 4; ++j) {
            int idx = i + j;
            val <<= 1;
            if (idx < n && bits[idx] == '1') val |= 1;
        }
        hex.push_back(hexDigits[val]);
    }
    return hex;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string input;
    getline(cin, input);
    if (!input.empty() && input.back() == '\r') input.pop_back();

    if (input.size() < 30) {
        cerr << "Error: Input must contain at least 30 characters." << endl;
        return 1;
    }

    unordered_map<char, int> freqMap;
    for (char c : input) {
        if (c < 32 || c > 126) {
            cerr << "Error: Invalid character detected (ASCII " << static_cast<int>(c) << ")." << endl;
            return 1;
        }
        freqMap[c]++;
    }
    if (freqMap.empty()) {
        cerr << "Error: No valid characters to encode." << endl;
        return 1;
    }

    // Construcción del árbol de Huffman
    Node* root = nullptr;
    {
        priority_queue<Node*, vector<Node*>, CompareNodes> pq;
        vector<char> keys;
        keys.reserve(freqMap.size());
        for (const auto& kv : freqMap) keys.push_back(kv.first);
        sort(keys.begin(), keys.end());
        for (char c : keys) pq.push(new Node(c, freqMap[c]));

        if (pq.size() == 1) {
            root = pq.top();
        } else {
            while (pq.size() > 1) {
                Node* a = pq.top(); pq.pop();
                Node* b = pq.top(); pq.pop();
                pq.push(new Node(a, b));
            }
            root = pq.top();
        }
    }

    unordered_map<char, int> codeLengths;
    unordered_map<char, string> treeCodes;
    traverseTree(root, "", 0, codeLengths, treeCodes);

    auto canonicalCodes = buildCanonicalCodes(codeLengths);

    string compressedBits;
    compressedBits.reserve(input.size() * 8);
    for (char c : input) compressedBits += canonicalCodes[c];

    size_t compressedSize = compressedBits.size();
    size_t originalSize = input.size() * 8;
    double ratio = static_cast<double>(compressedSize) / static_cast<double>(originalSize);
    double reduction = 1.0 - ratio;

    string treeString;
    buildTreeString(root, treeString);

    vector<pair<char, int>> freqList(freqMap.begin(), freqMap.end());
    sort(freqList.begin(), freqList.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

    vector<char> symbols;
    symbols.reserve(codeLengths.size());
    for (const auto& kv : codeLengths) symbols.push_back(kv.first);
    sort(symbols.begin(), symbols.end(), [&](char a, char b) {
        int la = codeLengths.at(a), lb = codeLengths.at(b);
        if (la != lb) return la < lb;
        return a < b;
    });

    cout << "\n=== Huffman Coding Report ===\n";
    cout << "Input length (characters): " << input.size() << "\n";
    cout << "Original size (bits): " << originalSize << "\n";
    cout << "Compressed size (bits): " << compressedSize << "\n";
    cout << fixed << setprecision(4);
    cout << "Compression ratio: " << ratio << "\n";
    cout << "Reduction: " << (reduction * 100.0) << "%\n";

    cout << "\nFrequency table (sorted by symbol):\n";
    cout << "Symbol  ASCII  Freq\n";
    for (const auto& kv : freqList) {
        char c = kv.first;
        cout << setw(4) << (c == ' ' ? string("<sp>") : string(1, c));
        cout << setw(7) << static_cast<int>(c) << setw(7) << kv.second << "\n";
    }

    cout << "\nSymbol details (sorted by code length then symbol):\n";
    cout << "Symbol  Freq  Length  TreeCode  Canonical\n";
    for (char c : symbols) {
        int f = freqMap.at(c);
        int len = codeLengths.at(c);
        string treeCode = treeCodes.at(c);
        string canon = canonicalCodes.at(c);
        cout << setw(4) << (c == ' ' ? string("<sp>") : string(1, c));
        cout << setw(7) << f << setw(8) << len << setw(10) << treeCode << setw(11) << canon << "\n";
    }

    cout << "\nHuffman tree (preorder with parentheses):\n";
    cout << treeString << "\n";

    cout << "\nCompressed output (first 128 bits):\n";
    if (compressedBits.size() <= 128) cout << compressedBits << "\n";
    else cout << compressedBits.substr(0, 128) << "..." << "\n";

    cout << "Total compressed bits: " << compressedSize << "\n";

    string hex = bitsToHex(compressedBits);
    cout << "Compressed output (hex):\n";
    if (hex.size() <= 64) cout << hex << "\n";
    else cout << hex.substr(0, 64) << "..." << "\n";

    // Liberación de memoria
    function<void(Node*)> deleteTree = [&](Node* n) {
        if (!n) return;
        deleteTree(n->left);
        deleteTree(n->right);
        delete n;
    };
    deleteTree(root);
    return 0;
}
