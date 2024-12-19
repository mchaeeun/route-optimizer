#include <limits>    // For INT_MAX
#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include <conio.h> // _getch 사용

using namespace std;

// -- for image --
#include "../opencv/include/opencv2/imgcodecs.hpp" // 이미지 입출력
#include "../opencv/include/opencv2/highgui.hpp" // GUI
#include "../opencv/include/opencv2/imgproc.hpp" // 이미지 처리

using namespace cv;

const int defMaxGraphSize = 700,      // Default number of vertices
infiniteEdgeWt = INT_MAX;  // "Weight" of a missing edge

class Vertex {
public:
    string label;   // Vertex label
public:
    Vertex() { label = ""; }
    Vertex(string l) { label = l; }
};

class WtGraph {
public:

    WtGraph(int maxNumber = defMaxGraphSize);
    ~WtGraph();

    void insertVertex(string newVertex);
    void insertEdge(string v1, string v2, float wt);
    bool retrieveVertex(string v);     // Get vertex
    bool getEdgeWeight(string v1, string v2, float& wt);

    void floyd();
    void trace_path(int u, int v);
    bool print_path(int u, int v);

    int getIndex(string v);
    string getroute(int i) { return route[i].label; }

private:

    float getEdge(int row, int col);
    void setEdge(int row, int col, float wt);

    float getPath(int row, int col);
    void setPath(int row, int col, float wt);

    // Data members
    int maxSize,
        size;
    Vertex* vertexList;
    int* next;
    float* adjMatrix;
    float* pathMatrix;

    Vertex* route;
    int route_size = 0;

};

WtGraph::WtGraph(int maxNumber) {
    maxSize = maxNumber;
    size = 0;

    vertexList = new Vertex[maxSize];

    adjMatrix = new float[maxSize * maxSize];
    pathMatrix = new float[maxSize * maxSize];
    next = new int[maxSize * maxSize];

    route = new Vertex[50];

    for (int i = 0; i < maxSize; i++) {
        for (int j = 0; j < maxSize; j++) {
            adjMatrix[i * maxSize + j] = infiniteEdgeWt;
        }
    }

    for (int i = 0; i < maxSize; i++) {
        for (int j = 0; j < maxSize; j++) {
            pathMatrix[i * maxSize + j] = 0;
        }
    }
}

WtGraph::~WtGraph()
{}

//--------------------------------------------------------------------

int WtGraph::getIndex(string v) {
    for (int i = 0; i < size; i++) {
        if (vertexList[i].label == v) {
            return i;
        }
    }
    return -1;
}

float WtGraph::getEdge(int row, int col)
{
    return adjMatrix[row * maxSize + col];
}

void WtGraph::setEdge(int row, int col, float wt) {
    adjMatrix[row * maxSize + col] = wt;
}

//--------------------------------------------------------------------

void WtGraph::insertVertex(string newVertex) {
    if (!retrieveVertex(newVertex)) {
        Vertex* temp = new Vertex(newVertex);
        vertexList[size++] = *temp;
    }
    return;
}

void WtGraph::insertEdge(string v1, string v2, float wt) {
    Vertex _v1, _v2;
    setEdge(getIndex(v1), getIndex(v2), wt);
    setEdge(getIndex(v2), getIndex(v1), wt);
}

bool WtGraph::retrieveVertex(string v)
{
    for (int i = 0; i < size; i++) {
        if (vertexList[i].label == v) {
            return true;
        }
    }
    return false;
}

bool WtGraph::getEdgeWeight(string v1, string v2, float& wt)
{
    Vertex _v1, _v2;
    if (adjMatrix[getIndex(v1) * maxSize + getIndex(v2)] != 0) {
        wt = getEdge(getIndex(v1) * maxSize, getIndex(v2));
        return true;
    }
    return false;
}

//--------------------------------------------------------------------

float WtGraph::getPath(int row, int col)
{
    return pathMatrix[row * maxSize + col];
}

void WtGraph::setPath(int row, int col, float wt)
{
    pathMatrix[row * maxSize + col] = wt;
}

void WtGraph::floyd()
{
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            pathMatrix[i * maxSize + j] = adjMatrix[i * maxSize + j];
            next[i * maxSize + j] = j;
        }
    }

    for (int m = 0; m < size; m++) {                // m - 경유지
        for (int j = 0; j < size; j++) {            // j - 시작점
            for (int k = 0; k < size; k++) {        // k - 끝점
                if (getPath(j, m) == infiniteEdgeWt || getPath(m, k) == infiniteEdgeWt) {
                    continue;
                }
                if (getPath(j, k) > getPath(j, m) + getPath(m, k)) {
                    setPath(j, k, getPath(j, m) + getPath(m, k));
                    next[j * maxSize + k] = next[j * maxSize + m];
                }
            }
        }
    }
}

