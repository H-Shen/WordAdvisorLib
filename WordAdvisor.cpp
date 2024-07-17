#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <cassert>

// Calculate the Levenshtein distance between two strings
static int levenshtein_distance(const std::string &s1, const std::string &s2) {
    int m = static_cast<int>(s1.size());
    int n = static_cast<int>(s2.size());
    // Initialize the matrix
    std::vector<std::vector<int>> d(m + 1, std::vector<int>(n + 1));
    int cost;
    // Compute the distance with dynamic programming
    for (int i = 0; i <= m; ++i) d[i][0] = i;
    for (int j = 0; j <= n; ++j) d[0][j] = j;
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            cost = s1[i - 1] == s2[j - 1] ? 0 : 1;
            d[i][j] = std::min({d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + cost});
        }
    }
    return d[m][n];
}

// Divide an interval [lower_bound, upper_bound] into n approximately equal subintervals
static std::vector<std::pair<int, int>>
allocate_jobs(int lower_bound, int upper_bound, int n) {
    std::vector<std::pair<int, int>> jobs;
    int length = upper_bound - lower_bound + 1;
    // Adjust chunk size if n > length
    int chunk = (n > length) ? length : n;
    auto temp = div(length, chunk);
    int rem = temp.rem;
    int quot = temp.quot;
    // Allocate ranges to each job
    jobs.resize(chunk);
    for (int i = 0; i < chunk; ++i) {
        jobs.at(i).first = std::min(rem, i) + i * quot + lower_bound;
        jobs.at(i).second = std::min(rem, i + 1) + (i + 1) * quot + lower_bound - 1;
    }
    return jobs;
}

// A word advisor by using comparing the Levenshtein distance between the word given with m_dictionary in the dictionary,
// the word comes with the smallest Levenshtein distance and appear first will be the output
class WordAdvisor {
private:
    // The dictionary of words
    std::vector<std::string> m_dictionary;
    // A hashmap stored all previous valid queries, we use it to save time cost by repeated queries
    std::unordered_map<std::string, std::string> m_memo;
public:
    explicit WordAdvisor(const std::vector<std::string> &words) : m_dictionary(words) {
        if (m_dictionary.empty()) {
            throw std::runtime_error("The dictionary cannot be empty!");
        }
    }

    // Get the best matches to each word in the query list and return the result as a hashmap using multi-threads
    std::unordered_map<std::string, std::string>
    get_best_match_of_query_list_by_multi_threads(const std::vector<std::string> &query_list) {
        if (query_list.empty()) {
            throw std::runtime_error("The query list cannot be empty!");
        }
        std::unordered_set<std::string> unique_words(query_list.begin(), query_list.end());
        std::unordered_map<std::string, std::string> result;
        std::vector<std::string> queries;
        for (const auto &unique_word: unique_words) {
            auto iter = m_memo.find(unique_word);
            if (iter != m_memo.end()) {
                result[unique_word] = iter->second;
            } else {
                queries.emplace_back(unique_word);
            }
        }
        if (!queries.empty()) {
            // Get the number of physical threads
            int num_threads = static_cast<int>(std::thread::hardware_concurrency());
            auto jobs = allocate_jobs(0, static_cast<int>(queries.size()) - 1, num_threads);
            std::vector<std::string> answers(queries.size());
            // Define the work for each thread
            auto worker = [&](int start, int end) {
                std::string word, answer;
                for (int index = start; index <= end; ++index) {
                    word = queries.at(index);
                    answers.at(index) = *std::min_element(m_dictionary.begin(), m_dictionary.end(),
                                                          [&](const std::string &a, const std::string &b) {
                                                              return levenshtein_distance(a, word) <
                                                                     levenshtein_distance(b, word);
                                                          });
                }
            };
            // Create threads for jobs
            std::vector<std::thread> threads;
            threads.reserve(jobs.size());
            for (const auto &job: jobs) {
                threads.emplace_back(worker, job.first, job.second);
            }
            for (auto &thread: threads) {
                thread.join();
            }
            for (size_t i = 0; i != queries.size(); ++i) {
                result[queries.at(i)] = answers.at(i);
            }
        }
        return result;
    }

    // Get the best match to 'word' from the dictionary using multi-threads
    std::string get_best_match_by_multi_threads(const std::string &word) {
        auto iter = m_memo.find(word);
        if (iter != m_memo.end()) {
            return iter->second;
        }
        // Get the number of physical threads
        int num_threads = static_cast<int>(std::thread::hardware_concurrency());
        std::vector<std::pair<int, int>> jobs = allocate_jobs(0, static_cast<int>(m_dictionary.size()) - 1,
                                                              num_threads);
        std::vector<std::string> answers_of_multi_threads(jobs.size());
        // Define the work for each thread
        auto worker = [&](int thread_id, int start, int end) {
            answers_of_multi_threads.at(thread_id) = *std::min_element(m_dictionary.begin() + start,
                                                                       m_dictionary.begin() + end + 1,
                                                                       [&](const std::string &a, const std::string &b) {
                                                                           return levenshtein_distance(a, word) <
                                                                                  levenshtein_distance(b, word);
                                                                       });
        };
        // Create threads for jobs
        std::vector<std::thread> threads;
        threads.reserve(jobs.size());
        for (int i = 0; i < static_cast<int>(jobs.size()); ++i) {
            threads.emplace_back(worker, i, jobs.at(i).first, jobs.at(i).second);
        }
        for (auto &thread: threads) {
            thread.join();
        }
        // Run the last round
        std::string answer = *std::min_element(answers_of_multi_threads.begin(), answers_of_multi_threads.end(),
                                               [&](const std::string &a, const std::string &b) {
                                                   return levenshtein_distance(a, word) < levenshtein_distance(b, word);
                                               });
        m_memo[word] = answer;
        return answer;
    }
};

int main() {
    // Tests
    std::vector<std::string> dict = {"apple", "apply", "april", "pineapple", "grape", "banana", "orange", "strawberry",
                                     "blueberry", "blackberry", "raspberry", "cranberry"};
    WordAdvisor word_advisor(dict);
    std::string result = word_advisor.get_best_match_by_multi_threads("appel");
    assert(word_advisor.get_best_match_by_multi_threads("appel") == "apple");
    assert(word_advisor.get_best_match_by_multi_threads("bpple") == "apple");
    assert(word_advisor.get_best_match_by_multi_threads("grap") == "grape");
    assert(word_advisor.get_best_match_by_multi_threads("banan") == "banana");
    assert(word_advisor.get_best_match_by_multi_threads("oran") == "orange");
    assert(word_advisor.get_best_match_by_multi_threads("strawbery") == "strawberry");
    std::vector<std::string> query_list = {"grapee", "grapp", "ananb"};
    auto result2 = word_advisor.get_best_match_of_query_list_by_multi_threads(query_list);
    for (const auto &[k, v]: result2) {
        std::cout << k << ' ' << v << '\n';
    }
    return 0;
}