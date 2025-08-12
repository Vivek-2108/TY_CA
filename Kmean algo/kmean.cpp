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
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int K = 2;  // number of clusters
    int N = 100000; // number of points to generate

    // Random number generator
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    uniform_real_distribution<double> dist(0.0, 1000.0);

    vector<Point> points;
    points.reserve(N);

    // Generate random points
    for(int i = 0; i < N; i++) {
        points.push_back({dist(rng), dist(rng), 0});
    }

    // Pick first K points as initial centroids
    vector<Point> centroids(points.begin(), points.begin() + K);

    // One iteration of K-means
    for(auto &p : points) {
        double minDist = 1e18;
        int clusterId = 0;
        for(int i = 0; i < K; i++) {
            double d = distance(p, centroids[i]);
            if(d < minDist) {
                minDist = d;
                clusterId = i;
            }
        }
        p.cluster = clusterId;
    }

    // Update centroids
    vector<double> sumX(K, 0), sumY(K, 0);
    vector<int> count(K, 0);
    for(auto &p : points) {
        sumX[p.cluster] += p.x;
        sumY[p.cluster] += p.y;
        count[p.cluster]++;
    }
    for(int i = 0; i < K; i++) {
        if(count[i] > 0) {
            centroids[i].x = sumX[i] / count[i];
            centroids[i].y = sumY[i] / count[i];
        }
    }

    // Save points to file for gnuplot
    ofstream fout("clusters.csv");
    for(auto &p : points) {
        fout << p.x << " " << p.y << " " << p.cluster << "\n";
    }
    fout.close();

    cerr << "Generated " << N << " points and saved to clusters.csv\n";
    return 0;
}