void WtGraph::trace_path(int u, int v) {
    if (u != v) {
        u = next[u * maxSize + v];
        route[route_size++].label = vertexList[u].label;
        trace_path(u, v);
    }
}

bool WtGraph::print_path(int u, int v) {
    if (u < 0 || v < 0) {
        return false;
    }
    route[route_size++].label = vertexList[u].label;
    trace_path(u, v);
    return true;
}


class img_node {
private:
    string name;
    float x, y;
public:
    img_node() { name = ""; x = -1; y = -1; }
    img_node(string n, float _x, float _y) { name = n; x = _x; y = _y; }
    string getname() { return name; }
    float getx() { return x; }
    float gety() { return y; }
};

class img {
private:
    img_node* wonarr; // 
    img_node* sinarr;
    int wonarr_size = 0, sinarr_size = 0;
    string prenode_name = ""; Point prenode; // 이전 노드 정보
    Mat loc_img; // 현재 층의 이미지

public:
    img(int ignored) { wonarr = new img_node[300]; sinarr = new img_node[300]; };
    ~img() {};
    bool insertLoc(string n, float x, float y) { // 이미지 상 위치값 저장
        if (!n.rfind("원", 0)) {
            img_node* tmp = new img_node(n, x, y);
            wonarr[wonarr_size++] = *tmp;
            return true;
        }
        else if (!n.rfind("신", 0)) {
            img_node* tmp = new img_node(n, x, y);
            sinarr[sinarr_size++] = *tmp;
            return true;
        }
        return false;

    }
    Mat getimgLoc() {
        return loc_img;
    }
    Point getnodePoint(string n) {
        if (!n.rfind("원", 0)) {
            for (int i = 0; i < wonarr_size; i++) {
                if (wonarr[i].getname() == n)
                    return Point(wonarr[i].getx(), wonarr[i].gety());
            }
        }
        else if (!n.rfind("신", 0)) {
            for (int i = 0; i < sinarr_size; i++) {
                if (sinarr[i].getname() == n)
                    return Point(sinarr[i].getx(), sinarr[i].gety());
            }
        }
        return Point(-1, -1);
    }

    void print_img(string loc_node) {
        if (!prenode_name.compare("")) { // 이미지 출력 첫 번째
            prenode_name = loc_node;
            prenode = getnodePoint(prenode_name);
            if (!loc_node.rfind("원", 0)) {
                string floor = "../resource/img+file/7-" + loc_node.substr(7, 1) + ".png";
                loc_img = imread(floor);
            }
            else if (!loc_node.rfind("신", 0)) {
                string floor = "../resource/img+file/32-" + loc_node.substr(9, 1) + ".png";
                loc_img = imread(floor);
            }
        }
        else {
            if (!loc_node.rfind("원", 0)) {
                if (prenode_name.substr(0, 8) == loc_node.substr(0, 8)) {
                    arrowedLine(loc_img, prenode, getnodePoint(loc_node), Scalar(255, 0, 255), 2);
                    prenode_name = loc_node;
                    prenode = getnodePoint(loc_node);
                }
                else {
                    imshow("", getimgLoc());
                    if (waitKey(0) == 27) {
                        prenode_name = loc_node;
                        prenode = getnodePoint(loc_node);
                        if (!loc_node.rfind("원", 0)) {
                            string floor = "../resource/img+file/7-" + loc_node.substr(7, 1) + ".png";
                            loc_img = imread(floor);
                        }
                        else if (!loc_node.rfind("신", 0)) {
                            string floor = "../resource/img+file/32-" + loc_node.substr(9, 1) + ".png";
                            loc_img = imread(floor);
                        }
                    }
                }
            }
            else if (!loc_node.rfind("신", 0)) {
                if (prenode_name.substr(0, 10) == loc_node.substr(0, 10)) {
                    arrowedLine(loc_img, prenode, getnodePoint(loc_node), Scalar(255, 0, 255), 2);
                    prenode_name = loc_node;
                    prenode = getnodePoint(loc_node);
                }
                else {
                    imshow("", getimgLoc());
                    if (waitKey(0) == 27) {
                        prenode_name = loc_node;
                        prenode = getnodePoint(loc_node);
                        if (!loc_node.rfind("원", 0)) {
                            string floor = "../resource/img+file/7-" + loc_node.substr(7, 1) + ".png";
                            loc_img = imread(floor);
                        }
                        else if (!loc_node.rfind("신", 0)) {
                            string floor = "../resource/img+file/32-" + loc_node.substr(9, 1) + ".png";
                            loc_img = imread(floor);
                        }
                    }

                }
            }
        }
    }
};


// 가능한 명령어 목록
vector<string> commandList = {
    "원흥관", "신공학관"
};

