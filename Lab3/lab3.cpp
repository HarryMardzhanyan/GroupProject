// clang++ -std=c++20 -o lab3 lab3.cpp
// ./lab3

// Графики реализованы в Google Colab на основе benchmark_results.csv файла по ссылке: https://colab.research.google.com/drive/1_0BxyO4iaYNKsG38Te2bsTIZSuVMfiHW#scrollTo=VarFJpMesr3I

#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <random>
#include <fstream>
#include <iomanip>
#include <memory>
#include <map>

// Базовый интерфейс
template<typename T>
class RangeQueryStructure {
public:
    virtual ~RangeQueryStructure() = default;
    virtual T query(int l, int r) = 0;
    virtual void update(int idx, T value) { }
    virtual size_t getOperationCount() const { return 0; }
    virtual void resetOperations() { }
    virtual std::string getName() const = 0;
};

// 1D RSQ через префиксные суммы
template<typename T>
class PrefixSumRSQ1D : public RangeQueryStructure<T> {
private:
    std::vector<T> prefix;
    mutable size_t operationCount;
    
public:
    PrefixSumRSQ1D(const std::vector<T>& arr) : operationCount(0) {
        prefix.resize(arr.size() + 1, 0);
        for (size_t i = 0; i < arr.size(); ++i) {
            prefix[i + 1] = prefix[i] + arr[i];
            operationCount++;
        }
    }
    
    T query(int l, int r) override {
        operationCount = 0;
        operationCount++;
        return prefix[r + 1] - prefix[l];
    }
    
    size_t getOperationCount() const override { return operationCount; }
    void resetOperations() override { operationCount = 0; }
    std::string getName() const override { return "PrefixSum1D"; }
};

// 2D RSQ через префиксные суммы
template<typename T>
class PrefixSumRSQ2D {
private:
    std::vector<std::vector<T>> prefix;
    mutable size_t operationCount;
    
public:
    PrefixSumRSQ2D(const std::vector<std::vector<T>>& matrix) : operationCount(0) {
        int n = matrix.size();
        int m = matrix[0].size();
        prefix.assign(n + 1, std::vector<T>(m + 1, 0));
        
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                prefix[i + 1][j + 1] = matrix[i][j] 
                                     + prefix[i][j + 1] 
                                     + prefix[i + 1][j] 
                                     - prefix[i][j];
                operationCount += 3;
            }
        }
    }
    
    T query(int x1, int y1, int x2, int y2) {
        operationCount = 0;
        operationCount += 4;
        return prefix[x2 + 1][y2 + 1] - prefix[x1][y2 + 1] 
             - prefix[x2 + 1][y1] + prefix[x1][y1];
    }
    
    size_t getOperationCount() const { return operationCount; }
    void resetOperations() { operationCount = 0; }
    std::string getName() const { return "PrefixSum2D"; }
};

// 1D RMQ через предпросчет всех отрезков
template<typename T>
class PrecomputeRMQ : public RangeQueryStructure<T> {
private:
    std::vector<std::vector<T>> st;
    mutable size_t operationCount;
    
public:
    PrecomputeRMQ(const std::vector<T>& arr) : operationCount(0) {
        int n = arr.size();
        st.assign(n, std::vector<T>(n));
        
        for (int i = 0; i < n; ++i) {
            st[i][i] = arr[i];
            operationCount++;
            for (int j = i + 1; j < n; ++j) {
                st[i][j] = std::min(st[i][j - 1], arr[j]);
                operationCount++;
            }
        }
    }
    
    T query(int l, int r) override {
        operationCount = 0;
        operationCount++;
        return st[l][r];
    }
    
    size_t getOperationCount() const override { return operationCount; }
    void resetOperations() override { operationCount = 0; }
    std::string getName() const override { return "PrecomputeRMQ"; }
};

