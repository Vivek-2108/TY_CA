#include <bits/stdc++.h>
using namespace std;

struct Point {
    double x, y;
    int cluster;
};

double distance(Point a, Point b) {
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

int main() {
    int K = 2; 
    vector<Point> points = {{1,1},{2,1},{4,3},{5,4},{8,8},{9,9}};
    vector<Point> centroids(points.begin(), points.begin() + K);

    // One iteration
    for(auto &p : points) {
        double minDist = 1e9;
        int clusterId = 0;
        for(int i=0; i<K; i++) {
            double d = distance(p, centroids[i]);
            if(d < minDist) {
                minDist = d;
                clusterId = i;
            }
        }
        p.cluster = clusterId;
    }
    
    vector<double> sumX(K, 0), sumY(K, 0);
    vector<int> count(K, 0);
    for(auto &p : points) {
        sumX[p.cluster] += p.x;
        sumY[p.cluster] += p.y;
        count[p.cluster]++;
    }
    for(int i=0; i<K; i++) {
        if(count[i] > 0) {
            centroids[i].x = sumX[i] / count[i];
            centroids[i].y = sumY[i] / count[i];
        }
    }

    // Save to file for gnuplot
    ofstream fout("clusters.csv");
    for(auto &p : points) {
        fout << p.x << " " << p.y << " " << p.cluster << "\n";
    }
    fout.close();

    return 0;
}
