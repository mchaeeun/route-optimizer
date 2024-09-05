#include <limits>    // For INT_MAX
#include <iostream>
#include <cstring>
#include <fstream>
#include <string>

using namespace std;

// -- for image --
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

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

int main() {

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

    // -- 이미지 경로 생성 --
    for (int i = 0; 배열 사이즈만큼; i++) {
        testImg.print_img(배열(i));
    }

    imshow("", testImg.getimgLoc());
    if (waitKey(0) == 27)
        return 0;
}