// 입력값을 기반으로 가장 유사한 자동완성 단어를 찾는 함수
string getAutoComplete(const string& input) {
    if (input.empty()) {
        return commandList.front(); // 첫 번째 문자열 반환
    }
    for (auto it = commandList.begin(); it != commandList.end(); ++it) {
        const auto& command = *it;

        // 비교해서 매칭되는 경우 (input과 command가 같을 경우)
        if (input.compare(command) == 0) {
            auto nextIt = next(it);
            // 현재 command의 다음 항목 반환
            if (nextIt != commandList.end()) {
                return *nextIt; // 다음 항목 반환
            }
            else {
                return ""; // 다음 항목이 없다면 빈 문자열 반환
            }
        }

        // 접두어 매칭
        if (command.find(input) == 0) {
            return command;
        }
    }
    return ""; // 일치하는 단어가 없으면 빈 문자열 반환
}

void setEdge(string& edge) {
    char ch;
    // 입력 루프
    while (true) {
        ch = _getch();
        if (ch == '\r') { // Enter 키 (입력 완료)
            cout << endl;
            break; // 입력 종료
        }
        else if (ch == '\b') { // Backspace 키 (입력 삭제)
            if (!edge.empty()) {
                edge.pop_back();
                cout << "\b \b"; // 커서를 뒤로 이동하고 공백 출력
            }
        }
        else if (ch == '\t') { // Tab 키 (자동완성)
            string suggestion = getAutoComplete(edge);
            if (!suggestion.empty()) {
                // 커서 이동
                cout << "\033[" << edge.size() << "D" << flush;
                edge = suggestion; // 자동완성 단어로 교체
                cout << edge; // 현재 입력 상태를 새로 출력
            }
            else {

            }
        }
        else if (ch == 26) { // Ctrl + Z (ASCII 코드 26)
            exit(0); // 프로그램 종료
        }
        else {
            edge += ch; // 일반 키 입력 추가
            cout << ch;
        }
    }
    if (edge.empty()) {
        cout << "※ 지원하지 않는 장소이거나 잘못된 입력입니다" << endl;
    }
}

int main() {
    WtGraph testGraph(defMaxGraphSize);         // Test graph
    string v1, v2;   // Vertex labels
    char cmd;                     // Input command
    float wt;                       // Edge weight

    // ---- 데이터 파일 읽기 ----
    string str_buf;
    fstream fs;
    fs.open("../resource/edges.csv", ios::in);

    while (!fs.eof()) {
        getline(fs, v1, ',');
        testGraph.insertVertex(v1);
        getline(fs, v2, ',');
        testGraph.insertVertex(v2);
        getline(fs, str_buf, '\n');
        testGraph.insertEdge(v1, v2, stof(str_buf));
    }
    fs.close();

    // ---- 플로이드 와샬 ----

    testGraph.floyd();

    cout << "Route Optimizer! " << endl;
    cout << "동국대학교 원흥관과 신공학관 건물 내 최단 거리 경로를 제공합니다" << endl;
    cout << "※ 프로그램 종료를 원하면 Ctrl + Z를 누르세요\n" << endl;

    while (true) {
        string start = "",
            end = "";
        while (start.empty()) {
            cout << "출발 강의실을 입력하세요 (ex 원흥관 314): ";
            setEdge(start);
        }
        while (end.empty()) {
            cout << "도착 강의실을 입력하세요 (ex 신공학관 6119): ";
            setEdge(end);
        }

        // -- 이미지 출력 --
        img testImg(300);

        // ---- 이미지 노드 데이터 파일 읽기 ----
        fstream fsi;
        string buf;
        string v; float x, y;
        fsi.open("../resource/img_node.csv", ios::in);
        while (!fsi.eof()) {
            getline(fsi, buf, ',');
            v = buf;
            getline(fsi, buf, ',');
            x = stof(buf);
            getline(fsi, buf, '\n');
            y = stof(buf);
            testImg.insertLoc(v, x, y);
        }
        fsi.close();

        // -- 경로 생성 --
        bool isFound = testGraph.print_path(testGraph.getIndex(start), testGraph.getIndex(end));

        // 경로를 찾은 경우
        if (isFound) {
            // -- 텍스트 경로 생성 --
            cout << "* 경로 *" << endl;

            for (int i = 0; testGraph.getroute(i) != ""; i++) {
                cout << testGraph.getroute(i) << endl;
            }
            // -- 이미지 경로 생성 --
            for (int i = 0; testGraph.getroute(i) != ""; i++) {
                testImg.print_img(testGraph.getroute(i));
            }

            imshow("", testImg.getimgLoc());
            waitKey(0);
        } else { // 경로를 못 찾은 경우
            cout << "경로를 찾을 수 없습니다\n" << endl;
        }
    }
}