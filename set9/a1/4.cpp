#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <random>
#include <utility>
#include <iomanip>
#include <fstream>

class StringGenerator {
private:
    std::mt19937 rng;
    const std::string ALPHABET = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "!@#%:;^&*()-.";

    std::string generateRandomString(size_t minLen = 10, size_t maxLen = 200) {
        std::uniform_int_distribution<size_t> lenDist(minLen, maxLen);
        std::uniform_int_distribution<size_t> charDist(0, ALPHABET.size() - 1);
        
        size_t length = lenDist(rng);
        std::string result;
        result.reserve(length);
        
        for (size_t i = 0; i < length; ++i) {
            result.push_back(ALPHABET[charDist(rng)]);
        }
        return result;
    }

public:
    StringGenerator(unsigned int seed = 42) : rng(seed) {}

    std::vector<std::string> generateRandomArray(size_t size = 3000) {
        std::vector<std::string> arr;
        arr.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            arr.push_back(generateRandomString());
        } 
        return arr;
    }

    std::vector<std::string> generateReverseSortedArray(size_t size = 3000) {
        std::vector<std::string> arr = generateRandomArray(size);
        std::sort(arr.begin(), arr.end(), std::greater<std::string>());
        return arr;
    }

    std::vector<std::string> generateNearlySortedArray(size_t size = 3000, double swapRatio = 0.05) {
        std::vector<std::string> arr = generateRandomArray(size);
        std::sort(arr.begin(), arr.end());
        size_t numSwaps = static_cast<size_t>(size * swapRatio);
        std::uniform_int_distribution<size_t> indexDist(0, size - 1);
        for (size_t i = 0; i < numSwaps; ++i) {
            std::swap(arr[indexDist(rng)], arr[indexDist(rng)]);
        }
        return arr;
    }

    std::vector<std::string> generatePrefixArray(size_t size = 3000, const std::string& prefix = "COMMON_PREFIX_") {
        std::vector<std::string> arr;
        arr.reserve(size);
        size_t minRemain = (prefix.length() >= 10) ? 0 : (10 - prefix.length());
        size_t maxRemain = (prefix.length() >= 200) ? 0 : (200 - prefix.length());

        for (size_t i = 0; i < size; ++i) {
            if (maxRemain > 0) {
                arr.push_back(prefix + generateRandomString(minRemain, maxRemain));
            } 
            else {
                arr.push_back(prefix.substr(0, 200));
            }
        }
        return arr;
    }
};

class StringSortTester {
private:
    size_t charComparisons;

    int charAt(const std::string& s, size_t d) {
        if (d < s.length())  return static_cast<unsigned char>(s[d]);
        return -1;
    }

    int compareStrings(const std::string& a, const std::string& b) {
        size_t minLen = std::min(a.length(), b.length());
        for (size_t i = 0; i < minLen; ++i) {
            charComparisons++;
            if (a[i] != b[i]) return a[i] < b[i] ? -1 : 1;
        }
        charComparisons++;
        if (a.length() == b.length()) return 0;
        return a.length() < b.length() ? -1 : 1;
    }

    int getLCP(const std::string& a, const std::string& b) {
        size_t minLen = std::min(a.length(), b.length());
        size_t lcp = 0;
        for (size_t i = 0; i < minLen; ++i) {
            charComparisons++;
            if (a[i] == b[i]) lcp++;
            else break;
        }
        charComparisons++;
        return lcp;
    }

