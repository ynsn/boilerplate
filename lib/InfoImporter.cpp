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

#include "cg/InfoImporter.h"

namespace cg {

void AsyncInfoImporter::LoadAsync(const std::string &file) {
  this->future_ = std::async(std::launch::async, &AsyncInfoImporter::ImportMeshInfo, this, file, 0);
}

cg::MeshInfo AsyncInfoImporter::ImportMeshInfo(AsyncInfoImporter *i, const std::string &file, unsigned int index) {
  i->ready_ = false;
  i->progress = 0.0f;
  std::cout << "Importing...\n";
  Assimp::Importer importer;
  Handler *handler = new Handler(i);
  importer.SetProgressHandler(handler);

  const aiScene *scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes | aiProcess_FindInvalidData | aiProcess_PreTransformVertices
       | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType
  );

  if (!scene) {
    std::cerr << "Import error: " << importer.GetErrorString() << "\n";
  } else {
    std::cout << "Import finished!\n";
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
      afterVertexCache = meshopt_analyzeVertexCache(&finalIndices[0], indexCount, vertexCount, 32, 32, 32);
  meshopt_OverdrawStatistics
      afterOverdraw = meshopt_analyzeOverdraw(&finalIndices[0], indexCount, &finalVertices[0].position.x, vertexCount,
                                              sizeof(cg::Vertex));
  meshopt_VertexFetchStatistics afterFetch = meshopt_analyzeVertexFetch(&finalIndices[0], indexCount, vertexCount,
                                                                        sizeof(cg::Vertex));

  std::vector<unsigned int> simplified(indexCount);
  simplified.resize(meshopt_simplify(&simplified[0], &finalIndices[0], indexCount, &finalVertices[0].position.x, vertexCount, sizeof(cg::Vertex), indexCount - indexCount / 3, 0.25f));

  float acmr_before = beforeVertexCache.acmr;
  float acmr_after = afterVertexCache.acmr;
  float atvr_before = beforeVertexCache.atvr;
  float atvr_after = afterVertexCache.atvr;
  float overdraw_before = beforeOverdraw.overdraw;
  float overdraw_after = afterOverdraw.overdraw;
  float overfetch_before = beforeFetch.overfetch;
  float overfetch_after = afterFetch.overfetch;
  unsigned int indices_after = simplified.size();

  OptimizationStats s =
      {acmr_before, acmr_after, atvr_before, atvr_after, overdraw_before, overdraw_after, overfetch_before,
       overfetch_after, indexCount, indices_after};
  i->stats_ = s;

  i->ready_ = true;
  importer.SetProgressHandler(nullptr);
  delete handler;

  MeshInfo info(finalVertices, simplified);
  return info;
}

bool AsyncInfoImporter::IsReady() {
  return ready_;
}

cg::MeshInfo AsyncInfoImporter::Get() {
  MeshInfo i = future_.get();
  ready_ = false;
  return i;
}

bool AsyncInfoImporter::IsBusy() {
  return !ready_;
}

}
