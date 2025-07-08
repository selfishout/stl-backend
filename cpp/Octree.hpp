#pragma once

#include "Point.hpp"

#include <functional>
#include <memory>
#include <array>
#include <vector>
#include <algorithm>
#include <limits>

namespace MF
{
    // Octree template class
    // ObjectType must have "getBounds" and "center" methods, see e.g., Point.hpp and Triangle.hpp
    template<typename ObjectType, typename CoordType = float>
    class Octree
    {
    private:
        static constexpr int MAX_DEPTH = 8;
        static constexpr int MAX_OBJECTS_PER_NODE = 10;

        struct Node
        {
            AABB<CoordType> bounds;
            std::vector<ObjectType> objects;
            std::array<std::unique_ptr<Node>, 8> children;
            int depth;

            Node(const AABB<CoordType>& bounds, int depth) : bounds(bounds), depth(depth) {}

            bool isLeaf() const {
                return children[0] == nullptr;
            }

            void subdivide() {
                if (!isLeaf()) return;

                Vec3<CoordType> center = bounds.center();
                Vec3<CoordType> size = bounds.size() * CoordType(0.5);

                // Create 8 child octants
                children[0] = std::make_unique<Node>(AABB<CoordType>(bounds.min, center), depth + 1);
                children[1] = std::make_unique<Node>(AABB<CoordType>(Vec3<CoordType>(center.x, bounds.min.y, bounds.min.z),
                    Vec3<CoordType>(bounds.max.x, center.y, center.z)), depth + 1);
                children[2] = std::make_unique<Node>(AABB<CoordType>(Vec3<CoordType>(bounds.min.x, center.y, bounds.min.z),
                    Vec3<CoordType>(center.x, bounds.max.y, center.z)), depth + 1);
                children[3] = std::make_unique<Node>(AABB<CoordType>(Vec3<CoordType>(center.x, center.y, bounds.min.z),
                    Vec3<CoordType>(bounds.max.x, bounds.max.y, center.z)), depth + 1);
                children[4] = std::make_unique<Node>(AABB<CoordType>(Vec3<CoordType>(bounds.min.x, bounds.min.y, center.z),
                    Vec3<CoordType>(center.x, center.y, bounds.max.z)), depth + 1);
                children[5] = std::make_unique<Node>(AABB<CoordType>(Vec3<CoordType>(center.x, bounds.min.y, center.z),
                    Vec3<CoordType>(bounds.max.x, center.y, bounds.max.z)), depth + 1);
                children[6] = std::make_unique<Node>(AABB<CoordType>(Vec3<CoordType>(bounds.min.x, center.y, center.z),
                    Vec3<CoordType>(center.x, bounds.max.y, bounds.max.z)), depth + 1);
                children[7] = std::make_unique<Node>(AABB<CoordType>(center, bounds.max), depth + 1);
            }
        };

        std::unique_ptr<Node> root;

        void insertRecursive(Node* node, const ObjectType& object);

        void queryRecursive(Node* node, const AABB<CoordType>& queryBounds, std::vector<ObjectType>& results) const;

        void queryRadiusRecursive(Node* node, const Vec3<CoordType>& center, CoordType radius, std::vector<ObjectType>& results) const;

        // Helper function to get minimum distance from point to AABB
        CoordType getMinDistanceToAABB(const Vec3<CoordType>& point, const AABB<CoordType>& bounds) const;

        void nearestNeighborRecursive(Node* node, const Vec3<CoordType>& queryPoint, ObjectType& bestObject, CoordType& bestDistanceSq, bool& found) const;

        void kNearestNeighborsRecursive(Node* node, const Vec3<CoordType>& queryPoint, std::vector<std::pair<CoordType, ObjectType>>& candidates, int k) const;

        void collectAllRecursive(Node* node, std::vector<ObjectType>& results) const;

        int getDepthRecursive(Node* node) const;

        int getNodeCountRecursive(Node* node) const;

    public:
        Octree(const AABB<CoordType>& bounds);

        void insert(const ObjectType& object);

        std::vector<ObjectType> query(const AABB<CoordType>& bounds) const;

        std::vector<ObjectType> queryRadius(const Vec3<CoordType>& center, CoordType radius) const;

        // Find the single nearest neighbor to a query point
        std::pair<bool, ObjectType> nearestNeighbor(const Vec3<CoordType>& queryPoint) const;

