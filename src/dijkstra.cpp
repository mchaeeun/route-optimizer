#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include<sstream>
#include <cstring>
#include <stdio.h>

// -- for image --
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#define MAX 600 //최대 정점의 개수
#define INF 1e9

using namespace cv;
using namespace std;

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
	void insertLoc(string n, float x, float y) { // 이미지 상 위치값 저장
		if (!n.rfind("원", 0)) {
			img_node* tmp = new img_node(n, x, y);
			wonarr[wonarr_size++] = *tmp;
		}
		else if (!n.rfind("신", 0)) {
			img_node* tmp = new img_node(n, x, y);
			sinarr[sinarr_size++] = *tmp;
		}

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
	}

	void print_img(string loc_node) {
		if (!prenode_name.compare("")) { // 이미지 출력 첫 번째
			prenode_name = loc_node;
			prenode = getnodePoint(prenode_name);
			if (!loc_node.rfind("원", 0)) {
				string floor = "7-" + loc_node.substr(7, 1) + ".png";
				loc_img = imread(floor);
			}
			else if (!loc_node.rfind("신", 0)) {
				string floor = "32-" + loc_node.substr(9, 1) + ".png";
				loc_img = imread(floor);
			}
		}
		else {
			if (!loc_node.rfind("원", 0)) {
				if (prenode_name.substr(0, 8) == loc_node.substr(0, 8)) {
					line(loc_img, prenode, getnodePoint(loc_node), Scalar(255, 0, 255), 3);
					prenode_name = loc_node;
					prenode = getnodePoint(loc_node);
				}
				else {
					imshow("", getimgLoc());
					if (waitKey(0) == 27) {
						prenode_name = loc_node;
						prenode = getnodePoint(loc_node);
						if (!loc_node.rfind("원", 0)) {
							string floor = "7-" + loc_node.substr(7, 1) + ".png";
							loc_img = imread(floor);
						}
						else if (!loc_node.rfind("신", 0)) {
							string floor = "32-" + loc_node.substr(9, 1) + ".png";
							loc_img = imread(floor);
						}
					}
				}
			}
			else if (!loc_node.rfind("신", 0)) {
				if (prenode_name.substr(0, 10) == loc_node.substr(0, 10)) {
					line(loc_img, prenode, getnodePoint(loc_node), Scalar(255, 0, 255), 3);
					prenode_name = loc_node;
					prenode = getnodePoint(loc_node);
				}
				else {
					imshow("", getimgLoc());
					if (waitKey(0) == 27) {
						prenode_name = loc_node;
						prenode = getnodePoint(loc_node);
						if (!loc_node.rfind("원", 0)) {
							string floor = "7-" + loc_node.substr(7, 1) + ".png";
							loc_img = imread(floor);
						}
						else if (!loc_node.rfind("신", 0)) {
							string floor = "32-" + loc_node.substr(9, 1) + ".png";
							loc_img = imread(floor);
						}
					}

				}
			}
		}
	}
};

vector<pair<int, float>> adj[MAX]; //인접리스트. 이중배열. 인덱스가 시작점의 인덱스. <도착점 인덱스, 가중치>
vector<string> nodeInfo; //인덱스에 따른 노드의 이름 저장
vector<int> before; //이전에 방문한 노드의 인덱스가 저장


vector<float> dijkstra(int startIdx, int V) {
	vector<float> dist(V, INF);    // 전부 INF로 초기화. 크기는 정점의 수 V. 시작점에 대한 모든 정점의 최단 거리 저장 
	priority_queue<pair<float, int> > pq; //<목표 정점까지의 dist 값, 정점의 index값>

	dist[startIdx] = 0;
	pq.push(make_pair(0, startIdx));    // 시작 정점 방문 
	before[startIdx] = startIdx;

	while (!pq.empty()) {
		float cost = -pq.top().first;    // 방문한 정점의 dist 값 
		int cur = pq.top().second;    // 현재 방문한 정점의 인덱스
		pq.pop();

		for (int i = 0; i < adj[cur].size(); i++) {    // 현재 방문한 정점의 주변 정점 모두 조사. size는 cur정점과 이어진 가중치 수
			int next = adj[cur][i].first;    // 조사할 다음 정점. 시작점이 cur이고 i(가중치 인덱스)의 first(도착정점인데스)
			float nCost = cost + adj[cur][i].second;    // 현재 방문한 정점을 거쳐서 다음 정점을 갈때의 비용 
			if (nCost < dist[next]) {     // 기존 비용보다 현재 방문한 정점을 거친 비용이 더 싸다면 
				dist[next] = nCost;    // 갱신 
				before[next] = cur;    //next로 가는 최단경로는 직전에 cur을 거침.
				pq.push(make_pair(-nCost, next));    // pq에 저장 
			}
		}
	}
	return dist;
}