// Корневая декомпозиция
template<typename T, typename CombineOp>
class SqrtDecomposition : public RangeQueryStructure<T> {
private:
    std::vector<T> data;
    std::vector<T> blocks;
    int blockSize;
    CombineOp combine;
    mutable size_t operationCount;
    T neutral;
    
public:
    SqrtDecomposition(const std::vector<T>& arr, CombineOp op, T neutralElem) 
        : data(arr), combine(op), operationCount(0), neutral(neutralElem) {
        int n = arr.size();
        blockSize = static_cast<int>(std::sqrt(n)) + 1;
        int numBlocks = (n + blockSize - 1) / blockSize;
        blocks.assign(numBlocks, neutral);
        
        for (int i = 0; i < numBlocks; ++i) {
            int start = i * blockSize;
            int end = std::min(n, start + blockSize);
            for (int j = start; j < end; ++j) {
                if (j == start) blocks[i] = data[j];
                else blocks[i] = combine(blocks[i], data[j]);
                operationCount++;
            }
        }
    }
    
    T query(int l, int r) override {
        operationCount = 0;
        int leftBlock = l / blockSize;
        int rightBlock = r / blockSize;
        
        T result = neutral;
        bool first = true;
        
        if (leftBlock == rightBlock) {
            for (int i = l; i <= r; ++i) {
                if (first) {
                    result = data[i];
                    first = false;
                } else {
                    result = combine(result, data[i]);
                }
                operationCount++;
            }
        } else {
            int leftEnd = (leftBlock + 1) * blockSize;
            for (int i = l; i < leftEnd; ++i) {
                if (first) {
                    result = data[i];
                    first = false;
                } else {
                    result = combine(result, data[i]);
                }
                operationCount++;
            }
            
            for (int b = leftBlock + 1; b < rightBlock; ++b) {
                if (first) {
                    result = blocks[b];
                    first = false;
                } else {
                    result = combine(result, blocks[b]);
                }
                operationCount++;
            }
            
            int rightStart = rightBlock * blockSize;
            for (int i = rightStart; i <= r; ++i) {
                if (first) {
                    result = data[i];
                    first = false;
                } else {
                    result = combine(result, data[i]);
                }
                operationCount++;
            }
        }
        return result;
    }
    
    void update(int idx, T value) override {
        data[idx] = value;
        int blockIdx = idx / blockSize;
        int start = blockIdx * blockSize;
        int end = std::min((int)data.size(), start + blockSize);
        
        blocks[blockIdx] = data[start];
        for (int i = start + 1; i < end; ++i) {
            blocks[blockIdx] = combine(blocks[blockIdx], data[i]);
        }
    }
    
    size_t getOperationCount() const override { return operationCount; }
    void resetOperations() override { operationCount = 0; }
    std::string getName() const override { return "SqrtDecomposition"; }
};

// Дерево отрезков
template<typename T, typename CombineOp>
class SegmentTree : public RangeQueryStructure<T> {
private:
    std::vector<T> tree;
    int n;
    CombineOp combine;
    mutable size_t operationCount;
    T neutral;
    
    void build(const std::vector<T>& arr, int node, int left, int right) {
        if (left == right) {
            tree[node] = arr[left];
            operationCount++;
            return;
        }
        int mid = (left + right) / 2;
        build(arr, node * 2, left, mid);
        build(arr, node * 2 + 1, mid + 1, right);
        tree[node] = combine(tree[node * 2], tree[node * 2 + 1]);
        operationCount++;
    }
    
    T query(int node, int left, int right, int ql, int qr) const {
        if (ql > right || qr < left) return neutral;
        if (ql <= left && right <= qr) {
            operationCount++;
            return tree[node];
        }
        int mid = (left + right) / 2;
        T leftRes = query(node * 2, left, mid, ql, qr);
        T rightRes = query(node * 2 + 1, mid + 1, right, ql, qr);
        operationCount++;
        return combine(leftRes, rightRes);
    }
    
    void update(int node, int left, int right, int idx, T value) {
        if (left == right) {
            tree[node] = value;
            operationCount++;
            return;
        }
        int mid = (left + right) / 2;
        if (idx <= mid) update(node * 2, left, mid, idx, value);
        else update(node * 2 + 1, mid + 1, right, idx, value);
        tree[node] = combine(tree[node * 2], tree[node * 2 + 1]);
        operationCount++;
    }
    
public:
    SegmentTree(const std::vector<T>& arr, CombineOp op, T neutralElem) 
        : combine(op), operationCount(0), neutral(neutralElem) {
        n = arr.size();
        tree.resize(4 * n);
        build(arr, 1, 0, n - 1);
    }
    