        // Find the k nearest neighbors to a query point
        std::vector<std::pair<CoordType, ObjectType>> kNearestNeighbors(const Vec3<CoordType>& queryPoint, int k) const;

        // Find nearest neighbor within a maximum distance
        std::pair<bool, ObjectType> nearestNeighborWithinRadius(const Vec3<CoordType>& queryPoint, CoordType maxRadius) const;

        std::vector<ObjectType> queryPoint(const Vec3<CoordType>& point) const;

        std::vector<ObjectType> getAll() const;

        void clear();

        AABB<CoordType> getBounds() const;

        int getMaxDepth() const;

        int getNodeCount() const;

        // Traverse the octree and apply a function to each node
        void traverse(std::function<void(const AABB<CoordType>&, int, bool, const std::vector<ObjectType>&)> visitor) const;

    private:
        void traverseRecursive(Node* node, std::function<void(const AABB<CoordType>&, int, bool, const std::vector<ObjectType>&)>& visitor) const;
    };

} // end namespace MF

// Template method implementations (outside namespace)

template<typename ObjectType, typename CoordType>
void MF::Octree<ObjectType, CoordType>::insertRecursive(Node* node, const ObjectType& object)
{
    if (!node->bounds.intersects(object.getBounds())) {
        return;
    }
    if (node->isLeaf()) {
        node->objects.push_back(object);
        if (node->objects.size() > MAX_OBJECTS_PER_NODE && node->depth < MAX_DEPTH) {
            node->subdivide();
            auto objects = std::move(node->objects);
            node->objects.clear();
            for (const auto& obj : objects) {
                for (auto& child : node->children) {
                    insertRecursive(child.get(), obj);
                }
            }
        }
    } else {
        for (auto& child : node->children) {
            insertRecursive(child.get(), object);
        }
    }
}

template<typename ObjectType, typename CoordType>
void MF::Octree<ObjectType, CoordType>::queryRecursive(Node* node, const AABB<CoordType>& queryBounds, std::vector<ObjectType>& results) const
{
    if (!node || !node->bounds.intersects(queryBounds)) {
        return;
    }
    if (node->isLeaf()) {
        for (const auto& obj : node->objects) {
            if (queryBounds.intersects(obj.getBounds())) {
                results.push_back(obj);
            }
        }
    } else {
        for (auto& child : node->children) {
            queryRecursive(child.get(), queryBounds, results);
        }
    }
}

template<typename ObjectType, typename CoordType>
void MF::Octree<ObjectType, CoordType>::queryRadiusRecursive(Node* node, const Vec3<CoordType>& center, CoordType radius, std::vector<ObjectType>& results) const
{
    if (!node) return;
    Vec3<CoordType> closest = Vec3<CoordType>(
        std::max(node->bounds.min.x, std::min(center.x, node->bounds.max.x)),
        std::max(node->bounds.min.y, std::min(center.y, node->bounds.max.y)),
        std::max(node->bounds.min.z, std::min(center.z, node->bounds.max.z))
    );
    CoordType distSq = (closest - center).dot(closest - center);
    if (distSq > radius * radius) {
        return;
    }
    if (node->isLeaf()) {
        for (const auto& obj : node->objects) {
            Vec3<CoordType> objCenter = obj.center();
            CoordType objDistSq = (objCenter - center).dot(objCenter - center);
            if (objDistSq <= radius * radius) {
                results.push_back(obj);
            }
        }
    } else {
        for (auto& child : node->children) {
            queryRadiusRecursive(child.get(), center, radius, results);
        }
    }
}

template<typename ObjectType, typename CoordType>
CoordType MF::Octree<ObjectType, CoordType>::getMinDistanceToAABB(const Vec3<CoordType>& point, const AABB<CoordType>& bounds) const
{
    Vec3<CoordType> closest = Vec3<CoordType>(
        std::max(bounds.min.x, std::min(point.x, bounds.max.x)),
        std::max(bounds.min.y, std::min(point.y, bounds.max.y)),
        std::max(bounds.min.z, std::min(point.z, bounds.max.z))
    );
    return (closest - point).length();
}

