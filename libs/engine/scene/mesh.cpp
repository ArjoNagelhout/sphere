#include "mesh.h"
#include "engine.h"

#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>

namespace engine {

    Mesh::Mesh(const std::string &filePath) : allocator(*engine->allocator) {
        loadObj(filePath);
        allocator.createBuffer<VertexAttributes>(vertexBuffer,
                                                  vertexBufferAllocation,
                                                  *vertices.data(),
                                                  vertices.size() * sizeof(vertices[0]),
                                                  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        allocator.createBuffer<uint32_t>(indexBuffer,
                                          indexBufferAllocation,
                                          *indices.data(),
                                          indices.size() * sizeof(indices[0]),
                                          VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    }

    Mesh::~Mesh() {
        allocator.destroyBuffer(vertexBuffer);
        allocator.destroyBuffer(indexBuffer);
    }

    /*
     * Todo: refactor to use index buffer again
     */
    void Mesh::loadObj(const std::string &filePath) {

        vertices.clear();
        indices.clear();

        tinyobj::ObjReaderConfig config;
        //config.mtl_search_path = "./";

        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(filePath, config)) {
            if (!reader.Error().empty()) {
                throw std::runtime_error(
                        std::string("tiny obj reader failed to read from file: ") + reader.Error());
            }
        }

        if (!reader.Warning().empty()) {
            std::cout << "tiny obj reader warning: " << reader.Warning() << std::endl;
        }

        auto &attributes = reader.GetAttrib();
        auto &shapes = reader.GetShapes();
        //auto &materials = reader.GetMaterials();

//        for (size_t v = 0; v < attributes.vertices.size(); v += 3) {
//
//            VertexAttributes vertexData{
//                    .position = {
//                            attributes.vertices[v + 0],
//                            attributes.vertices[v + 1],
//                            attributes.vertices[v + 2]
//                    },
//                    .uv = {
//                            attributes.texcoords[v + 0],
//                            attributes.texcoords[v + 1]
//                    },
//                    .normal = {
//                            attributes.normals[v + 0],
//                            attributes.normals[v + 1],
//                            attributes.normals[v + 2]
//                    }
//            };
//            vertices.push_back(vertexData);
//        }

//        for (size_t s = 0; s < shapes.size(); s++) {
//            const std::vector<tinyobj::index_t> &sourceIndices = shapes[s].mesh.indices;
//
//            for (size_t i = 0; i < sourceIndices.size(); i++) {
//                const tinyobj::index_t &index = sourceIndices[i];
//                indices.push_back(index.vertex_index);
//            }
//        }

        std::cout << "loaded 3d model at: " << filePath << std::endl;

        for (size_t s = 0; s < shapes.size(); s++) {

            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

                for (size_t v = 0; v < fv; v++) {
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                    tinyobj::real_t vx = attributes.vertices[3 * size_t(idx.vertex_index) + 0];
                    tinyobj::real_t vy = attributes.vertices[3 * size_t(idx.vertex_index) + 1];
                    tinyobj::real_t vz = attributes.vertices[3 * size_t(idx.vertex_index) + 2];

                    VertexAttributes out{
                            .position = {vx, vy, vz},
                    };

                    // Check if `normal_index` is zero or positive. negative = no normal data
                    if (idx.normal_index >= 0) {
                        tinyobj::real_t nx = attributes.normals[3 * size_t(idx.normal_index) + 0];
                        tinyobj::real_t ny = attributes.normals[3 * size_t(idx.normal_index) + 1];
                        tinyobj::real_t nz = attributes.normals[3 * size_t(idx.normal_index) + 2];

                        out.normal = {nx, ny, nz};
                    }

                    // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                    if (idx.texcoord_index >= 0) {
                        tinyobj::real_t tx = attributes.texcoords[2 * size_t(idx.texcoord_index) + 0];
                        tinyobj::real_t ty = attributes.texcoords[2 * size_t(idx.texcoord_index) + 1];

                        out.uv = {tx, ty};
                    }
                    // Optional: vertex colors
                    // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                    // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                    // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];

                    vertices.push_back(out);
                    indices.push_back(indices.size());
                }
                index_offset += fv;
            }
        }
    }
}