    T query(int l, int r) override {
        operationCount = 0;
        return query(1, 0, n - 1, l, r);
    }
    
    void update(int idx, T value) override {
        operationCount = 0;
        update(1, 0, n - 1, idx, value);
    }
    
    size_t getOperationCount() const override { return operationCount; }
    void resetOperations() override { operationCount = 0; }
    std::string getName() const override { return "SegmentTree"; }
};

// Дерево Фенвика
template<typename T>
class FenwickTree : public RangeQueryStructure<T> {
private:
    std::vector<T> bit;
    int n;
    mutable size_t operationCount;
    
    T sum(int idx) const {
        T result = 0;
        for (; idx >= 0; idx = (idx & (idx + 1)) - 1) {
            result += bit[idx];
            operationCount++;
        }
        return result;
    }
    
public:
    FenwickTree(const std::vector<T>& arr) : operationCount(0) {
        n = arr.size();
        bit.assign(n, 0);
        for (int i = 0; i < n; ++i) {
            for (int j = i; j < n; j = j | (j + 1)) {
                bit[j] += arr[i];
                operationCount++;
            }
        }
    }
    
    T query(int l, int r) override {
        operationCount = 0;
        T rightSum = sum(r);
        T leftSum = (l > 0) ? sum(l - 1) : 0;
        operationCount++;
        return rightSum - leftSum;
    }
    
    void update(int idx, T delta) override {
        operationCount = 0;
        for (; idx < n; idx = idx | (idx + 1)) {
            bit[idx] += delta;
            operationCount++;
        }
    }
    
    size_t getOperationCount() const override { return operationCount; }
    void resetOperations() override { operationCount = 0; }
    std::string getName() const override { return "FenwickTree"; }
};

// Разреженная таблица (RMQ)
template<typename T>
class SparseTable : public RangeQueryStructure<T> {
private:
    std::vector<std::vector<T>> st;
    std::vector<int> log;
    mutable size_t operationCount;
    
public:
    SparseTable(const std::vector<T>& arr) : operationCount(0) {
        int n = arr.size();
        log.resize(n + 1);
        log[1] = 0;
        for (int i = 2; i <= n; ++i) {
            log[i] = log[i / 2] + 1;
        }
        
        int K = log[n] + 1;
        st.assign(n, std::vector<T>(K));
        
        for (int i = 0; i < n; ++i) {
            st[i][0] = arr[i];
            operationCount++;
        }
        
        for (int j = 1; j < K; ++j) {
            for (int i = 0; i + (1 << j) <= n; ++i) {
                st[i][j] = std::min(st[i][j - 1], st[i + (1 << (j - 1))][j - 1]);
                operationCount++;
            }
        }
    }
    
    T query(int l, int r) override {
        operationCount = 0;
        int j = log[r - l + 1];
        operationCount++;
        return std::min(st[l][j], st[r - (1 << j) + 1][j]);
    }
    
    size_t getOperationCount() const override { return operationCount; }
    void resetOperations() override { operationCount = 0; }
    std::string getName() const override { return "SparseTable"; }
};

// Комбинация Sqrt + Sparse Table
template<typename T>
class HybridRMQ : public RangeQueryStructure<T> {
private:
    std::vector<T> data;
    std::vector<SparseTable<T>> blockTables;
    std::vector<T> blockMins;
    int blockSize;
    mutable size_t operationCount;
    
public:
    HybridRMQ(const std::vector<T>& arr) : operationCount(0) {
        data = arr;
        int n = arr.size();
        blockSize = static_cast<int>(std::sqrt(n)) + 1;
        int numBlocks = (n + blockSize - 1) / blockSize;
        
        for (int b = 0; b < numBlocks; ++b) {
            int start = b * blockSize;
            int end = std::min(n, start + blockSize);
            std::vector<T> blockData(arr.begin() + start, arr.begin() + end);
            blockTables.emplace_back(blockData);
            
            T blockMin = blockData[0];
            for (size_t i = 1; i < blockData.size(); ++i) {
                blockMin = std::min(blockMin, blockData[i]);
                operationCount++;
            }
            blockMins.push_back(blockMin);
        }
    }
    
