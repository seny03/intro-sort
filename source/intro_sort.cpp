#include <bits/stdc++.h>
#include <chrono>
#include <random>
#include <thread>
using namespace std;

void insertionSort(vector<int> &arr, int left, int right) {
  for (int i = left + 1; i <= right; i++) {
    int val = arr[i];

    int j = i - 1;
    while (j >= left && arr[j] > val) {
      arr[j + 1] = arr[j];
      --j;
    }
    arr[j + 1] = val;
  }
  return;
}

void heapify(vector<int> &arr, int n, int i, int left) {
  int l = 2 * i + 1;
  int r = 2 * i + 2;
  int largest = i;

  if (l < n && arr[left + l] > arr[left + largest]) {
    largest = l;
  }
  if (r < n && arr[left + r] > arr[left + largest]) {
    largest = r;
  }

  if (largest != i) {
    swap(arr[left + largest], arr[left + i]);
    heapify(arr, n, largest, left);
  }

  return;
}

void heapSort(vector<int> &arr, int left, int right) {
  int n = right - left + 1;
  for (int i = n / 2 - 1; i >= 0; i--) {
    heapify(arr, n, i, left);
  }

  for (int i = n - 1; i > 0; --i) {
    swap(arr[left], arr[left + i]);
    heapify(arr, i, 0, left);
  }
  return;
}

int partition(vector<int> &arr, int left, int right) {
  int pivot = arr[right];
  int i = left;
  for (int j = left; j < right; j++) {
    if (arr[j] <= pivot) {
      swap(arr[i], arr[j]);
      i++;
    }
  }
  swap(arr[right], arr[i]);
  return i;
}

void quickSortRealization(vector<int> &arr, int left, int right) {
  if (left >= right) {
    return;
  }

  int pivot = partition(arr, left, right);
  quickSortRealization(arr, left, pivot - 1);
  quickSortRealization(arr, pivot + 1, right);
  return;
}

void quickSort(vector<int> &arr) {
  quickSortRealization(arr, 0, arr.size() - 1);
}

void introSortRealization(vector<int> &arr, int left, int right, int depth) {
  if (left >= right) {
    return;
  }

  if (right - left + 1 < 16) {
    insertionSort(arr, left, right);
    return;
  }

  if (depth == 0) {
    heapSort(arr, left, right);
    return;
  }

  int pivot = partition(arr, left, right);
  introSortRealization(arr, left, pivot - 1, depth - 1);
  introSortRealization(arr, pivot + 1, right, depth - 1);
}

void introSort(vector<int> &arr) {
  introSortRealization(arr, 0, arr.size() - 1, 2 * log2(arr.size()));
}

class ArrayGenerator {
private:
  const int UNSORTED_PERCENTS = 5;
  int size_;

  std::mt19937 gen_;
  std::uniform_int_distribution<> dis_;

  int randNum() { return dis_(gen_); }
  void randSwap(vector<int> &arr) {
    swap(arr[rand() % arr.size()], arr[rand() % arr.size()]);
  }

public:
  vector<int> generated;

  ArrayGenerator(int size, int minValue, int maxValue)
      : size_(size), dis_(minValue, maxValue) {}
  ArrayGenerator() : ArrayGenerator(0, 0, 0) {}

  void genRandom() {
    generated.resize(size_);
    for (size_t i = 0; i < size_; ++i) {
      generated[i] = randNum();
    }
  }

  void genReverseSorted() {
    genRandom();
    sort(generated.rbegin(), generated.rend());
  }

  void genAlmostSorted() {
    genRandom();
    sort(generated.begin(), generated.end());
    int swaps = size_ * UNSORTED_PERCENTS / 100;
    while (swaps--) {
      randSwap(generated);
    }
  }

  void gen(string dataType) {
    if (dataType == "random") {
      genRandom();
    } else if (dataType == "reverse_sorted") {
      genReverseSorted();
    } else if (dataType == "almost_sorted") {
      genReverseSorted();
    } else {
      throw invalid_argument{"incorrect dataType"};
    }
    return;
  }

  vector<int> selectK(size_t k) {
    std::vector<int> indices(generated.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), gen_);
    sort(indices.begin(), indices.begin() + k);

    vector<int> selected(k);
    for (size_t i = 0; i < k; i++) {
      selected[i] = generated[indices[i]];
    }
    return selected;
  }
};

class SortTester {
private:
  vector<int> testData_;
  std::function<void(std::vector<int> &)> sortFunction_;

public:
  SortTester(vector<int> testData,
             const std::function<void(std::vector<int> &)> &sortFunction)
      : testData_(testData), sortFunction_(sortFunction) {}

  long long test() {
    auto start = std::chrono::high_resolution_clock::now();

    sortFunction_(testData_);

    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    long long msec =
        std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    return msec;
  }
};

void Sleep(size_t milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
  return;
}

void query(string dataType, int repeats = 100) {
  auto generator = ArrayGenerator(10000, 0, 6000);

  vector<long double> arrSizes, quickScores, introScores;
  for (int repeat = 0; repeat < repeats; repeat++) {
    generator.gen(dataType);
    for (size_t size = 500, i = 0; size <= 10000; size += 100, i++) {
      auto testData = generator.selectK(size);
      auto quickTester = SortTester(testData, quickSort);
      auto introTester = SortTester(testData, introSort);

      long long quickScore = quickTester.test();
      long long introScore = introTester.test();

      if (i < arrSizes.size()) {
        quickScores[i] += quickScore;
        introScores[i] += introScore;
      } else {
        arrSizes.push_back(size);
        quickScores.push_back(quickScore);
        introScores.push_back(introScore);
      }
    }
  }

  // get average in milliseconds
  for (auto &v : quickScores) {
    v /= (repeats);
  }
  for (auto &v : introScores) {
    v /= (repeats);
  }

  string filename = "quick_" + dataType + ".txt";
  cout << filename << " created!\n";
  std::ofstream out(filename);
  for (auto v : arrSizes) {
    out << v << ' ';
  }
  out << '\n';
  for (auto v : quickScores) {
    out << v << ' ';
  }
  out << '\n';

  filename = "intro_" + dataType + ".txt";
  cout << filename << " created!\n";
  out = std::ofstream(filename);
  for (auto v : arrSizes) {
    out << v << ' ';
  }
  out << '\n';
  for (auto v : introScores) {
    out << v << ' ';
  }
  out << '\n';
  return;
}

void solve() {
  vector<string> dataTypes = {"random", "reverse_sorted", "almost_sorted"};
  for (string dataType : dataTypes) {
    query(dataType);
  }
  return;
}

int main() {
  cin.tie(nullptr);
  cout.tie(nullptr);
  ios_base::sync_with_stdio(false);

  solve();

  return 0;
}
