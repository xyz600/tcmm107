#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

#define TEST

class PointsOnGrid
{
public:
    vector<string> findSolution(int H, int W, int h, int w, int Kmin, int Kmax, vector<string> board);

private:
    int H;
    int W;
    int h;
    int w;
    int Kmin;
    int Kmax;

    void markGreedy(vector<string>& result, const vector<string>& input);

    pair<int, int> minmaxCountInGrid(const vector<vector<int>>& counter_in_grid, int sy, int sx);

    void incrementCountInGrid(vector<vector<int>>& counter_in_grid, int sy, int sx);
};

void PointsOnGrid::incrementCountInGrid(vector<vector<int>>& counter_in_grid, int y, int x)
{
    const int sy = max(0, y - h + 1);
    const int sx = max(0, x - w + 1);
    const int ey = min(H - h, y);
    const int ex = min(W - w, x);

    for (int i = sy; i <= ey; i++)
    {
        for (int j = sx; j <= ex; j++)
        {
            counter_in_grid[i][j]++;
        }
    }
}

pair<int, int> PointsOnGrid::minmaxCountInGrid(const vector<vector<int>>& counter_in_grid, const int y, const int x)
{
    const int sy = max(0, y - h + 1);
    const int sx = max(0, x - w + 1);
    const int ey = min(H - h, y);
    const int ex = min(W - w, x);

    int min_count = 0;
    int max_count = 0;

    for (int i = sy; i <= ey; i++)
    {
        for (int j = sx; j <= ex; j++)
        {
            max_count = max(max_count, counter_in_grid[i][j]);
            min_count = min(min_count, counter_in_grid[i][j]);
        }
    }
    return make_pair(min_count, max_count);
}

void PointsOnGrid::markGreedy(vector<string>& result, const vector<string>& input)
{
    vector<vector<int>> counter_in_grid(H - h + 1, vector<int>(W - w + 1, 0));

    vector<tuple<int, int, int>> val_pos;
    for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < W; j++)
        {
            val_pos.emplace_back(input[i][j] - '0', i, j);
        }
    }
    sort(val_pos.begin(), val_pos.end(), greater<>());

    for (auto v : val_pos)
    {
        int val, i, j;
        tie(val, i, j) = v;
        int min_count, max_count;
        tie(min_count, max_count) = minmaxCountInGrid(counter_in_grid, i, j);
        if (max_count < Kmax)
        {
            incrementCountInGrid(counter_in_grid, i, j);
            result[i][j] = 'x';
        }
    }
}

vector<string> PointsOnGrid::findSolution(const int H, const int W, const int h, const int w, const int Kmin, const int Kmax, vector<string> board)
{
    this->H = H;
    this->W = W;
    this->h = h;
    this->w = w;
    this->Kmin = Kmin;
    this->Kmax = Kmax;

    vector<string> ret(H, string(W, '.'));

    markGreedy(ret, board);

    return ret;
}

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
#endif // TEST