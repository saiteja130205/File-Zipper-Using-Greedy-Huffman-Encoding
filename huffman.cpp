#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>

using namespace std;

// Node structure for Huffman tree
struct Node {
    char data;
    int freq;
    Node* left;
    Node* right;

    Node(char data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
    Node(int freq) : data('\0'), freq(freq), left(nullptr), right(nullptr) {}
};

// Comparison function for priority queue
struct Compare {
    bool operator()(Node* left, Node* right) {
        return left->freq > right->freq;
    }
};

// Function to build Huffman tree
Node* buildHuffmanTree(const unordered_map<char, int>& freqMap) {
    priority_queue<Node*, vector<Node*>, Compare> minHeap;

    for (const auto& pair : freqMap) {
        minHeap.push(new Node(pair.first, pair.second));
    }

    while (minHeap.size() > 1) {
        Node* left = minHeap.top();
        minHeap.pop();

        Node* right = minHeap.top();
        minHeap.pop();

        Node* internalNode = new Node(left->freq + right->freq);
        internalNode->left = left;
        internalNode->right = right;

        minHeap.push(internalNode);
    }

    return minHeap.top();
}

// Function to build Huffman codes
void buildHuffmanCodes(Node* root, const string& code, unordered_map<char, string>& huffmanCodes) {
    if (!root) {
        return;
    }

    if (root->data != '\0') {
        huffmanCodes[root->data] = code;
    }

    buildHuffmanCodes(root->left, code + "0", huffmanCodes);
    buildHuffmanCodes(root->right, code + "1", huffmanCodes);
}

// Function to write Huffman codes to the output file
void writeHuffmanCodesToFile(const unordered_map<char, string>& huffmanCodes, const string& outputFilePath) {
    ofstream outputFile(outputFilePath);

    for (const auto& pair : huffmanCodes) {
        outputFile << pair.first << " " << pair.second << endl;
    }

    outputFile.close();
}

// Function to encode the input file using Huffman codes and write to the compressed file
void encodeFile(const string& inputFilePath, const unordered_map<char, string>& huffmanCodes, const string& outputFilePath) {
    ifstream inputFile(inputFilePath, ios::binary);
    ofstream compressedFile(outputFilePath, ios::binary);

    char ch;
    string encodedBits;

    while (inputFile.get(ch)) {
        auto it = huffmanCodes.find(ch);
        if (it != huffmanCodes.end()) {
            encodedBits += it->second;
        } else {
            cerr << "Error: Huffman code not found for character '" << ch << "'\n";
            inputFile.close();
            compressedFile.close();
            return;
        }

        while (encodedBits.length() >= 8) {
            char byte = 0;
            for (int i = 0; i < 8; ++i) {
                byte = (byte << 1) | (encodedBits[i] - '0');
            }

            compressedFile.put(byte);
            encodedBits = encodedBits.substr(8);
        }
    }

    // Write remaining bits
    if (!encodedBits.empty()) {
        char byte = 0;
        for (char bit : encodedBits) {
            byte = (byte << 1) | (bit - '0');
        }

        int padding = 8 - encodedBits.length();
        byte = byte << padding;
        compressedFile.put(byte);
    }

    inputFile.close();
    compressedFile.close();
}

int main() {
    string inputFilePath, outputFilePath;
    cout << "Enter the path of the file to compress: ";
    cin >> inputFilePath;

    unordered_map<char, int> freqMap;
    ifstream inputFile(inputFilePath);

    char ch;
    while (inputFile.get(ch)) {
        freqMap[ch]++;
    }

    inputFile.close();

    Node* root = buildHuffmanTree(freqMap);

    unordered_map<char, string> huffmanCodes;
    buildHuffmanCodes(root, "", huffmanCodes);

    cout << "Huffman Codes:\n";
    for (const auto& pair : huffmanCodes) {
        cout << pair.first << ": " << pair.second << endl;
    }

    cout << "Enter the path to save Huffman codes: ";
    cin >> outputFilePath;
    writeHuffmanCodesToFile(huffmanCodes, outputFilePath);

    cout << "Enter the path to save the compressed file: ";
    cin >> outputFilePath;
    encodeFile(inputFilePath, huffmanCodes, outputFilePath);

    cout << "File compressed successfully.\n";

    return 0;
}
