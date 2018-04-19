#include <bits/stdc++.h>
#include <functional>
#include <ncurses.h>

#define FORU(i, a, b) for(int (i) = (a); (i) <= (b); ++(i))
#define FOR(i, n) FORU((i), 0, (n)-1)

#define KEY_LEFT 68
#define KEY_RIGHT 67
#define KEY_ESC 113

using namespace std;

struct Piece {
    int a, b;
    int x, y;
    Piece(int a, int b, int x, int y) : a(a), b(b), x(x), y(y) {}
    bool operator<(const Piece& d) const {
        if(a != d.a) return a < d.a;
        if(b != d.b) return b < d.b;
        if(x != d.x) return x < d.x;
        return y < d.y;
    }
    bool operator!=(const Piece& b) const {
        return *this < b || b < *this;
    }
};

const int DIRS[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
const char* UNI[7] = {" ", "╚", "╝", "╔", "╗", "═", "║"};

class Board {
    public:
    int n, m;
    vector<vector<int>> free;
    vector<Piece> pieces;
    set<Piece> op;

    public:
    Board(){}
    Board(int n, int m, Piece p) : n(n), m(m) {
        FOR(i, n) {
            free.push_back(vector<int>());
            FOR(j, m)
                free[i].push_back(-1);
        }
        add_piece(p);
    }
    
    void print(bool std=false) {
        vector<vector<int>> grid;
        vector<vector<bool>> goal;
        FOR(i, 2*n) {
            grid.push_back(vector<int>());
            goal.push_back(vector<bool>());
            FOR(j, 2*m) {
                grid[i].push_back(0);
                goal[i].push_back(0);
            }
        }
        { Piece& p = pieces[0];
        FORU(i, 2*p.x, 2*(p.x+p.a)-1)
            FORU(j, 2*p.y, 2*(p.y+p.b)-1)
                goal[i][j] = 1;
        }
        for(auto p : pieces) {
            FORU(i, 2*p.x+1, 2*(p.x+p.a-1)) grid[i][2*p.y] = grid[i][2*(p.y+p.b)-1] = 6;
            FORU(i, 2*p.y+1, 2*(p.y+p.b-1)) grid[2*p.x][i] = grid[2*(p.x+p.a)-1][i] = 5;
            grid[2*p.x][2*p.y] = 3;
            grid[2*(p.x+p.a)-1][2*p.y] = 1;
            grid[2*p.x][2*(p.y+p.b)-1] = 4;
            grid[2*(p.x+p.a)-1][2*(p.y+p.b)-1] = 2;
        }
        if(std)
            for(auto row : grid) {
                for(auto cell : row)
                    printf("%s", UNI[cell]);
                printf("\n");
            }
        else {
            FOR(i, 2*n) {
                FOR(j, 2*m) {
                    int cell = grid[i][j];
                    if(goal[i][j])
                        attron(COLOR_PAIR(1));
                    printw("%s", UNI[cell]);
                    if(goal[i][j])
                        attroff(COLOR_PAIR(1));
                }
                printw("\n");
            }
            refresh();
        }
    }

    void add_piece(Piece p) {
        set_piece(p, pieces.size());
        pieces.push_back(p);
        op.insert(p);
    }

    void set_piece(Piece& p, int v = -1) {
        FORU(i, p.x, p.x+p.a-1)
            FORU(j, p.y, p.y+p.b-1)
                free[i][j] = v;
    }

    void move_piece(int p, int d) {
        op.erase(pieces[p]);
        set_piece(pieces[p]);
        pieces[p].x += DIRS[d][0];
        pieces[p].y += DIRS[d][1];
        op.insert(pieces[p]);
        set_piece(pieces[p], p);
    }

    bool operator<(const Board& b) const {
        return op < b.op;
    }

    bool operator==(const Board& b) const {
        if(n != b.n || m != b.m) return false;
        if(pieces.size() != b.pieces.size()) return false;
        set<Piece>::iterator ia = op.begin(), ib = b.op.begin();
        while(ia != op.end()) {
            if(*ia != *ib)
                return false;
            ++ia; ++ib;
        }
        return true;
    }

    vector<pair<int, int>> moves(int ip = -1) {
        vector<pair<int, int>> r;
        FOR(i, (int)pieces.size()) if(ip == -1 || ip == i) {
            Piece& p = pieces[i];
            set_piece(p);
            FOR(j, 4) {
                int x = p.x + DIRS[j][0];
                int y = p.y + DIRS[j][1];
                if(x >= 0 && x + p.a <= n && y >= 0 && y + p.b <= m) {
                    bool ok = 1;
                    FORU(k, x, x+p.a-1)
                        FORU(l, y, y+p.b-1)
                            if(free[k][l] != -1) {
                                ok = 0;
                                break;
                            }
                    if(ok) r.push_back({i, j});
                }
            }
            set_piece(p, i);
        }
        return r;
    }
};

struct BoardHash {
    int operator()(const Board& b) const {
        long long int h = 0;
        for(auto row : b.free)
            for(auto cell : row)
                h = (h * (b.n * b.m + 1) + cell + 1) % 1000000007;
        return h;
    }
};

map<Board, pair<int, Board>> bfs(Board b, int X, int Y) {
    map<Board, pair<int, Board>> dv;
    auto f = [X, Y](const pair<Board, pair<int, Board>>& a,
                const pair<Board, pair<int, Board>>& b) -> bool {
        int A = a.second.first + abs(X - a.first.pieces[0].x) + abs(Y - a.first.pieces[0].y);
        int B = b.second.first + abs(X - b.first.pieces[0].x) + abs(Y - b.first.pieces[0].y);
        return A > B;
    };
    priority_queue<pair<Board, pair<int, Board>>,
                   vector<pair<Board, pair<int, Board>>>,
                   decltype(f)> file(f);
    file.push({b, {0, b}});
    int best = 13;
    while(file.size()) {
        auto cur = file.top();
        file.pop();
        if(dv.count(cur.first) == 0) {
            dv.insert(cur);
            //if(dv.size() % 100 == 0) printf("%d\n", (int)dv.size());
            //int dd = abs(X - cur.first.pieces[0].x) + abs(Y - cur.first.pieces[0].y);
            //if(dd < best) {
            //    best = dd;
            //    cur.first.print(true);
            //    printf("\n");
            //}
            //printf("%d\n", (int)dv.size());
            if(cur.first.pieces[0].x == X && cur.first.pieces[0].y == Y)
                return dv;
            Board& b = cur.first;
            for(auto move : b.moves()) {
                Board nouv = Board(b);
                nouv.move_piece(move.first, move.second);
                file.push({nouv, {cur.second.first + 1, b}});
                //for(auto movep : nouv.moves(move.first)) {
                //    Board nouvp = Board(nouv);
                //    nouvp.move_piece(movep.first, movep.second);
                //    file.push({nouvp, {cur.second.first + 1, b}});
                //}
            }
        }
    }
    return dv;
}

int main(int _, char* argv[]) {
    FILE* f = fopen(argv[1], "r");
    int n, m, a, c, x, y, xf, yf, N;
    fscanf(f, "%d%d%d%d%d%d%d%d%d", &n, &m, &a, &c, &x, &y, &xf, &yf, &N);
    Board b(n, m, {a, c, x, y});
    FOR(i, N) {
        fscanf(f, "%d%d%d%d", &a, &c, &x, &y);
        b.add_piece({a, c, x, y});
    }
    b.print(true);
    auto r = bfs(b, xf, yf);
    Board b0;
    int dist = 1E9;
    for(auto bb : r) if(bb.first.pieces[0].x == xf && bb.first.pieces[0].y == yf)
        if(dist > bb.second.first) {
            dist = bb.second.first;
            b0 = bb.first;
        }
    vector<Board> path;
    do {
        path.push_back(b0);
        b0 = r[b0].second;
    } while(!(b0 == b));
    path.push_back(b0);
    reverse(path.begin(), path.end());
    int i = 0;
    int C;
    setlocale(LC_ALL,"");

    initscr();
    cbreak();
    noecho();
    start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);

    bool ok = 1;
    while(ok) {
        erase();
        printw("%d / %d\n", i, (int)path.size()-1);
        path[i].print();
        switch(C=getch()) {
            case KEY_LEFT:
                i = max(0, i-1);
                break;
            case KEY_RIGHT:
                i = min<int>(path.size()-1, i+1);
                break;
            case KEY_ESC:
                ok = 0;
                break;
            default:
                break;
        }
    }
	endwin();
    printf("%d\n", dist);
    return 0;
}