template<class iter, class T>
iter myfind(iter first, iter last, const T& value) {
	for (; first != last; ++first) {
		if (*first == value)
			return first;
	}
	return last;
}

int main(int ac, char** av) {
	fstream fs;
	string from, to;
	float cost;
	int fromIndex, toIndex;
	int index = 0;

	// -- 이미지 출력 --
	img testImg(300);

	// ---- 이미지 노드 데이터 파일 읽기 ----
	fstream fsi;
	string buf;
	string v; float x, y;
	fsi.open("img_node.csv", ios::in);
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

	//간선 데이터 파일 읽기
	fs.open("edges.csv", ios::in);
	while (!fs.eof()) {

		//, 단위로 읽어서 buffer에 저장 (시작 노드 이름, 도착 노드 이름, 거리) 반복
		getline(fs, from, ','); //시작 노드 이름
		getline(fs, to, ','); //도착 노드 이름
		fs >> cost; //둘 사이의 가중치

		auto it = myfind(nodeInfo.begin(), nodeInfo.end(), from); //nodeInfo에서 from 노드를 찾음
		if (it == nodeInfo.end()) { //찾지못함. 새로운 노드
			fromIndex = index;
			nodeInfo.push_back(from);
			index++;
		}
		else { //찾았음. 이미 있는 인덱스값을 찾음
			fromIndex = it - nodeInfo.begin();
		}

		it = myfind(nodeInfo.begin(), nodeInfo.end(), to); //nodeInfo에서 to 노드를 찾음
		if (it == nodeInfo.end()) { //찾지못함. 새로운 버텍스
			toIndex = index;
			nodeInfo.push_back(to);
			index++;
		}
		else { //찾았음. 이미 있는 인덱스값을 찾아 가중치 추가
			toIndex = it - nodeInfo.begin();
		}
		adj[fromIndex].push_back(make_pair(toIndex, cost)); //양방향 그래프
		adj[toIndex].push_back(make_pair(fromIndex, cost));

		fs.ignore(); //'\n' 무시
	}
	fs.close();

	before.assign(index, -1); //before의 크기 노드의 수만큼 지정, -1로 초기화

	//사용자에게 출발, 도착 지점 입력받기
	string start;
	string goal;

	cout << "출발 강의실을 입력하세요 (ex 원흥관 314): "; 
	getline(cin, start);
	auto it = myfind(nodeInfo.begin(), nodeInfo.end(), start);
	if (it == nodeInfo.end()) {
		cout << "데이터에 존재하지 않는 강의실입니다." << endl;
		return 0;
	}
	fromIndex = it - nodeInfo.begin(); //시작 노드 인덱스

	vector<float> dist = dijkstra(fromIndex, index); //시작점에서 모든 정점으로의 최단경로 계산

	cout << "도착 강의실을 입력하세요 (ex 신공학관 6119): ";
	getline(cin, goal);
	it = myfind(nodeInfo.begin(), nodeInfo.end(), goal);
	if (it == nodeInfo.end()) {
		cout << "데이터에 존재하지 않는 강의실입니다." << endl;
		return 0;
	}
	toIndex = it - nodeInfo.begin(); // 도착 노드 인덱스

	vector<string> route;
	route.push_back(nodeInfo[toIndex]);
	while (toIndex != fromIndex) { //목표지점을 시작으로 경로 역추적하여 route에 저장
		string name = nodeInfo[before[toIndex]];
		route.push_back(name);
		toIndex = before[toIndex];
	}

	//route를 거꾸로 출력하여 최단 경로 텍스트로 출력
	cout << "\n***START!!***" << endl << route[route.size() - 1] << endl; //시작노드 출력
	for (int i = route.size() - 2; i >= 0; i--) {
		cout << " ->" << route[i] << endl;
	}
	cout << " ***GOAL!!***" << endl;

	// -- 이미지 경로 생성 --
	for (int i = route.size() - 1; i >= 0; i--) {
		testImg.print_img(route[i]);
	}
	imshow("", testImg.getimgLoc());
	if (waitKey(0) == 27)
		return 0;
}