    void merge(std::vector<std::string>& arr, int left, int mid, int right) {
        int n1 = mid - left + 1; int n2 = right - mid;
        std::vector<std::string> L(n1), R(n2);
        for (int i = 0; i < n1; ++i) {
            L[i] = arr[left + i];
        } 
        for (int j = 0; j < n2; ++j) {
            R[j] = arr[mid + 1 + j];
        } 
        
        int i = 0;
        int j = 0;
        int k = left;
        while (i < n1 && j < n2) {
            if (compareStrings(L[i], R[j]) <= 0) arr[k++] = L[i++];
            else arr[k++] = R[j++];
        }
        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];
    }

    void mergeSort(std::vector<std::string>& arr, int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }

    int partition(std::vector<std::string>& arr, int low, int high) {
        std::string pivot = arr[high];
        int i = low - 1;
        for (int j = low; j <= high - 1; j++) {
            if (compareStrings(arr[j], pivot) <= 0) std::swap(arr[++i], arr[j]);
        }
        std::swap(arr[i + 1], arr[high]);
        return i + 1;
    }

    void quickSort(std::vector<std::string>& arr, int low, int high) {
        if (low < high) {
            int pi = partition(arr, low, high);
            quickSort(arr, low, pi - 1);
            quickSort(arr, pi + 1, high);
        }
    }

    void ternaryStringQuickSort(std::vector<std::string>& arr, int lo, int hi, int d) {
        if (hi <= lo) return;
        int lt = lo, gt = hi;
        int v = charAt(arr[lo], d);
        int i = lo + 1;
        while (i <= gt) {
            int t = charAt(arr[i], d);
            charComparisons++;
            if (t < v) std::swap(arr[lt++], arr[i++]);
            else {
                charComparisons++;
                if (t > v) std::swap(arr[i], arr[gt--]);
                else i++;
            }
        }
        ternaryStringQuickSort(arr, lo, lt - 1, d);
        if (v >= 0) ternaryStringQuickSort(arr, lt, gt, d + 1);
        ternaryStringQuickSort(arr, gt + 1, hi, d);
    }

    void stringMerge(std::vector<std::string>& arr, int left, int mid, int right) {
        int n1 = mid - left + 1; 
        int n2 = right - mid;
        std::vector<std::string> L(n1), R(n2);
        for (int i = 0; i < n1; ++i) {
            L[i] = arr[left + i];
        } 
        for (int j = 0; j < n2; ++j) {
            R[j] = arr[mid + 1 + j];
        } 
        
        int i = 0;
        int j = 0;
        int k = left;
        while (i < n1 && j < n2) {
            int lcp = getLCP(L[i], R[j]);
            bool leftIsSmaller = false;
            if (lcp == L[i].length() && lcp == R[j].length()) leftIsSmaller = true;
            else if (lcp == L[i].length()) leftIsSmaller = true;
            else if (lcp == R[j].length()) leftIsSmaller = false;
            else {
                charComparisons++;
                leftIsSmaller = L[i][lcp] < R[j][lcp];
            }

            if (leftIsSmaller) arr[k++] = L[i++];
            else arr[k++] = R[j++];
        }
        while (i < n1) {
            arr[k++] = L[i++];
        } 
        while (j < n2) {
            arr[k++] = R[j++];
        }
    }

    void stringMergeSort(std::vector<std::string>& arr, int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        stringMergeSort(arr, left, mid);
        stringMergeSort(arr, mid + 1, right);
        stringMerge(arr, left, mid, right);
    }

    void msdRadixSort(std::vector<std::string>& arr, int lo, int hi, int d, std::vector<std::string>& aux) {
        if (hi <= lo) return;
        int count[258] = {0};
        for (int i = lo; i <= hi; i++) count[charAt(arr[i], d) + 2]++;
        for (int r = 0; r < 257; r++) count[r + 1] += count[r];
        for (int i = lo; i <= hi; i++) aux[count[charAt(arr[i], d) + 1]++] = std::move(arr[i]);
        for (int i = lo; i <= hi; i++) arr[i] = std::move(aux[i - lo]);
        for (int r = 0; r < 256; r++) msdRadixSort(arr, lo + count[r], lo + count[r + 1] - 1, d + 1, aux);
    }

    void msdRadixSortWithFallback(std::vector<std::string>& arr, int lo, int hi, int d, std::vector<std::string>& aux) {
        if (hi <= lo) return;
        if (hi - lo + 1 < 74) {
            ternaryStringQuickSort(arr, lo, hi, d);
            return;
        }
        int count[258] = {0};
        for (int i = lo; i <= hi; i++) count[charAt(arr[i], d) + 2]++;
        for (int r = 0; r < 257; r++) count[r + 1] += count[r];
        for (int i = lo; i <= hi; i++) aux[count[charAt(arr[i], d) + 1]++] = std::move(arr[i]);
        for (int i = lo; i <= hi; i++) arr[i] = std::move(aux[i - lo]);
        for (int r = 0; r < 256; r++) msdRadixSortWithFallback(arr, lo + count[r], lo + count[r + 1] - 1, d + 1, aux);
    }

