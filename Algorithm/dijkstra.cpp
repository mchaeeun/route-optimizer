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

#define MAX 600 //�ִ� ������ ����
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
	string prenode_name = ""; Point prenode; // ���� ��� ����
	Mat loc_img; // ���� ���� �̹���

public:
	img(int ignored) { wonarr = new img_node[300]; sinarr = new img_node[300]; };
	~img() {};
	void insertLoc(string n, float x, float y) { // �̹��� �� ��ġ�� ����
		if (!n.rfind("��", 0)) {
			img_node* tmp = new img_node(n, x, y);
			wonarr[wonarr_size++] = *tmp;
		}
		else if (!n.rfind("��", 0)) {
			img_node* tmp = new img_node(n, x, y);
			sinarr[sinarr_size++] = *tmp;
		}

	}
	Mat getimgLoc() {
		return loc_img;
	}
	Point getnodePoint(string n) {
		if (!n.rfind("��", 0)) {
			for (int i = 0; i < wonarr_size; i++) {
				if (wonarr[i].getname() == n)
					return Point(wonarr[i].getx(), wonarr[i].gety());
			}
		}
		else if (!n.rfind("��", 0)) {
			for (int i = 0; i < sinarr_size; i++) {
				if (sinarr[i].getname() == n)
					return Point(sinarr[i].getx(), sinarr[i].gety());
			}
		}
	}

	void print_img(string loc_node) {
		if (!prenode_name.compare("")) { // �̹��� ��� ù ��°
			prenode_name = loc_node;
			prenode = getnodePoint(prenode_name);
			if (!loc_node.rfind("��", 0)) {
				string floor = "7-" + loc_node.substr(7, 1) + ".png";
				loc_img = imread(floor);
			}
			else if (!loc_node.rfind("��", 0)) {
				string floor = "32-" + loc_node.substr(9, 1) + ".png";
				loc_img = imread(floor);
			}
		}
		else {
			if (!loc_node.rfind("��", 0)) {
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
						if (!loc_node.rfind("��", 0)) {
							string floor = "7-" + loc_node.substr(7, 1) + ".png";
							loc_img = imread(floor);
						}
						else if (!loc_node.rfind("��", 0)) {
							string floor = "32-" + loc_node.substr(9, 1) + ".png";
							loc_img = imread(floor);
						}
					}
				}
			}
			else if (!loc_node.rfind("��", 0)) {
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
						if (!loc_node.rfind("��", 0)) {
							string floor = "7-" + loc_node.substr(7, 1) + ".png";
							loc_img = imread(floor);
						}
						else if (!loc_node.rfind("��", 0)) {
							string floor = "32-" + loc_node.substr(9, 1) + ".png";
							loc_img = imread(floor);
						}
					}

				}
			}
		}
	}
};

vector<pair<int, float>> adj[MAX]; //��������Ʈ. ���߹迭. �ε����� �������� �ε���. <������ �ε���, ����ġ>
vector<string> nodeInfo; //�ε����� ���� ����� �̸� ����
vector<int> before; //������ �湮�� ����� �ε����� ����


vector<float> dijkstra(int startIdx, int V) {
	vector<float> dist(V, INF);    // ���� INF�� �ʱ�ȭ. ũ��� ������ �� V. �������� ���� ��� ������ �ִ� �Ÿ� ���� 
	priority_queue<pair<float, int> > pq; //<��ǥ ���������� dist ��, ������ index��>

	dist[startIdx] = 0;
	pq.push(make_pair(0, startIdx));    // ���� ���� �湮 
	before[startIdx] = startIdx;

	while (!pq.empty()) {
		float cost = -pq.top().first;    // �湮�� ������ dist �� 
		int cur = pq.top().second;    // ���� �湮�� ������ �ε���
		pq.pop();

		for (int i = 0; i < adj[cur].size(); i++) {    // ���� �湮�� ������ �ֺ� ���� ��� ����. size�� cur������ �̾��� ����ġ ��
			int next = adj[cur][i].first;    // ������ ���� ����. �������� cur�̰� i(����ġ �ε���)�� first(���������ε���)
			float nCost = cost + adj[cur][i].second;    // ���� �湮�� ������ ���ļ� ���� ������ ������ ��� 
			if (nCost < dist[next]) {     // ���� ��뺸�� ���� �湮�� ������ ��ģ ����� �� �δٸ� 
				dist[next] = nCost;    // ���� 
				before[next] = cur;    //next�� ���� �ִܰ�δ� ������ cur�� ��ħ.
				pq.push(make_pair(-nCost, next));    // pq�� ���� 
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

	// -- �̹��� ��� --
	img testImg(300);

	// ---- �̹��� ��� ������ ���� �б� ----
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

	//���� ������ ���� �б�
	fs.open("edges.csv", ios::in);
	while (!fs.eof()) {

		//, ������ �о buffer�� ���� (���� ��� �̸�, ���� ��� �̸�, �Ÿ�) �ݺ�
		getline(fs, from, ','); //���� ��� �̸�
		getline(fs, to, ','); //���� ��� �̸�
		fs >> cost; //�� ������ ����ġ

		auto it = myfind(nodeInfo.begin(), nodeInfo.end(), from); //nodeInfo���� from ��带 ã��
		if (it == nodeInfo.end()) { //ã������. ���ο� ���
			fromIndex = index;
			nodeInfo.push_back(from);
			index++;
		}
		else { //ã����. �̹� �ִ� �ε������� ã��
			fromIndex = it - nodeInfo.begin();
		}

		it = myfind(nodeInfo.begin(), nodeInfo.end(), to); //nodeInfo���� to ��带 ã��
		if (it == nodeInfo.end()) { //ã������. ���ο� ���ؽ�
			toIndex = index;
			nodeInfo.push_back(to);
			index++;
		}
		else { //ã����. �̹� �ִ� �ε������� ã�� ����ġ �߰�
			toIndex = it - nodeInfo.begin();
		}
		adj[fromIndex].push_back(make_pair(toIndex, cost)); //����� �׷���
		adj[toIndex].push_back(make_pair(fromIndex, cost));

		fs.ignore(); //'\n' ����
	}
	fs.close();

	before.assign(index, -1); //before�� ũ�� ����� ����ŭ ����, -1�� �ʱ�ȭ

	//����ڿ��� ���, ���� ���� �Է¹ޱ�
	string start;
	string goal;

	cout << "��� ���ǽ��� �Է��ϼ��� (ex ����� 314): "; 
	getline(cin, start);
	auto it = myfind(nodeInfo.begin(), nodeInfo.end(), start);
	if (it == nodeInfo.end()) {
		cout << "�����Ϳ� �������� �ʴ� ���ǽ��Դϴ�." << endl;
		return 0;
	}
	fromIndex = it - nodeInfo.begin(); //���� ��� �ε���

	vector<float> dist = dijkstra(fromIndex, index); //���������� ��� ���������� �ִܰ�� ���

	cout << "���� ���ǽ��� �Է��ϼ��� (ex �Ű��а� 6119): ";
	getline(cin, goal);
	it = myfind(nodeInfo.begin(), nodeInfo.end(), goal);
	if (it == nodeInfo.end()) {
		cout << "�����Ϳ� �������� �ʴ� ���ǽ��Դϴ�." << endl;
		return 0;
	}
	toIndex = it - nodeInfo.begin(); // ���� ��� �ε���

	vector<string> route;
	route.push_back(nodeInfo[toIndex]);
	while (toIndex != fromIndex) { //��ǥ������ �������� ��� �������Ͽ� route�� ����
		string name = nodeInfo[before[toIndex]];
		route.push_back(name);
		toIndex = before[toIndex];
	}

	//route�� �Ųٷ� ����Ͽ� �ִ� ��� �ؽ�Ʈ�� ���
	cout << "\n***START!!***" << endl << route[route.size() - 1] << endl; //���۳�� ���
	for (int i = route.size() - 2; i >= 0; i--) {
		cout << " ->" << route[i] << endl;
	}
	cout << " ***GOAL!!***" << endl;

	// -- �̹��� ��� ���� --
	for (int i = route.size() - 1; i >= 0; i--) {
		testImg.print_img(route[i]);
	}
	imshow("", testImg.getimgLoc());
	if (waitKey(0) == 27)
		return 0;
}