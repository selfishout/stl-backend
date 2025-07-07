#pragma once

#include "Point.hpp"
#include <vector>
#include <memory>
#include <algorithm>

namespace MF
{
    template<typename T, typename U>
    class Octree
    {
    private:
        struct Node
        {
            AABB<U> bounds;
            std::vector<T> objects;
            std::array<std::unique_ptr<Node>, 8> children;
            bool isLeaf;
            
            Node(const AABB<U>& bounds) : bounds(bounds), isLeaf(true) {}
        };
        
        std::unique_ptr<Node> root;
        size_t maxObjectsPerNode;
        size_t maxDepth;
        
    public:
        Octree(const AABB<U>& bounds, size_t maxObjects = 8, size_t maxDepth = 8) 
            : maxObjectsPerNode(maxObjects), maxDepth(maxDepth)
        {
            root = std::make_unique<Node>(bounds);
        }
        
        void insert(const T& object, const AABB<U>& objectBounds)
        {
            insertRecursive(root.get(), object, objectBounds, 0);
        }
        
        std::vector<T> query(const Vec3<U>& point) const
        {
            std::vector<T> result;
            queryRecursive(root.get(), point, result);
            return result;
        }
        
        std::vector<T> query(const AABB<U>& bounds) const
        {
            std::vector<T> result;
            queryRecursive(root.get(), bounds, result);
            return result;
        }
        
        void clear()
        {
            root = std::make_unique<Node>(root->bounds);
        }
        
    private:
        void insertRecursive(Node* node, const T& object, const AABB<U>& objectBounds, size_t depth)
        {
            if (!node->bounds.contains(objectBounds.min) && !node->bounds.contains(objectBounds.max))
                return;
                
            if (node->isLeaf)
            {
                node->objects.push_back(object);
                
                if (node->objects.size() > maxObjectsPerNode && depth < maxDepth)
                {
                    subdivide(node);
                }
            }
            else
            {
                for (auto& child : node->children)
                {
                    if (child)
                        insertRecursive(child.get(), object, objectBounds, depth + 1);
                }
            }
        }
        
        void subdivide(Node* node)
        {
            Vec3<U> center = (node->bounds.min + node->bounds.max) * 0.5;
            Vec3<U> extents = (node->bounds.max - node->bounds.min) * 0.5;
            
            for (int i = 0; i < 8; ++i)
            {
                Vec3<U> childMin = node->bounds.min;
                Vec3<U> childMax = center;
                
                if (i & 1) childMin.x = center.x;
                else childMax.x = center.x;
                if (i & 2) childMin.y = center.y;
                else childMax.y = center.y;
                if (i & 4) childMin.z = center.z;
                else childMax.z = center.z;
                
                node->children[i] = std::make_unique<Node>(AABB<U>(childMin, childMax));
            }
            
            node->isLeaf = false;
            
            // Redistribute objects to children
            for (const auto& obj : node->objects)
            {
                for (auto& child : node->children)
                {
                    insertRecursive(child.get(), obj, node->bounds, 0);
                }
            }
            
            node->objects.clear();
        }
        
        void queryRecursive(const Node* node, const Vec3<U>& point, std::vector<T>& result) const
        {
            if (!node->bounds.contains(point))
                return;
                
            if (node->isLeaf)
            {
                result.insert(result.end(), node->objects.begin(), node->objects.end());
            }
            else
            {
                for (const auto& child : node->children)
                {
                    if (child)
                        queryRecursive(child.get(), point, result);
                }
            }
        }
        
        void queryRecursive(const Node* node, const AABB<U>& bounds, std::vector<T>& result) const
        {
            if (!node->bounds.contains(bounds.min) && !node->bounds.contains(bounds.max))
                return;
                
            if (node->isLeaf)
            {
                result.insert(result.end(), node->objects.begin(), node->objects.end());
            }
            else
            {
                for (const auto& child : node->children)
                {
                    if (child)
                        queryRecursive(child.get(), bounds, result);
                }
            }
        }
    };
} 