template<typename ObjectType, typename CoordType>
void MF::Octree<ObjectType, CoordType>::nearestNeighborRecursive(Node* node, const Vec3<CoordType>& queryPoint, ObjectType& bestObject, CoordType& bestDistanceSq, bool& found) const
{
    if (!node) return;
    
    // Check if this node could contain a better candidate
    CoordType minDistToNode = getMinDistanceToAABB(queryPoint, node->bounds);
    if (minDistToNode >= bestDistanceSq) {
        return;
    }
    
    if (node->isLeaf()) {
        for (const auto& obj : node->objects) {
            Vec3<CoordType> objCenter = obj.center();
            CoordType distSq = (objCenter - queryPoint).dot(objCenter - queryPoint);
            if (distSq < bestDistanceSq) {
                bestDistanceSq = distSq;
                bestObject = obj;
                found = true;
            }
        }
    } else {
        // Sort children by distance to query point for better pruning
        std::vector<std::pair<CoordType, Node*>> childDistances;
        for (auto& child : node->children) {
            if (child) {
                CoordType dist = getMinDistanceToAABB(queryPoint, child->bounds);
                childDistances.push_back({dist, child.get()});
            }
        }
        std::sort(childDistances.begin(), childDistances.end());
        
        for (auto& [dist, child] : childDistances) {
            if (dist < bestDistanceSq) {
                nearestNeighborRecursive(child, queryPoint, bestObject, bestDistanceSq, found);
            }
        }
    }
}

template<typename ObjectType, typename CoordType>
void MF::Octree<ObjectType, CoordType>::kNearestNeighborsRecursive(Node* node, const Vec3<CoordType>& queryPoint, std::vector<std::pair<CoordType, ObjectType>>& candidates, int k) const
{
    if (!node) return;
    
    CoordType minDistToNode = getMinDistanceToAABB(queryPoint, node->bounds);
    if (!candidates.empty() && minDistToNode >= candidates.back().first) {
        return;
    }
    
    if (node->isLeaf()) {
        for (const auto& obj : node->objects) {
            Vec3<CoordType> objCenter = obj.center();
            CoordType distSq = (objCenter - queryPoint).dot(objCenter - queryPoint);
            
            if (candidates.size() < k || distSq < candidates.back().first) {
                candidates.push_back({distSq, obj});
                std::sort(candidates.begin(), candidates.end());
                if (candidates.size() > k) {
                    candidates.resize(k);
                }
            }
        }
    } else {
        std::vector<std::pair<CoordType, Node*>> childDistances;
        for (auto& child : node->children) {
            if (child) {
                CoordType dist = getMinDistanceToAABB(queryPoint, child->bounds);
                childDistances.push_back({dist, child.get()});
            }
        }
        std::sort(childDistances.begin(), childDistances.end());
        
        for (auto& [dist, child] : childDistances) {
            if (candidates.empty() || dist < candidates.back().first) {
                kNearestNeighborsRecursive(child, queryPoint, candidates, k);
            }
        }
    }
}

template<typename ObjectType, typename CoordType>
void MF::Octree<ObjectType, CoordType>::collectAllRecursive(Node* node, std::vector<ObjectType>& results) const
{
    if (!node) return;
    
    if (node->isLeaf()) {
        results.insert(results.end(), node->objects.begin(), node->objects.end());
    } else {
        for (auto& child : node->children) {
            collectAllRecursive(child.get(), results);
        }
    }
}

template<typename ObjectType, typename CoordType>
int MF::Octree<ObjectType, CoordType>::getDepthRecursive(Node* node) const
{
    if (!node) return 0;
    
    if (node->isLeaf()) {
        return node->depth;
    }
    
    int maxChildDepth = 0;
    for (auto& child : node->children) {
        maxChildDepth = std::max(maxChildDepth, getDepthRecursive(child.get()));
    }
    return maxChildDepth;
}

template<typename ObjectType, typename CoordType>
int MF::Octree<ObjectType, CoordType>::getNodeCountRecursive(Node* node) const
{
    if (!node) return 0;
    
    int count = 1;
    for (auto& child : node->children) {
        count += getNodeCountRecursive(child.get());
    }
    return count;
}

template<typename ObjectType, typename CoordType>
MF::Octree<ObjectType, CoordType>::Octree(const AABB<CoordType>& bounds)
    : root(std::make_unique<Node>(bounds, 0))
{
}

