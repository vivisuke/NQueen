#include <iostream>
#include <vector>
#include <chrono>
#include <future>

//#define      NQ        8              // クィーン個数、縦・横サイズ

using namespace std;

int    g_count;       // 解個数
//int g_nNode;       // 末端ノード数

void test_BF();           // ブルートフォース
void test_BF_MT();     // ブルートフォース＋マルチスレッド

int main()
{
    test_BF();
    test_BF_MT();
    //
    std::cout << "OK\n";
}

bool check(const char row[], int NQ)   //  配置済みクイーンが効き無しに置かれているか？
{
    for(int r = 1; r < NQ; ++r) {
        for(int i = 0; i < r; ++i) {
            if( row[i] == row[r] || abs(row[i] - row[r]) == r - i )
                return false;     // 水平 or 斜めに効きがある
        }
    }
    return true;
}
#if   0
void generateAndCheck(char row[], int n)      //  n: 配置済みクイーン数
{
    for(row[n] = 1; row[n] <= NQ; ++row[n]) {
        if( n == NQ - 1 ) {
            ++g_nNode;
            if( check(row) )
                ++g_count;    // 解を発見
        } else
            generateAndCheck(row, n+1);
    }
}
#endif
void generateAndCheck(char row[], int n, int NQ)      //  n: 配置済みクイーン数
{
    for (row[n] = 1; row[n] <= NQ; ++row[n]) {
        if (n == NQ - 1) {
            //++g_nNode;
            if (check(row, NQ))
                ++g_count;    // 解を発見
        }
        else
            generateAndCheck(row, n + 1, NQ);
    }
}
template<int NQ>
void generateAndCheck(char row[], int n)      //  n: 配置済みクイーン数
{
    for(row[n] = 1; row[n] <= NQ; ++row[n]) {
        if( n == NQ - 1 ) {
            if( check(row, NQ) )
                ++g_count;    // 解を発見
        } else
            generateAndCheck<NQ>(row, n+1);
    }
}
void test_BF()
{
    cout << "test_BF()\n";
    const int MAX_NQ = 9;
    char row[MAX_NQ];       // 各行のウィーン位置、[1, NQ]
    for (int nq = 4; nq <= MAX_NQ; ++nq) {
        g_count = 0;
        //g_nNode = 0;
        auto start = std::chrono::system_clock::now();      // 計測スタート時刻を保存
        //generateAndCheck<nq>(row, 0);
        generateAndCheck(row, 0, nq);
        auto end = std::chrono::system_clock::now();       // 計測終了時刻を保存
        auto dur = end - start;        // 要した時間を計算
        auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        cout << nq << ": count = " << g_count << ",\t";
        //cout << "nNode = " << g_nNode << ",\t";
        cout << "dur = " << msec << "msec\n";
    }
    cout << "\n";
}
// マルチスレッド用
//  n: 配置済みクイーン数
int generateAndCheckMT(char row[], int n, int NQ)
{
    int count = 0;
    for(row[n] = 1; row[n] <= NQ; ++row[n]) {
        if( n == NQ - 1 ) {
            if( check(row, NQ) )
                ++count;    // 解を発見
        } else
            count += generateAndCheckMT(row, n+1, NQ);
    }
    return count;
}
int generateAndCheckMT0(int n, int NQ)
{
    vector<char> row(NQ);
    row[0] = n;
    return generateAndCheckMT(&row[0], 1, NQ);
}
void test_BF_MT()
{
    cout << "test_BF_MT()\n";
    const int MAX_NQ = 9;
    char row[MAX_NQ][MAX_NQ];    //  各行のウィーン位置、[1, NQ]
    int nq = MAX_NQ;
        auto start = std::chrono::system_clock::now();      // 計測スタート時刻を保存
        //int count = generateAndCheckMT(row, 0, nq);
        vector<std::future<int>> lst;
        for (int i = 0; i < nq; ++i) {
            //row[i][0] = i + 1;
            //auto f = std::async(std::launch::async, generateAndCheckMT, row[i], 1, nq);
            //lst.push_back(std::async(std::launch::async, generateAndCheckMT, row[i], 1, nq));
            lst.push_back(std::async(std::launch::async, generateAndCheckMT0, 1, nq));
        }
        int count = 0;
        for(auto& f: lst)
            count += f.get();
        //for (int i = 0; i != lst.size(); ++i)
        // count += lst[i].get();
        auto end = std::chrono::system_clock::now();       // 計測終了時刻を保存
        auto dur = end - start;        // 要した時間を計算
        auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        cout << nq << ": count = " << count << ",\t";
        //cout << "nNode = " << g_nNode << ",\t";
        cout << "dur = " << msec << "msec\n";
    cout << "\n";
}
