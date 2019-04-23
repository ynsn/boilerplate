/**
 * MIT License
 *
 * Copyright (c) 2019 Yoram
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef RENDOR_INCLUDE_CG_MESH_H_
#define RENDOR_INCLUDE_CG_MESH_H_

#include <iostream>
#include <vector>
#include <meshoptimizer.h>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/ProgressHandler.hpp>

#include "cg/common/VertexArray.h"
#include "cg/Vertex.h"
#include "cg/common/Shader.h"
#include "cg/common/Program.h"

#include <future>
#include <thread>

namespace cg {

class Mesh {
 public:
  std::vector<cg::Vertex> vertices;
  std::vector<unsigned int> indices;

  std::vector<cg::Vertex> boundingBoxVertices;
 private:
  cg::VertexArray vao;
  unsigned int vertexBuffer;
  unsigned int indexBuffer;

 public:
  Mesh(std::vector<cg::Vertex> verts, std::vector<unsigned int> indices)
      : vertices(verts), indices(indices) {

    vao.bind();
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(cg::Vertex), (const void *) offsetof(cg::Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(cg::Vertex), (const void *) offsetof(cg::Vertex, color));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(cg::Vertex), (const void *) offsetof(cg::Vertex, normal));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    vao.unbind();
  }
  ~Mesh() {
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
  }

  void draw(cg::ShaderProgram *program, glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
    glm::mat4 mvp = projection*view*model;

    vao.bind();
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    glUseProgram(program->getHandle());
    program->setUniformMat4f("E_MODEL", model);
    program->setUniformMat4f("E_VIEW", view);
    program->setUniformMat4f("E_PROJ", projection);
    program->setUniform4f("color", glm::vec4(1.0f, 0.7f, 0.3f, 1.0f));

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  }

  static Mesh *LoadMesh(const std::string file, unsigned int index) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_OptimizeGraph
        | aiProcess_OptimizeMeshes | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices
    );
    if (!scene) {
      std::cerr << "Import error: " << importer.GetErrorString() << "\n";
      return nullptr;
    }

    aiMesh *mesh = scene->mMeshes[index];
    std::vector<cg::Vertex> verts;
    std::vector<unsigned int> inds;
    for (int i = 0; i < mesh->mNumVertices; ++i) {
      cg::Vertex vertex;
      vertex.position.x = mesh->mVertices[i].x;
      vertex.position.y = mesh->mVertices[i].y;
      vertex.position.z = mesh->mVertices[i].z;
      vertex.normal.x = mesh->mNormals[i].x;
      vertex.normal.y = mesh->mNormals[i].y;
      vertex.normal.z = mesh->mNormals[i].z;
      verts.push_back(vertex);
    }

    for (int j = 0; j < mesh->mNumFaces; ++j) {
      aiFace face = mesh->mFaces[j];
      for (int i = 0; i < face.mNumIndices; ++i) {
        inds.push_back(face.mIndices[i]);
      }
    }

    verts.shrink_to_fit();
    inds.shrink_to_fit();
    size_t indexCount = inds.size();

    std::vector<unsigned int> remap(indexCount);
    size_t vertexCount =
        meshopt_generateVertexRemap(&remap[0], &inds[0], indexCount, &verts[0], verts.size(), sizeof(cg::Vertex));

    std::vector<cg::Vertex> finalVertices(vertexCount);
    std::vector<unsigned int> finalIndices(indexCount);

    meshopt_remapIndexBuffer(&finalIndices[0], &inds[0], indexCount, &remap[0]);
    meshopt_remapVertexBuffer(&finalVertices[0], &verts[0], vertexCount, sizeof(cg::Vertex), &remap[0]);

    meshopt_VertexCacheStatistics
        beforeVertexCache = meshopt_analyzeVertexCache(&finalIndices[00], indexCount, vertexCount, 32, 32, 32);
    meshopt_OverdrawStatistics beforeOverdraw =
        meshopt_analyzeOverdraw(&finalIndices[0], indexCount, &finalVertices[0].position.x, vertexCount,
                                sizeof(cg::Vertex));
    meshopt_VertexFetchStatistics beforeFetch = meshopt_analyzeVertexFetch(&finalIndices[0], indexCount, vertexCount,
                                                                           sizeof(cg::Vertex));

    meshopt_optimizeVertexCache(&finalIndices[0], &finalIndices[0], indexCount, vertexCount);

    meshopt_optimizeOverdraw(&finalIndices[0],
                             &finalIndices[0],
                             indexCount,
                             &finalVertices[0].position.x,
                             vertexCount,
                             sizeof(cg::Vertex),
                             1.05f);

    meshopt_optimizeVertexFetch(&finalVertices[0], &finalIndices[0], indexCount, &finalVertices[0], vertexCount,
                                sizeof(cg::Vertex));

    meshopt_VertexCacheStatistics
        afterVertexCache = meshopt_analyzeVertexCache(&finalIndices[00], indexCount, vertexCount, 32, 32, 32);
    meshopt_OverdrawStatistics
        afterOverdraw = meshopt_analyzeOverdraw(&finalIndices[0], indexCount, &finalVertices[0].position.x, vertexCount,
                                                sizeof(cg::Vertex));
    meshopt_VertexFetchStatistics afterFetch = meshopt_analyzeVertexFetch(&finalIndices[0], indexCount, vertexCount,
                                                                          sizeof(cg::Vertex));

    return new Mesh(finalVertices, finalIndices);
  }
};

}

#endif //RENDOR_INCLUDE_CG_MESH_H_