    T query(int l, int r) override {
        operationCount = 0;
        int leftBlock = l / blockSize;
        int rightBlock = r / blockSize;
        
        T result = data[l];
        operationCount++;
        
        if (leftBlock == rightBlock) {
            for (int i = l + 1; i <= r; ++i) {
                result = std::min(result, data[i]);
                operationCount++;
            }
        } else {
            int leftBlockStart = leftBlock * blockSize;
            int leftBlockEnd = std::min((int)data.size(), leftBlockStart + blockSize) - 1;
            T leftMin = blockTables[leftBlock].query(l - leftBlockStart, leftBlockEnd - leftBlockStart);
            result = std::min(result, leftMin);
            operationCount++;
            
            for (int b = leftBlock + 1; b < rightBlock; ++b) {
                result = std::min(result, blockMins[b]);
                operationCount++;
            }
            
            int rightBlockStart = rightBlock * blockSize;
            T rightMin = blockTables[rightBlock].query(0, r - rightBlockStart);
            result = std::min(result, rightMin);
            operationCount++;
        }
        return result;
    }
    
    size_t getOperationCount() const override { return operationCount; }
    void resetOperations() override { operationCount = 0; }
    std::string getName() const override { return "HybridRMQ"; }
};

// Измерение производительности
class PerformanceMeasurer {
public:
    struct Measurement {
        std::string name;
        int size;
        double buildOps;
        double queryOps;
    };
    
    template<typename T, typename CreateFunc>
    static Measurement measureQuery(const std::string& name, int size, 
                                     CreateFunc creator, int numQueries = 1000) {
        Measurement m;
        m.name = name;
        m.size = size;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, size - 1);
        
        std::vector<int> arr(size);
        for (int i = 0; i < size; ++i) {
            arr[i] = dis(gen);
        }
        
        auto structure = creator(arr);
        m.buildOps = structure->getOperationCount();
        
        structure->resetOperations();
        long long totalOps = 0;
        
        for (int q = 0; q < numQueries; ++q) {
            int l = dis(gen);
            int r = dis(gen);
            if (l > r) std::swap(l, r);
            structure->query(l, r);
            totalOps += structure->getOperationCount();
            structure->resetOperations();
        }
        
        m.queryOps = static_cast<double>(totalOps) / numQueries;
        return m;
    }
    
    static void saveToCSV(const std::vector<Measurement>& measurements, 
                          const std::string& filename) {
        std::ofstream file(filename);
        file << "Size,Structure,BuildOps,QueryOps\n";
        
        for (const auto& m : measurements) {
            file << m.size << "," << m.name << "," << m.buildOps << "," << m.queryOps << "\n";
        }
        
        file.close();
        std::cout << "Data saved to " << filename << std::endl;
    }
};

// Функция для тестирования
void testStructures() {
    std::cout << "\n=== TESTING STRUCTURES ===\n\n";
    
    std::vector<int> arr = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
    
    // Test PrefixSum1D
    PrefixSumRSQ1D<int> ps1d(arr);
    std::cout << "PrefixSum1D sum[2..5] = " << ps1d.query(2, 5) 
              << " (expected: 21)\n";
    
    // Test PrecomputeRMQ
    PrecomputeRMQ<int> prmq(arr);
    std::cout << "PrecomputeRMQ min[2..5] = " << prmq.query(2, 5) 
              << " (expected: 1)\n";
    
    // Test SqrtDecomposition
    auto minOp = [](int a, int b) { return std::min(a, b); };
    SqrtDecomposition<int, decltype(minOp)> sqrtDec(arr, minOp, INT_MAX);
    std::cout << "SqrtDecomposition min[2..5] = " << sqrtDec.query(2, 5) 
              << " (expected: 1)\n";
    
    // Test SegmentTree
    SegmentTree<int, decltype(minOp)> segTree(arr, minOp, INT_MAX);
    std::cout << "SegmentTree min[2..5] = " << segTree.query(2, 5) 
              << " (expected: 1)\n";
    segTree.update(4, 100);
    std::cout << "After update(4,100) min[2..5] = " << segTree.query(2, 5) 
              << " (expected: 1)\n";
    
    // Test FenwickTree
    auto sumOp = [](int a, int b) { return a + b; };
    FenwickTree<int> fenwick(arr);
    std::cout << "FenwickTree sum[2..5] = " << fenwick.query(2, 5) 
              << " (expected: 21)\n";
    fenwick.update(4, 100);
    std::cout << "After update(4,100) sum[2..5] = " << fenwick.query(2, 5) 
              << " (expected: 121)\n";
    
    // Test SparseTable
    SparseTable<int> sparse(arr);
    std::cout << "SparseTable min[2..5] = " << sparse.query(2, 5) 
              << " (expected: 1)\n";
    
    // Test HybridRMQ
    HybridRMQ<int> hybrid(arr);
    std::cout << "HybridRMQ min[2..5] = " << hybrid.query(2, 5) 
              << " (expected: 1)\n";
}

