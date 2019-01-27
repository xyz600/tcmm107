#include <ctime>
#include <iomanip>
#include <iostream>
#include <vector>

#define TEST

using namespace std;

// clock -> sec は、CLOCKS_PER_SEC (in <ctime>)
unsigned long long int getCycle()
{
    unsigned int low, high;
    __asm__ volatile("rdtsc"
                     : "=a"(low), "=d"(high));
    return ((unsigned long long int)low) | ((unsigned long long int)high << 32);
}

class PointsOnGrid
{
public:
    vector<string> findSolution(int H, int W, int h, int w, int Kmin, int Kmax, vector<string> board)
    {
        vector<string> ret(H, string(W, '.'));

        unsigned long long int begin_cycle = getCycle();
        int begin_clocks = clock();
        unsigned long long int end_cycle = getCycle();
        int end_clocks = clock();
        cerr << begin_clocks << " " << begin_cycle << endl;
        cerr << end_clocks << " " << end_cycle << endl;
        cerr << (double)(end_clocks - begin_clocks) / CLOCKS_PER_SEC << endl;
        cerr << fixed << setprecision(20) << ((double)(end_clocks - begin_clocks) / CLOCKS_PER_SEC) / (end_cycle - begin_cycle) << endl;
        return ret;
    }
};

// -------8<------- end of solution submitted to the website -------8<-------
#ifdef TEST
template <class T>
void getVector(vector<T>& v)
{
    for (int i = 0; i < v.size(); ++i)
        cin >> v[i];
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    PointsOnGrid pog;
    int H;
    int W;
    int h;
    int w;
    int Kmin;
    int Kmax;
    int size;
    cin >> H;
    cin >> W;
    cin >> h;
    cin >> w;
    cin >> Kmin;
    cin >> Kmax;
    cin >> size;
    vector<string> grid(size);
    getVector(grid);

    vector<string> ret = pog.findSolution(H, W, h, w, Kmin, Kmax, grid);
    cout << ret.size() << endl;
    for (int i = 0; i < (int)ret.size(); ++i)
        cout << ret[i] << endl;
    cout.flush();
}
#endif