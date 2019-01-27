#include <algorithm>
#include <cstdint>
#include <ctime>
#include <functional>
#include <iostream>
#include <random>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

#define TEST

constexpr uint64_t MY_CLOCK_PER_SEC = 2800000000;

// clock -> sec は、CLOCKS_PER_SEC
uint64_t getCycle()
{
    uint32_t low, high;
    __asm__ volatile("rdtsc"
                     : "=a"(low), "=d"(high));
    return ((uint64_t)low) | ((uint64_t)high << 32);
}

uint64_t getMillisecTime(const uint64_t start_clock)
{
    return (getCycle() - start_clock) * 1000ull / MY_CLOCK_PER_SEC;
}

uint64_t getMicrosecTime(const uint64_t start_clock)
{
    return (getCycle() - start_clock) * 1000000ull / MY_CLOCK_PER_SEC;
}

class PointsOnGrid
{
public:
    vector<string> findSolution(int H, int W, int h, int w, int Kmin, int Kmax, vector<string> board_str);

private:
    using BoardType = vector<vector<uint8_t>>;
    using CounterBoardType = vector<vector<int>>;

    // 属性は 全て const
    int H;
    int W;
    int h;
    int w;
    int Kmin;
    int Kmax;

    BoardType board;

    uint64_t start_clock;

    int markSA(vector<string>& result);

    int markGreedy(vector<string>& result, CounterBoardType& counter_in_subgrid);

    pair<int, int> minmaxCountInGrid(const CounterBoardType& counter_in_subgrid, int sy, int sx);

    pair<int, int> minmaxCountInGridAll(const CounterBoardType& counter_in_subgrid);

    void incrementCountInGrid(CounterBoardType& counter_in_subgrid, int sy, int sx);

    void decrementCountInGrid(CounterBoardType& counter_in_subgrid, int sy, int sx);

    double calculateScoreDiff(double time_rest_rate, const CounterBoardType& counter_in_subgrid, int y, int x, const vector<string>& result);

    double calculatePenaltyDiff(double time_rest_rate, const CounterBoardType& counter_in_subgrid, int y, int x, int diff);
};

double PointsOnGrid::calculatePenaltyDiff(double time_rest_rate, const CounterBoardType& counter_in_subgrid, int y, int x, int diff_count)
{
    const int sy = max(0, y - h + 1);
    const int sx = max(0, x - w + 1);
    const int ey = min(H - h, y);
    const int ex = min(W - w, x);

    double diff = 0.0;

    auto coef = 1.0 + time_rest_rate * 4;

    auto getPenalty = [&](int c) {
        double raw = 0;
        if (c < Kmin)
        {
            raw = (Kmin - c);
        }
        else if (Kmax < c)
        {
            raw = Kmax - c;
        }
        raw *= coef;
        return raw * raw;
    };

    for (int i = sy; i <= ey; i++)
    {
        for (int j = sx; j <= ex; j++)
        {
            diff += getPenalty(counter_in_subgrid[i][j]);
            diff -= getPenalty(counter_in_subgrid[i][j] + diff_count);
        }
    }
    return diff;
}

double PointsOnGrid::calculateScoreDiff(const double time_rest_rate, const CounterBoardType& counter_in_subgrid, const int y, const int x, const vector<string>& result)
{
    if (result[y][x] == 'x')
    {
        return -board[y][x] + calculatePenaltyDiff(time_rest_rate, counter_in_subgrid, y, x, -1);
    }
    else
    {
        return board[y][x] + calculatePenaltyDiff(time_rest_rate, counter_in_subgrid, y, x, 1);
    }
}

void PointsOnGrid::decrementCountInGrid(CounterBoardType& counter_in_subgrid, int y, int x)
{
    const int sy = max(0, y - h + 1);
    const int sx = max(0, x - w + 1);
    const int ey = min(H - h, y);
    const int ex = min(W - w, x);

    for (int i = sy; i <= ey; i++)
    {
        for (int j = sx; j <= ex; j++)
        {
            counter_in_subgrid[i][j]--;
        }
    }
}

void PointsOnGrid::incrementCountInGrid(CounterBoardType& counter_in_subgrid, int y, int x)
{
    const int sy = max(0, y - h + 1);
    const int sx = max(0, x - w + 1);
    const int ey = min(H - h, y);
    const int ex = min(W - w, x);

    for (int i = sy; i <= ey; i++)
    {
        for (int j = sx; j <= ex; j++)
        {
            counter_in_subgrid[i][j]++;
        }
    }
}

