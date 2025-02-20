#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <sstream>

using namespace std;

const int MAX_WORD_LEN = 7;

// 加载字典文件
unordered_set<string> loadDictionary(const string& path) {
    unordered_set<string> dict;
    ifstream file(path);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                dict.insert(line);
            }
        }
        file.close();
    }
    return dict;
}

// 加载停用词表
unordered_set<string> loadStopwords(const string& path) {
    unordered_set<string> stopwords;
    ifstream file(path);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                stopwords.insert(line);
            }
        }
        file.close();
    }
    return stopwords;
}

// UTF-8字符分割
vector<string> splitUTF8(const string& str) {
    vector<string> chars;
    for (size_t i = 0; i < str.size();) {
        int charLen = 1;
        if ((str[i] & 0xF0) == 0xE0) charLen = 3;
        else if ((str[i] & 0xE0) == 0xC0) charLen = 2;
        chars.push_back(str.substr(i, charLen));
        i += charLen;
    }
    return chars;
}

// 最大正向匹配分词
vector<string> segmentSentence(const vector<string>& chars,
                              const unordered_set<string>& dict) {
    vector<string> words;
    int n = chars.size();
    int i = 0;

    while (i < n) {
        int maxLen = min(MAX_WORD_LEN, n - i);
        bool found = false;

        while (maxLen > 0) {
            string candidate;
            for (int j = i; j < i + maxLen; ++j) {
                candidate += chars[j];
            }

            if (dict.count(candidate)) {
                words.push_back(candidate);
                i += maxLen;
                found = true;
                break;
            }

            if (maxLen == 1) {
                words.push_back(candidate);
                i += 1;
                found = true;
                break;
            }

            maxLen--;
        }

        if (!found) i++;
    }
    return words;
}

int main() {
    // 加载数据
    auto dict = loadDictionary("../corpus.dict.txt");
    auto stopwords = loadStopwords("../cn_stopwords.txt");

    // 处理输入文件
    ifstream input("../corpus.sentence.txt");
    unordered_map<string, int> freq;
    int total = 0;

    string line;
    while (getline(input, line)) {
        vector<string> chars = splitUTF8(line);
        vector<string> words = segmentSentence(chars, dict);

        // 统计词频（过滤停用词和标点）
        for (const auto& word : words) {
            if (!stopwords.count(word) && word.size() > 3) { // 过滤单字
                freq[word]++;
                total++;
            }
        }
    }

    // 转换为排序数组
    vector<pair<string, int>> vec(freq.begin(), freq.end());
    sort(vec.begin(), vec.end(), [](auto& a, auto& b) {
        return a.second > b.second;
    });

    // 输出前10高频词
    cout << "Top 10高频词（概率降序）:\n";
    for (int i = 0; i < 10 && i < vec.size(); ++i) {
        double prob = static_cast<double>(vec[i].second) / total;
        cout << (i+1) << ". " << vec[i].first
             << " (出现次数: " << vec[i].second
             << ", 概率: " << prob * 100 << "%)" << endl;
    }

    return 0;
}