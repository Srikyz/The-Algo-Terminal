#include <bits/stdc++.h>
#include <windows.h>
#include <chrono>
using namespace std;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
void setColor(int code) { SetConsoleTextAttribute(hConsole, code); }

enum Cell { EMPTY, WALL, START, GOAL, PATH, VISITED };

char cellChar(Cell c) {
    switch (c) {
        case EMPTY: return '.';
        case WALL: return '#';
        case START: return 'S';
        case GOAL: return 'G';
        case PATH: return '*';
        case VISITED: return '+';
    }
    return '?';
}

void printGrid(const vector<vector<Cell>>& grid) {
    for (const auto& row : grid) {
        for (Cell c : row) {
            switch (c) {
                case EMPTY: setColor(7); break;
                case WALL: setColor(8); break;
                case START: setColor(10); break;
                case GOAL: setColor(12); break;
                case PATH: setColor(14); break;
                case VISITED: setColor(9); break;
            }
            cout << cellChar(c) << ' ';
        }
        cout << '\n';
    }
    setColor(7);
}

void clearScreen() { system("cls"); }
void sleepMS(int ms) { Sleep(ms); }

bool isValid(int r, int c, const vector<vector<Cell>>& grid, const vector<vector<bool>>& visited) {
    return r >= 0 && r < (int)grid.size() && c >= 0 && c < (int)grid[0].size() && grid[r][c] != WALL && !visited[r][c];
}

void bfsGraph(int start, const vector<vector<pair<int, int>>>& adj, int n) {
    vector<bool> visited(n, false);
    queue<int> q;
    q.push(start);
    visited[start] = true;
    cout << "BFS: ";
    while (!q.empty()) {
        int node = q.front(); q.pop();
        cout << node << ' ';
        for (auto [neigh, _] : adj[node]) {
            if (!visited[neigh]) {
                visited[neigh] = true;
                q.push(neigh);
            }
        }
    }
    cout << '\n';
}

void dfsGraphUtil(int node, const vector<vector<pair<int, int>>>& adj, vector<bool>& visited) {
    visited[node] = true;
    cout << node << ' ';
    for (auto [neigh, _] : adj[node]) {
        if (!visited[neigh]) dfsGraphUtil(neigh, adj, visited);
    }
}

void dfsGraph(int start, const vector<vector<pair<int, int>>>& adj, int n) {
    vector<bool> visited(n, false);
    cout << "DFS: ";
    dfsGraphUtil(start, adj, visited);
    cout << '\n';
}

void printGraphEdges(const vector<vector<pair<int, int>>>& adj) {
    cout << "Graph Edges:\n";
    for (int u = 0; u < (int)adj.size(); ++u) {
        for (auto [v, w] : adj[u]) {
            cout << u << " -> " << v << " (weight: " << w << ")\n";
        }
    }
}

void bfs(vector<vector<Cell>>& grid, pair<int, int> start, pair<int, int> goal) {
    int R = grid.size(), C = grid[0].size();
    queue<pair<int, int>> q;
    vector<vector<bool>> visited(R, vector<bool>(C, false));
    vector<vector<pair<int, int>>> parent(R, vector<pair<int, int>>(C, {-1, -1}));
    q.push(start);
    visited[start.first][start.second] = true;
    const vector<pair<int, int>> dirs = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    while (!q.empty()) {
        auto [r, c] = q.front(); q.pop();
        if (make_pair(r, c) != start && make_pair(r, c) != goal) grid[r][c] = VISITED;
        clearScreen(); printGrid(grid); sleepMS(30);
        if (make_pair(r, c) == goal) break;
        for (auto [dr, dc] : dirs) {
            int nr = r + dr, nc = c + dc;
            if (isValid(nr, nc, grid, visited)) {
                visited[nr][nc] = true;
                parent[nr][nc] = {r, c};
                q.push({nr, nc});
            }
        }
    }
    for (auto cur = goal; cur != start && parent[cur.first][cur.second] != make_pair(-1, -1); cur = parent[cur.first][cur.second]) {
        if (cur != start) grid[cur.first][cur.second] = PATH;
        clearScreen(); printGrid(grid); sleepMS(30);
    }
}

struct Node {
    int r, c, g, h, f;
    pair<int, int> parent;
    bool operator<(const Node& o) const { return f > o.f; }
};

