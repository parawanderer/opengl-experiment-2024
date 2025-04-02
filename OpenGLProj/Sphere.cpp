#include "Sphere.h"

#define _USE_MATH_DEFINES
#include <glad/glad.h>

#include "math.h"

// Generate triangles for a sphere. Mainly based on this, just rewritten into my own object: https://www.songho.ca/opengl/gl_sphere.html
Sphere::Sphere(int sectorCount, int stackCount, float radius):
_sectorCount(sectorCount), // number of horizontal rectangles on a single strip
_stackCount(stackCount), // number of vertical "strips" (kind of like the latitudes on an earth rendition) 
_radius(radius)
{
    // Generate vertices
    this->initVertices();

    // triangulate the rectangles above
    this->initIndices();

    this->setupSphere();
}

void Sphere::initVertices()
{
    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / this->_radius;    // vertex normal
    float s, t;                                     // vertex texCoord

    float sectorStep = 2 * M_PI / this->_sectorCount;
    float stackStep = M_PI / this->_stackCount;
    float sectorAngle, stackAngle;


    for (int i = 0; i <= this->_stackCount; ++i)
    {
        stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = this->_radius * cosf(stackAngle);             // r * cos(u)
        z = this->_radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= this->_sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            // vertex tex coord (s, t) range between [0, 1]
            s = (float)j / this->_sectorCount;
            t = (float)i / this->_sectorCount;

            this->_vertices.push_back({
                .position = glm::vec3(x, y, z),
                .normal = glm::vec3(nx, ny, nz),
                .texCoords = glm::vec2(s, t) // ** not going to use these for now
            });
        }
    }
}

void Sphere::initIndices()
{
    // generate CCW index list of sphere triangles
    // k1--k1+1
    // |  / |
    // | /  |
    // k2--k2+1
    //std::vector<int> lineIndices;
    int k1, k2;
    for (int i = 0; i < this->_stackCount; ++i)
    {
        k1 = i * (this->_sectorCount + 1);     // beginning of current stack
        k2 = k1 + this->_sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < this->_sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0)
            {
                this->_indices.push_back(k1);
                this->_indices.push_back(k2);
                this->_indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (this->_stackCount - 1))
            {
                this->_indices.push_back(k1 + 1);
                this->_indices.push_back(k2);
                this->_indices.push_back(k2 + 1);
            }

            // store indices for lines
            // vertical lines for all stacks, k1 => k2
            // lineIndices.push_back(k1);
            // lineIndices.push_back(k2);
            // if (i != 0)  // horizontal lines except 1st stack, k1 => k+1
            // {
            //     lineIndices.push_back(k1);
            //     lineIndices.push_back(k1 + 1);
            // }
        }
    }
}

void Sphere::setupSphere()
{
    glGenVertexArrays(1, &this->_VAO);
    glGenBuffers(1, &this->_VBO);
    glGenBuffers(1, &this->_EBO);

    glBindVertexArray(this->_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->_VBO);

    glBufferData(GL_ARRAY_BUFFER, this->_vertices.size() * sizeof(Vertex), &this->_vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->_indices.size() * sizeof(unsigned int), &this->_indices[0], GL_STATIC_DRAW);


    //vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    //vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    //texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);
}


void Sphere::draw(Shader& shader)
{
    glBindVertexArray(this->_VAO);
    glDrawElements(GL_TRIANGLES, this->_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
