#include "mesh.h"
#include "service_locator/locator.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, int id):
    vertices(vertices), indices(indices), id(id)
{

}

Mesh::~Mesh()
{
    Locator::getRenderer()->meshBuffersDelete(id);
}

void Mesh::draw() const
{
    Locator::getRenderer()->drawMesh(id);
}

void Mesh::generateBuffers()
{
    Locator::getRenderer()->meshBuffersGenerate(id);
}

void Mesh::buffersPushData()
{
    Locator::getRenderer()->meshBuffersPushData(id, vertices, indices);
}