int heur(pair<int, int> a, pair<int, int> b) {
    return abs(a.first - b.first) + abs(a.second - b.second);
}

void aStar(vector<vector<Cell>>& grid, pair<int, int> start, pair<int, int> goal) {
    int R = grid.size(), C = grid[0].size();
    vector<vector<int>> gScore(R, vector<int>(C, INT_MAX));
    vector<vector<bool>> closed(R, vector<bool>(C, false));
    vector<vector<pair<int, int>>> parent(R, vector<pair<int, int>>(C, {-1, -1}));
    priority_queue<Node> pq;
    gScore[start.first][start.second] = 0;
    pq.push({start.first, start.second, 0, heur(start, goal), heur(start, goal), {-1, -1}});
    const vector<pair<int, int>> dirs = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    while (!pq.empty()) {
        auto cur = pq.top(); pq.pop();
        if (closed[cur.r][cur.c]) continue;
        closed[cur.r][cur.c] = true;
        if (make_pair(cur.r, cur.c) != start && make_pair(cur.r, cur.c) != goal) grid[cur.r][cur.c] = VISITED;
        clearScreen(); printGrid(grid); sleepMS(30);
        if (make_pair(cur.r, cur.c) == goal) break;
        for (auto [dr, dc] : dirs) {
            int nr = cur.r + dr, nc = cur.c + dc;
            if (nr < 0 || nr >= R || nc < 0 || nc >= C || grid[nr][nc] == WALL || closed[nr][nc]) continue;
            int tg = cur.g + 1;
            if (tg < gScore[nr][nc]) {
                gScore[nr][nc] = tg;
                parent[nr][nc] = {cur.r, cur.c};
                int h = heur({nr, nc}, goal);
                pq.push({nr, nc, tg, h, tg + h, {cur.r, cur.c}});
            }
        }
    }
    for (auto cur = goal; cur != start && parent[cur.first][cur.second] != make_pair(-1, -1); cur = parent[cur.first][cur.second]) {
        if (cur != start) grid[cur.first][cur.second] = PATH;
        clearScreen(); printGrid(grid); sleepMS(30);
    }
}

bool parseGrid(vector<vector<Cell>>& grid, pair<int, int>& s, pair<int, int>& g, int R, int C) {
    int sc = 0, gc = 0; cin.ignore();
    cout << "Enter the grid (" << R << "x" << C << ") using . # S G:\n";
    for (int r = 0; r < R; ++r) {
        string line; getline(cin, line);
        while ((int)line.size() < C) { cout << "Line too short, re-enter:\n"; getline(cin, line); }
        if ((int)line.size() > C) line = line.substr(0, C);
        for (int c = 0; c < C; ++c) {
            char ch = line[c];
            switch (ch) {
                case '.': grid[r][c] = EMPTY; break;
                case '#': grid[r][c] = WALL; break;
                case 'S': grid[r][c] = START; s = {r, c}; ++sc; break;
                case 'G': grid[r][c] = GOAL; g = {r, c}; ++gc; break;
                default: cout << "Invalid char " << ch << "\n"; return false;
            }
        }
    }
    if (sc != 1 || gc != 1) { cout << "Need exactly one S and one G.\n"; return false; }
    return true;
}

void banner() {
    const vector<string> spinner = {"|", "/", "-", "\\"};
    for (int i = 0; i < 8; ++i) {
        clearScreen();
        setColor(14);
        cout << "\n\n           " << spinner[i % 4] << " WELCOME TO ";
        cout << "TheAlgoTerminal 1.0 " << spinner[i % 4] << "\n";
        setColor(7);
        Sleep(100);
    }
    clearScreen();
    setColor(11);
    cout << "====================================\n";
    cout << "   Welcome to TheAlgoTerminal 1.0   \n";
    cout << "====================================\n";
    setColor(7);
}

int mainMenu() {
    cout << "\nMain Topics:\n";
    cout << "1. Graphs\n2. Trees (coming soon)\n3. Sorting (coming soon)\n0. Exit\nChoice: ";
    int x; cin >> x; return x;
}

int graphsMenu() {
    cout << "\nGraphs Menu:\n";
    cout << "1. Nodes & Edges\n2. Visualize Graph (Grid Path-finding)\n0. Back\nChoice: ";
    int x; cin >> x; return x;
}