public:
    StringSortTester() : charComparisons(0) {}

    std::pair<double, size_t> runMergeSort(std::vector<std::string> arr) {
        charComparisons = 0;
        auto start = std::chrono::high_resolution_clock::now();
        if (!arr.empty()) mergeSort(arr, 0, arr.size() - 1);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> diff = end - start;
        return {diff.count(), charComparisons};
    }

    std::pair<double, size_t> runQuickSort(std::vector<std::string> arr) {
        charComparisons = 0;
        auto start = std::chrono::high_resolution_clock::now();
        if (!arr.empty()) quickSort(arr, 0, arr.size() - 1);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> diff = end - start;
        return {diff.count(), charComparisons};
    }

    std::pair<double, size_t> runTernaryStringQuickSort(std::vector<std::string> arr) {
        charComparisons = 0;
        auto start = std::chrono::high_resolution_clock::now();
        if (!arr.empty()) ternaryStringQuickSort(arr, 0, arr.size() - 1, 0);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> diff = end - start;
        return {diff.count(), charComparisons};
    }

    std::pair<double, size_t> runStringMergeSort(std::vector<std::string> arr) {
        charComparisons = 0;
        auto start = std::chrono::high_resolution_clock::now();
        if (!arr.empty()) stringMergeSort(arr, 0, arr.size() - 1);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> diff = end - start;
        return {diff.count(), charComparisons};
    }

    std::pair<double, size_t> runMsdRadixSort(std::vector<std::string> arr) {
        charComparisons = 0;
        auto start = std::chrono::high_resolution_clock::now();
        if (!arr.empty()) {
            std::vector<std::string> aux(arr.size());
            msdRadixSort(arr, 0, arr.size() - 1, 0, aux);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> diff = end - start;
        return {diff.count(), charComparisons};
    }

    std::pair<double, size_t> runMsdRadixSortWithFallback(std::vector<std::string> arr) {
        charComparisons = 0;
        auto start = std::chrono::high_resolution_clock::now();
        if (!arr.empty()) {
            std::vector<std::string> aux(arr.size());
            msdRadixSortWithFallback(arr, 0, arr.size() - 1, 0, aux);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> diff = end - start;
        return {diff.count(), charComparisons};
    }
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    StringGenerator generator;
    StringSortTester tester;

    std::cerr << "Generating master arrays (this might take a moment)...\n";
    auto randomArr = generator.generateRandomArray(3000);
    auto reverseArr = generator.generateReverseSortedArray(3000);
    auto nearlyArr = generator.generateNearlySortedArray(3000);
    auto prefixArr = generator.generatePrefixArray(3000);

    const int NUM_RUNS = 5; 

    std::ofstream outFile("results.csv");
    if (!outFile.is_open()) {
        std::cerr << "Failed to create results.csv file!\n";
        return 1;
    }

    outFile << "Size,ArrayType,Algorithm,TimeMs,Comparisons\n";

    std::vector<std::pair<std::string, std::vector<std::string>*>> datasets = {
        {"Random", &randomArr},
        {"Reverse_Sorted", &reverseArr},
        {"Nearly_Sorted", &nearlyArr},
        {"Common_Prefix", &prefixArr}
    };

    std::cerr << "Starting benchmarks...\n";
    for (size_t size = 100; size <= 3000; size += 100) {
        std::cerr << "Processing size: " << size << " / 3000\n";
        
        for (const auto& dataset : datasets) {
            std::string arrayType = dataset.first;
            std::vector<std::string> currentSlice(dataset.second->begin(), dataset.second->begin() + size);

            struct Acc { double time = 0; size_t comps = 0; };
            Acc accMS, accQS, accTSQS, accSMS, accMSD, accMSDF;

            for (int i = 0; i < NUM_RUNS; ++i) {
                auto res = tester.runMergeSort(currentSlice);
                accMS.time += res.first; accMS.comps += res.second;

                res = tester.runQuickSort(currentSlice);
                accQS.time += res.first; accQS.comps += res.second;

                res = tester.runTernaryStringQuickSort(currentSlice);
                accTSQS.time += res.first; accTSQS.comps += res.second;

                res = tester.runStringMergeSort(currentSlice);
                accSMS.time += res.first; accSMS.comps += res.second;

                res = tester.runMsdRadixSort(currentSlice);
                accMSD.time += res.first; accMSD.comps += res.second;

                res = tester.runMsdRadixSortWithFallback(currentSlice);
                accMSDF.time += res.first; accMSDF.comps += res.second;
            }

            outFile << std::fixed << std::setprecision(4);
            outFile << size << "," << arrayType << ",MergeSort," << (accMS.time / NUM_RUNS) << "," << (accMS.comps / NUM_RUNS) << "\n";
            outFile << size << "," << arrayType << ",QuickSort," << (accQS.time / NUM_RUNS) << "," << (accQS.comps / NUM_RUNS) << "\n";
            outFile << size << "," << arrayType << ",TernaryQuickSort," << (accTSQS.time / NUM_RUNS) << "," << (accTSQS.comps / NUM_RUNS) << "\n";
            outFile << size << "," << arrayType << ",StringMergeSort," << (accSMS.time / NUM_RUNS) << "," << (accSMS.comps / NUM_RUNS) << "\n";
            outFile << size << "," << arrayType << ",MSDRadix," << (accMSD.time / NUM_RUNS) << "," << (accMSD.comps / NUM_RUNS) << "\n";
            outFile << size << "," << arrayType << ",MSDRadix_Fallback," << (accMSDF.time / NUM_RUNS) << "," << (accMSDF.comps / NUM_RUNS) << "\n";
        }
    }

    outFile.close(); 
    std::cerr << "Done! The file 'results.csv' has been created.\n";
    return 0;
}