import WordAdvisor
import os

import nltk


def get_nltk_dictionary():
    dir_path = os.path.dirname(os.path.realpath(__file__))
    nltk.download('words', quiet=True, download_dir=dir_path)
    nltk.data.path.append(dir_path)
    return sorted(list(set(nltk.corpus.words.words())))


if __name__ == '__main__':
    d = get_nltk_dictionary()
    # Example usage
    advisor = WordAdvisor.WordAdvisor(d)
    result = advisor.get_best_match_by_multi_threads("appel")
    print(f"Best match for 'appel': {result}")
    query_list = ["grapee", "grapp", "ananb"]
    result2 = advisor.get_best_match_of_query_list_by_multi_threads(query_list)
    for word, match in result2.items():
        print(f"Best match for '{word}': {match}")