// Замер производительности
void runBenchmarks() {
    std::cout << "\n=== RUNNING BENCHMARKS ===\n";
    
    std::vector<int> sizes = {10, 50, 100, 200, 500, 1000, 2000, 5000, 10000};
    std::vector<PerformanceMeasurer::Measurement> measurements;
    
    auto minOp = [](int a, int b) { return std::min(a, b); };
    
    for (int size : sizes) {
        std::cout << "Testing size: " << size << std::endl;
        
        // PrefixSum1D
        auto m1 = PerformanceMeasurer::measureQuery<int>(
            "PrefixSum1D", size,
            [](const std::vector<int>& arr) -> std::unique_ptr<RangeQueryStructure<int>> {
                return std::make_unique<PrefixSumRSQ1D<int>>(arr);
            }
        );
        measurements.push_back(m1);
        
        // PrecomputeRMQ (only for small sizes)
        if (size <= 1000) {
            auto m2 = PerformanceMeasurer::measureQuery<int>(
                "PrecomputeRMQ", size,
                [](const std::vector<int>& arr) -> std::unique_ptr<RangeQueryStructure<int>> {
                    return std::make_unique<PrecomputeRMQ<int>>(arr);
                }
            );
            measurements.push_back(m2);
        }
        
        // SqrtDecomposition
        auto m3 = PerformanceMeasurer::measureQuery<int>(
            "SqrtDecomposition", size,
            [&](const std::vector<int>& arr) -> std::unique_ptr<RangeQueryStructure<int>> {
                return std::make_unique<SqrtDecomposition<int, decltype(minOp)>>(arr, minOp, INT_MAX);
            }
        );
        measurements.push_back(m3);
        
        // SegmentTree
        auto m4 = PerformanceMeasurer::measureQuery<int>(
            "SegmentTree", size,
            [&](const std::vector<int>& arr) -> std::unique_ptr<RangeQueryStructure<int>> {
                return std::make_unique<SegmentTree<int, decltype(minOp)>>(arr, minOp, INT_MAX);
            }
        );
        measurements.push_back(m4);
        
        // FenwickTree
        auto m5 = PerformanceMeasurer::measureQuery<int>(
            "FenwickTree", size,
            [](const std::vector<int>& arr) -> std::unique_ptr<RangeQueryStructure<int>> {
                return std::make_unique<FenwickTree<int>>(arr);
            }
        );
        measurements.push_back(m5);
        
        // SparseTable
        auto m6 = PerformanceMeasurer::measureQuery<int>(
            "SparseTable", size,
            [](const std::vector<int>& arr) -> std::unique_ptr<RangeQueryStructure<int>> {
                return std::make_unique<SparseTable<int>>(arr);
            }
        );
        measurements.push_back(m6);
        
        // HybridRMQ
        auto m7 = PerformanceMeasurer::measureQuery<int>(
            "HybridRMQ", size,
            [](const std::vector<int>& arr) -> std::unique_ptr<RangeQueryStructure<int>> {
                return std::make_unique<HybridRMQ<int>>(arr);
            }
        );
        measurements.push_back(m7);
    }
    
    PerformanceMeasurer::saveToCSV(measurements, "benchmark_results.csv");
    std::cout << "\nBenchmark complete! Results saved to benchmark_results.csv\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "Laboratory Work #3: Data Structures\n";
    std::cout << "RMQ/RSQ Implementation\n";
    std::cout << "========================================\n";
    
    testStructures();
    runBenchmarks();
    
    std::cout << "\nTo generate plots, run: python3 plot_results.py\n";
    
    return 0;
}