#include "KDTree.h"

KDTree::KDNode* KDTree::Build(std::vector<NeuralWarfareEngine::Agent*> points, int depth) {
    if (points.empty()) return nullptr;

    size_t axis = depth % 2;
    size_t median = points.size() / 2;

    auto compare = [axis](NeuralWarfareEngine::Agent*& a, NeuralWarfareEngine::Agent*& b) {
        return axis == 0 ? a->pos.x < b->pos.x : a->pos.y < b->pos.y;
        };

    std::nth_element(points.begin(), points.begin() + median, points.end(), compare);

    KDNode* node = new KDNode(points[median]);
    std::vector<NeuralWarfareEngine::Agent*> leftPoints(points.begin(), points.begin() + median);
    std::vector<NeuralWarfareEngine::Agent*> rightPoints(points.begin() + median + 1, points.end());

    node->left = Build(leftPoints, depth + 1);
    node->right = Build(rightPoints, depth + 1);

    return node;
}

void KDTree::FindNearest(KDNode* node, const Vec2& query, size_t k, int depth, MaxHeap& maxHeap) {
    if (!node) return;

    double dist = Vec2(node->point->pos.x - query.x, node->point->pos.y - query.y).Length();
    if (maxHeap.size() < k) {
        maxHeap.emplace(dist, node->point); // Add point if less than k elements
    }
    else if (dist < maxHeap.top().first) {
        maxHeap.pop(); // Remove the farthest point
        maxHeap.emplace(dist, node->point); // Add the new closer point
    }

    size_t axis = depth % 2;
    KDNode* nextBranch = (axis == 0 ? query.x < node->point->pos.x : query.y < node->point->pos.y) ? node->left : node->right;
    KDNode* otherBranch = (axis == 0 ? query.x < node->point->pos.x : query.y < node->point->pos.y) ? node->right : node->left;

    FindNearest(nextBranch, query, k, depth + 1, maxHeap);

    double axisDist = axis == 0 ? std::abs(query.x - node->point->pos.x) : std::abs(query.y - node->point->pos.y);
    if (maxHeap.size() < k || axisDist < maxHeap.top().first) {
        FindNearest(otherBranch, query, k, depth + 1, maxHeap);
    }
}

std::vector<NeuralWarfareEngine::Agent*> KDTree::FindNearestNeighbors(const Vec2& query, size_t k) {
    MaxHeap maxHeap;
    FindNearest(root, query, k, 0, maxHeap);

    std::vector<NeuralWarfareEngine::Agent*> neighbors;
    while (!maxHeap.empty()) {
        neighbors.push_back(maxHeap.top().second);
        maxHeap.pop();
    }

    std::reverse(neighbors.begin(), neighbors.end());
    return neighbors;
}
