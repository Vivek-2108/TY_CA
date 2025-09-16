#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include <bits/stdc++.h>
using namespace std;

struct VectorBlock {
    vector<int> values;
};

double distanceVec(const VectorBlock &a, const VectorBlock &b) {
    double d = 0;
    for (size_t i = 0; i < a.values.size(); i++) {
        double diff = a.values[i] - b.values[i];
        d += diff * diff;
    }
    return sqrt(d);
}

VectorBlock averageVector(const vector<VectorBlock> &cluster) {
    int dim = cluster[0].values.size();
    VectorBlock avg;
    avg.values.assign(dim, 0);
    for (auto &vec : cluster) {
        for (int i = 0; i < dim; i++)
            avg.values[i] += vec.values[i];
    }
    for (int i = 0; i < dim; i++)
        avg.values[i] /= cluster.size();
    return avg;
}

vector<VectorBlock> buildCodebook(vector<VectorBlock> &blocks, int codebookSize) {
    vector<VectorBlock> codebook;
    for (int i = 0; i < codebookSize; i++)
        codebook.push_back(blocks[rand() % blocks.size()]);

    bool changed = true;
    int maxIterations = 10;  // reduced for speed
    int iter = 0;

    while (changed && iter < maxIterations) {
        iter++;
        changed = false;
        vector<vector<VectorBlock>> clusters(codebookSize);

        for (auto &block : blocks) {
            int bestIdx = 0;
            double bestDist = 1e18;
            for (int i = 0; i < codebookSize; i++) {
                double d = distanceVec(block, codebook[i]);
                if (d < bestDist) {
                    bestDist = d;
                    bestIdx = i;
                }
            }
            clusters[bestIdx].push_back(block);
        }

        for (int i = 0; i < codebookSize; i++) {
            if (!clusters[i].empty()) {
                VectorBlock newVec = averageVector(clusters[i]);
                if (distanceVec(newVec, codebook[i]) > 1e-2) // relax
                    changed = true;
                codebook[i] = newVec;
            }
        }

        cout << "Iteration " << iter << " done\n";
    }
    return codebook;
}

int main() {
    srand(time(0));

    int width, height, channels;
    unsigned char *img = stbi_load("input.jpg", &width, &height, &channels, 3); // force RGB
    if (!img) {
        cerr << "Failed to load image\n";
        return -1;
    }
    cout << "Loaded image: " << width << "x" << height << " Channels=" << channels << "\n";

    // ✅ Resize big images to 512x512
    int targetSize = 512;
    int newW = targetSize, newH = targetSize;
    vector<unsigned char> resized(newW * newH * 3);
    for (int y = 0; y < newH; y++) {
        for (int x = 0; x < newW; x++) {
            int srcX = x * width / newW;
            int srcY = y * height / newH;
            for (int c = 0; c < 3; c++) {
                resized[(y * newW + x) * 3 + c] = img[(srcY * width + srcX) * 3 + c];
            }
        }
    }
    stbi_image_free(img);
    width = newW; height = newH;

    // Convert to grayscale
    vector<int> gray(width * height);
    for (int i = 0; i < width * height; i++) {
        int r = resized[i * 3 + 0];
        int g = resized[i * 3 + 1];
        int b = resized[i * 3 + 2];
        gray[i] = (r + g + b) / 3;
    }

    int blockSize = 8;        // bigger block → faster
    int codebookSize = 16;    // fewer clusters → faster
    vector<VectorBlock> blocks;
    for (int i = 0; i < height; i += blockSize) {
        for (int j = 0; j < width; j += blockSize) {
            VectorBlock b;
            for (int x = 0; x < blockSize; x++) {
                for (int y = 0; y < blockSize; y++) {
                    int row = i + x, col = j + y;
                    if (row < height && col < width)
                        b.values.push_back(gray[row * width + col]);
                    else
                        b.values.push_back(0);
                }
            }
            blocks.push_back(b);
        }
    }

    vector<VectorBlock> codebook = buildCodebook(blocks, codebookSize);

    vector<int> reconstructed(width * height, 0);
    int idx = 0;
    for (int i = 0; i < height; i += blockSize) {
        for (int j = 0; j < width; j += blockSize) {
            VectorBlock block = blocks[idx++];
            int bestIdx = 0;
            double bestDist = 1e18;
            for (int k = 0; k < codebookSize; k++) {
                double d = distanceVec(block, codebook[k]);
                if (d < bestDist) {
                    bestDist = d;
                    bestIdx = k;
                }
            }
            VectorBlock chosen = codebook[bestIdx];
            int p = 0;
            for (int x = 0; x < blockSize; x++) {
                for (int y = 0; y < blockSize; y++) {
                    int row = i + x, col = j + y;
                    if (row < height && col < width)
                        reconstructed[row * width + col] = chosen.values[p];
                    p++;
                }
            }
        }
    }

    vector<unsigned char> outImg(width * height * 3);
    for (int i = 0; i < width * height; i++) {
        outImg[i * 3 + 0] = reconstructed[i];
        outImg[i * 3 + 1] = reconstructed[i];
        outImg[i * 3 + 2] = reconstructed[i];
    }
    stbi_write_png("output.png", width, height, 3, outImg.data(), width * 3);

    cout << "Compressed image saved as output.png\n";
    return 0;
}
