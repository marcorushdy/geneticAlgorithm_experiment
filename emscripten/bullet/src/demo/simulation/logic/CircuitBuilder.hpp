
#pragma once

#include "thirdparty/GLMath.hpp"

#include <string>
#include <vector>
#include <functional>

class CircuitBuilder
{
public:
    typedef std::vector<glm::vec3>  t_vec3Array;
    typedef std::vector<int>        t_indices;

    struct t_startTransform
    {
        glm::vec3 position;
        glm::vec4 quaternion;
    };

    struct t_knot
    {
        glm::vec3 left;
        glm::vec3 right;
        float minDistance;
        glm::vec3 color;

        t_knot() = default;

        t_knot(
            const glm::vec3& left,
            const glm::vec3& right,
            float minDistance,
            const glm::vec3& color
        )
            : left(left)
            , right(right)
            , minDistance(minDistance)
            , color(color)
        {}
    };
    typedef std::vector<t_knot> t_knots;
    typedef t_knot t_circuitVertex;

public:
    typedef std::function<void(const t_vec3Array& vertices,
                               const t_indices& indices)> t_callbackNoNormals;

    typedef std::function<void(const t_vec3Array& vertices,
                               const t_vec3Array& colors,
                               const t_vec3Array& normals,
                               const t_indices& indices)> t_callbackNormals;

private:
    t_startTransform _startTransform;
    t_knots _knots;

public:
    void load(const std::string& filename);
    void load(const t_startTransform& startTransform, const t_knots& knots);

public:
    void generateSkeleton(t_callbackNoNormals onSkeletonPatch);
    void generate(t_callbackNormals onNewGroundPatch, t_callbackNormals onNewWallPatch);

public:
    const t_startTransform& getStartTransform() const;
    const t_knots& getKnots() const;

};