void nodesEdgesDemo() {
    cout << "--- Nodes & Edges ---\n";
    bool directed;
    cout << "Is the graph directed? (1 for Yes, 0 for No): ";
    cin >> directed;

    int n, e; cout << "Enter number of nodes: "; cin >> n;
    cout << "Enter number of edges: "; cin >> e;
    vector<vector<pair<int, int>>> adj(n);
    cout << "Enter edges in format (u v w), u to v with weight w:\n";
    for (int i = 0; i < e; ) {
        int u, v, w; cin >> u >> v >> w;
        if (u < 0 || v < 0 || u >= n || v >= n) {
            cout << "Invalid edge: (" << u << ", " << v << ") — node out of bounds. Try again.\n";
            continue;
        }
        adj[u].push_back({v, w});
        if (!directed) adj[v].push_back({u, w});
        ++i;
    }

    cout << "\nAdjacency List:\n";
    for (int i = 0; i < n; ++i) {
        cout << i << ":";
        for (auto [v, w] : adj[i]) cout << " (" << v << ", w=" << w << ")";
        cout << "\n";
    }

    int choice, src;
    while (true) {
        cout << "\nOptions:\n1. Print Graph (Edges)\n2. BFS\n3. DFS\n4. Back\nChoice: ";
        cin >> choice;
        if (choice == 4) break;
        if (choice == 1) {
            printGraphEdges(adj);
            continue;
        }
        cout << "Enter starting node: ";
        cin >> src;
        if (src < 0 || src >= n) {
            cout << "Invalid source node. Try again.\n";
            continue;
        }
        if (choice == 2) bfsGraph(src, adj, n);
        else if (choice == 3) dfsGraph(src, adj, n);
        else cout << "Invalid option.\n";
    }
}

void visualizeGraph() {
    cout << "--- Grid Path-finding ---\n";
    int R, C; cout << "Rows: "; cin >> R; cout << "Cols: "; cin >> C;

    while (true) {
        vector<vector<Cell>> grid(R, vector<Cell>(C, EMPTY));
        pair<int, int> s{-1, -1}, g{-1, -1};

        cout << "1. Custom Input\n"; 
        cout << "2. Generate Random Grid\n";
        cout << "3. Go Back\n";
        cout << "Choice: ";
        int opt; cin >> opt;

        if (opt == 3) break;

        if (opt == 1) {
            cin.ignore();
            while (!parseGrid(grid, s, g, R, C)) {
                cout << "Try again...\n";
            }
        } else {
            srand(time(0));
            for (int r = 0; r < R; ++r) {
                for (int c = 0; c < C; ++c) {
                    int rnd = rand() % 100;
                    if (rnd < 20) grid[r][c] = WALL;
                    else grid[r][c] = EMPTY;
                }
            }
            do { s = {rand() % R, rand() % C}; } while (grid[s.first][s.second] == WALL);
            do { g = {rand() % R, rand() % C}; } while (grid[g.first][g.second] == WALL || g == s);
            grid[s.first][s.second] = START;
            grid[g.first][g.second] = GOAL;
        }

        while (true) {
            clearScreen(); printGrid(grid);
            cout << "\nAlgorithm: 1.BFS 2.A* 3.Regenerate Grid 4.Back\nChoice: ";
            int ch; cin >> ch;
            if (ch == 3) break;
            if (ch == 4) return;
            auto tempGrid = grid;
            auto t0 = chrono::high_resolution_clock::now();
            if (ch == 1) bfs(tempGrid, s, g);
            else if (ch == 2) aStar(tempGrid, s, g);
            auto t1 = chrono::high_resolution_clock::now();
            cout << "\nTime taken: " << chrono::duration_cast<chrono::milliseconds>(t1 - t0).count() << " ms\n";
            cout << "Press Enter to try again or choose another algorithm..."; cin.ignore(); cin.get();
        }
    }
}

int main() {
    banner();
    while (true) {
        switch (mainMenu()) {
            case 1:
                while (true) {
                    switch (graphsMenu()) {
                        case 1: nodesEdgesDemo(); break;
                        case 2: visualizeGraph(); break;
                        case 0: goto endGraphs;
                        default: cout << "Invalid choice!\n";
                    }
                }
                endGraphs: break;
            case 2: cout << "Trees module coming soon!\n"; break;
            case 3: cout << "Sorting module coming soon!\n"; break;
            case 0: cout << "Goodbye!\n"; return 0;
            default: cout << "Invalid choice!\n";
        }
    }
}