pair<int, int> PointsOnGrid::minmaxCountInGridAll(const CounterBoardType& counter_in_subgrid)
{
    int min_count = 0;
    int max_count = 0;

    for (int i = 0; i <= H - h; i++)
    {
        for (int j = 0; j <= W - w; j++)
        {
            max_count = max(max_count, counter_in_subgrid[i][j]);
            min_count = min(min_count, counter_in_subgrid[i][j]);
        }
    }
    return make_pair(min_count, max_count);
}

pair<int, int> PointsOnGrid::minmaxCountInGrid(const CounterBoardType& counter_in_subgrid, const int y, const int x)
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
            max_count = max(max_count, counter_in_subgrid[i][j]);
            min_count = min(min_count, counter_in_subgrid[i][j]);
        }
    }
    return make_pair(min_count, max_count);
}

int PointsOnGrid::markSA(vector<string>& result)
{
    CounterBoardType counter_in_subgrid(H - h + 1, vector<int>(W - w + 1, 0));

    int score = markGreedy(result, counter_in_subgrid);

    mt19937_64 mt;
    uniform_int_distribution<size_t> rand_y(0, H - 1);
    uniform_int_distribution<size_t> rand_x(0, W - 1);
    uniform_real_distribution<double> rand_prob;

    vector<pair<size_t, size_t>> position_list;
    for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < W; j++)
        {
            if (board[i][j] != 0)
            {
                position_list.emplace_back(i, j);
            }
        }
    }

    int count = 0;
    constexpr uint64_t time_limit = 2000;
    uint64_t current_time = getMillisecTime(start_clock);
    double time_rest_rate = static_cast<double>(getMillisecTime(start_clock)) / time_limit;

    while (current_time <= time_limit)
    {
        shuffle(position_list.begin(), position_list.end(), mt);
        for (auto yx : position_list)
        {
            size_t y, x;
            tie(y, x) = yx;

            const double diff = calculateScoreDiff(time_rest_rate, counter_in_subgrid, y, x, result);

            if (diff > 0 || rand_prob(mt) <= exp(diff * time_rest_rate))
            {
                if (result[y][x] == 'x')
                {
                    result[y][x] = '.';
                    score -= board[y][x];
                    decrementCountInGrid(counter_in_subgrid, y, x);
                }
                else
                {
                    result[y][x] = 'x';
                    score += board[y][x];
                    incrementCountInGrid(counter_in_subgrid, y, x);
                }
            }
            count++;
        }
        current_time = getMillisecTime(start_clock);
        time_rest_rate = static_cast<double>(getMillisecTime(start_clock)) / time_limit;
    }
    cerr << count << endl;

    return score;
}

int PointsOnGrid::markGreedy(vector<string>& result, CounterBoardType& counter_in_subgrid)
{
    int score = 0;

    vector<tuple<int, int, int>> val_pos;
    for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < W; j++)
        {
            val_pos.emplace_back(board[i][j], i, j);
        }
    }
    sort(val_pos.begin(), val_pos.end(), greater<>());

    for (auto v : val_pos)
    {
        int val, i, j;
        tie(val, i, j) = v;
        int min_count, max_count;
        tie(min_count, max_count) = minmaxCountInGrid(counter_in_subgrid, i, j);
        if (max_count < Kmax)
        {
            incrementCountInGrid(counter_in_subgrid, i, j);
            result[i][j] = 'x';
            score += val;
        }
    }
    return score;
}

vector<string> PointsOnGrid::findSolution(const int H, const int W, const int h, const int w, const int Kmin, const int Kmax, vector<string> board_str)
{
    start_clock = getCycle();

    this->H = H;
    this->W = W;
    this->h = h;
    this->w = w;
    this->Kmin = Kmin;
    this->Kmax = Kmax;

    vector<string> ret(H, string(W, '.'));

    board.resize(H);
    for (auto& row : board)
    {
        row.resize(W);
    }

    for (size_t i = 0; i < H; i++)
    {
        for (size_t j = 0; j < W; j++)
        {
            board[i][j] = board_str[i][j] - '0';
        }
    }

    markSA(ret);

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