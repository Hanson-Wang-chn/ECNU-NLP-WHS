import re
from collections import defaultdict, Counter

# 读取词典
def load_dict(dict_path):
    with open(dict_path, 'r', encoding='utf-8') as f:
        words = set(line.strip() for line in f)
    return words

# 读取停用词
def load_stopwords(stopwords_path):
    with open(stopwords_path, 'r', encoding='utf-8') as f:
        stopwords = set(line.strip() for line in f)
    return stopwords

# 最大正向匹配分词算法
def max_forward_match(text, word_dict, max_len=7):
    words = []
    index = 0
    text_length = len(text)
    while index < text_length:
        matched = False
        for i in range(max_len, 0, -1):
            if index + i > text_length:
                continue
            word = text[index:index+i]
            if word in word_dict:
                words.append(word)
                index += i
                matched = True
                break
        if not matched:
            words.append(text[index])
            index += 1
    return words

# 去除标点符号和停用词
def clean_words(words, stopwords):
    cleaned_words = []
    for word in words:
        if word not in stopwords and not re.match(r'^\W+$', word):
            cleaned_words.append(word)
    return cleaned_words

# 主函数
def main():
    # 文件路径
    sentence_path = 'corpus.sentence.txt'
    dict_path = 'corpus.dict.txt'
    stopwords_path = 'cn_stopwords.txt'

    # 加载词典和停用词
    word_dict = load_dict(dict_path)
    stopwords = load_stopwords(stopwords_path)

    # 读取文本
    with open(sentence_path, 'r', encoding='utf-8') as f:
        text = f.read()

    # 分词
    words = max_forward_match(text, word_dict)

    # 去除停用词和标点符号
    cleaned_words = clean_words(words, stopwords)

    # 统计词频
    word_counts = Counter(cleaned_words)

    # 输出前10个高频词
    top_10_words = word_counts.most_common(10)
    print("前10个高频词：")
    for word, count in top_10_words:
        print(f"{word}: {count}")

if __name__ == "__main__":
    main()