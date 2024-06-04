#pragma once
#include <vector>
#include "Vec2.h"
#include <queue>
#include "NeuralWarfareEngine.h"
#include <functional>
/// <summary>
/// K-D tree class for efficiently finding neighbor
/// </summary>
/// <remarks>
/// This is a new one for me so i was looking forward to this
/// </remarks>
class KDTree {
public:
    struct CompareDist
    {
        bool operator()(const std::pair<double, NeuralWarfareEngine::Agent*>& a, const std::pair<double, NeuralWarfareEngine::Agent*>& b) const {
            return a.first < b.first;
        }
    };
    using MaxHeap = std::priority_queue<std::pair<double, NeuralWarfareEngine::Agent*>, std::vector<std::pair<double, NeuralWarfareEngine::Agent*>>, CompareDist>;
    using Condition = std::function<bool(const NeuralWarfareEngine::Agent&)>;

    KDTree(const std::vector<NeuralWarfareEngine::Agent*>& points) {
        root = Build(points, 0);
    }
    ~KDTree() {
        Clear(root);
    }
    std::vector<NeuralWarfareEngine::Agent*> FindNearestNeighbors(const Vec2& query, size_t k, Condition condition = [](const NeuralWarfareEngine::Agent& a) { return true; });



private:
    struct KDNode {
        NeuralWarfareEngine::Agent* point;
        KDNode* left;
        KDNode* right;

        KDNode(NeuralWarfareEngine::Agent*& pt) : point(pt), left(nullptr), right(nullptr) {}
    };
    KDNode* root;

    KDNode* Build(std::vector<NeuralWarfareEngine::Agent*> points, int depth);

    void FindNearest(KDNode* node, const Vec2& query, size_t k, int depth, MaxHeap& maxHeap, Condition condition);

    void Clear(KDNode* node) {
        if (node) {
            Clear(node->left);
            Clear(node->right);
            delete node;
        }
    }
};