template<typename ObjectType, typename CoordType>
void MF::Octree<ObjectType, CoordType>::insert(const ObjectType& object)
{
    insertRecursive(root.get(), object);
}

template<typename ObjectType, typename CoordType>
std::vector<ObjectType> MF::Octree<ObjectType, CoordType>::query(const AABB<CoordType>& bounds) const
{
    std::vector<ObjectType> results;
    queryRecursive(root.get(), bounds, results);
    return results;
}

template<typename ObjectType, typename CoordType>
std::vector<ObjectType> MF::Octree<ObjectType, CoordType>::queryRadius(const Vec3<CoordType>& center, CoordType radius) const
{
    std::vector<ObjectType> results;
    queryRadiusRecursive(root.get(), center, radius, results);
    return results;
}

template<typename ObjectType, typename CoordType>
std::pair<bool, ObjectType> MF::Octree<ObjectType, CoordType>::nearestNeighbor(const Vec3<CoordType>& queryPoint) const
{
    ObjectType bestObject;
    CoordType bestDistanceSq = std::numeric_limits<CoordType>::max();
    bool found = false;
    
    nearestNeighborRecursive(root.get(), queryPoint, bestObject, bestDistanceSq, found);
    
    return {found, bestObject};
}

template<typename ObjectType, typename CoordType>
std::vector<std::pair<CoordType, ObjectType>> MF::Octree<ObjectType, CoordType>::kNearestNeighbors(const Vec3<CoordType>& queryPoint, int k) const
{
    std::vector<std::pair<CoordType, ObjectType>> candidates;
    kNearestNeighborsRecursive(root.get(), queryPoint, candidates, k);
    return candidates;
}

template<typename ObjectType, typename CoordType>
std::pair<bool, ObjectType> MF::Octree<ObjectType, CoordType>::nearestNeighborWithinRadius(const Vec3<CoordType>& queryPoint, CoordType maxRadius) const
{
    auto result = nearestNeighbor(queryPoint);
    if (result.first) {
        Vec3<CoordType> objCenter = result.second.center();
        CoordType distSq = (objCenter - queryPoint).dot(objCenter - queryPoint);
        if (distSq <= maxRadius * maxRadius) {
            return result;
        }
    }
    return {false, ObjectType{}};
}

template<typename ObjectType, typename CoordType>
std::vector<ObjectType> MF::Octree<ObjectType, CoordType>::queryPoint(const Vec3<CoordType>& point) const
{
    AABB<CoordType> pointBounds(point, point);
    return query(pointBounds);
}

template<typename ObjectType, typename CoordType>
std::vector<ObjectType> MF::Octree<ObjectType, CoordType>::getAll() const
{
    std::vector<ObjectType> results;
    collectAllRecursive(root.get(), results);
    return results;
}

template<typename ObjectType, typename CoordType>
void MF::Octree<ObjectType, CoordType>::clear()
{
    root = std::make_unique<Node>(root->bounds, 0);
}

template<typename ObjectType, typename CoordType>
MF::AABB<CoordType> MF::Octree<ObjectType, CoordType>::getBounds() const
{
    return root ? root->bounds : MF::AABB<CoordType>{};
}

template<typename ObjectType, typename CoordType>
int MF::Octree<ObjectType, CoordType>::getMaxDepth() const
{
    return getDepthRecursive(root.get());
}

template<typename ObjectType, typename CoordType>
int MF::Octree<ObjectType, CoordType>::getNodeCount() const
{
    return getNodeCountRecursive(root.get());
}

template<typename ObjectType, typename CoordType>
void MF::Octree<ObjectType, CoordType>::traverse(std::function<void(const AABB<CoordType>&, int, bool, const std::vector<ObjectType>&)> visitor) const
{
    if (root) {
        traverseRecursive(root.get(), visitor);
    }
}

template<typename ObjectType, typename CoordType>
void MF::Octree<ObjectType, CoordType>::traverseRecursive(Node* node, std::function<void(const AABB<CoordType>&, int, bool, const std::vector<ObjectType>&)>& visitor) const
{
    if (!node) return;
    
    visitor(node->bounds, node->depth, node->isLeaf(), node->objects);
    
    if (!node->isLeaf()) {
        for (auto& child : node->children) {
            traverseRecursive(child.get(), visitor);
        }
    }
} 