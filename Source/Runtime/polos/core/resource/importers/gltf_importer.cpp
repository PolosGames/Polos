#include "polos/core/resource/importers/gltf_importer.h"

#include <assimp/BaseImporter.h>
#include <assimp/Importer.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/ProgressHandler.hpp>
#include <assimp/scene.h>

#include "polos/core/resource/resource.h"

namespace polos::resource
{
    GltfImporter::GltfImporter()
        : Base{
            CREATE_DELEGATE_MEM_FN(GltfImporter, Import, model* (std::string, import_options<model>, model*))
            , CREATE_DELEGATE_MEM_FN(GltfImporter, CanImport, bool(std::string))}
        , m_Importer{std::make_unique<Assimp::Importer>()}
    {
        m_IOHandle = m_Importer->GetIOHandler();
    }

    auto GltfImporter::Import(std::string p_Path, import_options<model>, model* p_Ptr) -> model*
    {
        std::string dir = p_Path.substr(0, p_Path.find_last_of('/'));
        // Remove points and lines.
        m_Importer->SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, 1);

        aiScene const* scene = m_Importer->ReadFile(p_Path,
            aiProcess_CalcTangentSpace
            | aiProcess_Triangulate
            | aiProcess_JoinIdenticalVertices
            | aiProcess_SortByPType
            | aiProcess_FindDegenerates
            | aiProcess_FindInvalidData
        );

        if (scene->mNumMeshes == 0)
        {
            LOG_ENGINE_ERROR("[GltfImporter::Import] The file that was being loaded has no meshes.\nPath:{}", p_Path);
        }

        model* model = p_Ptr;
        std::span<aiMesh*> meshes(scene->mMeshes, scene->mNumMeshes);

        for (uint32 i{}; i < scene->mNumMaterials; ++i)
        {
            auto& mat_info = model->materials.emplace_back();
            aiMaterial* mat = scene->mMaterials[i];

#if defined(PL_DEBUG)
            aiString mat_name;
            mat->Get(AI_MATKEY_NAME, mat_name);

            LOG_ENGINE_INFO("[GltfImporter::Import] Loading material: {}", mat_name.C_Str());
#endif

            // TODO: Create a seperate function for loading the textures
            aiString albedo_path;
            mat->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &albedo_path);

            if (albedo_path.length == 0)
            {
                LOG_ENGINE_WARN("[GltfImporter::Import] No base color texture found, skipping material.");
                continue;
            }

            mat_info.albedoMap = resource::ImportResource<image>(std::format("{}/{}", dir, albedo_path.C_Str()), {});

            aiString mr_path;
            mat->GetTexture(AI_MATKEY_METALLIC_TEXTURE, &mr_path);
            if (mr_path.length != 0)
            {
                mat_info.metallicRoughnessMap = resource::ImportResource<image>(std::format("{}/{}", dir, mr_path.C_Str()), {});
                mat->Get(AI_MATKEY_METALLIC_FACTOR, mat_info.metallicValue);
                mat->Get(AI_MATKEY_ROUGHNESS_FACTOR, mat_info.roughnessValue);
            }
        }

        // BFS to visit all aiNodes.
        std::queue<aiNode*> unvisited_nodes;
        unvisited_nodes.push(scene->mRootNode);
        
        model->nodes.emplace_back();
        std::size_t node_info_index{ 0 };

        while (!unvisited_nodes.empty())
        {
            aiNode* node = unvisited_nodes.front();

            // Get the current node that's going to be processed. The nodes in
            // the model->nodes vector are also in a BFS order.
            node_info& node_info = model->nodes[node_info_index++];

            //auto& node_info = model->nodes.emplace_back();

            // Register node's children's position as {start, count}
            node_info.childrenNodes = { model->nodes.size(), node->mNumChildren };

            // Find all child nodes and add them to the unvisited nodes.
            for (uint32 i{}; i < node->mNumChildren; ++i)
            {
                unvisited_nodes.push(node->mChildren[i]);
                model->nodes.emplace_back();
            }

            node_info.meshesSlice = { model->meshes.size(), node->mNumMeshes };

            // Process the meshes of the currently visited node
            for (uint32 i{}; i < node->mNumMeshes; ++i)
            {
                aiMesh* mesh    = meshes[node->mMeshes[i]];
                auto& mesh_info = model->meshes.emplace_back();

                auto verticesStart = model->vertices.size();
                for (uint32 j{}; j < mesh->mNumVertices; ++j)
                {
                    quad_vertex vertex{};

                    aiVector3D* pos = &mesh->mVertices[j];
                    vertex.position = glm::vec3(pos->x, pos->y, pos->z);

                    aiVector3D* normal = &mesh->mNormals[j];
                    if (normal != nullptr)
                    {
                        vertex.normal = glm::vec3(normal->x, normal->y, normal->z);
                    }

                    aiVector3D* tex_coords = mesh->mTextureCoords[0];
                    if (tex_coords != nullptr)
                    {
                        vertex.texture_coordinates = glm::vec2(tex_coords[j].x, tex_coords[j].y);
                    }

                    aiColor4D* color = mesh->mColors[0];
                    if (color != nullptr)
                    {
                        vertex.color = glm::vec4(color[j].r, color[j].g, color[j].b, color[j].a);
                    }

                    model->vertices.push_back(std::move(vertex));
                }

                mesh_info.verticesSlice = { verticesStart , mesh->mNumVertices };

                auto indicesStart = model->indices.size();
                for (uint32 j{}; j < mesh->mNumFaces; ++j)
                {
                    aiFace* face = &mesh->mFaces[j];
                    
                    for (uint32 k{}; k < face->mNumIndices; ++k)
                    {
                        model->indices.push_back(face->mIndices[k]);
                    }
                }

                auto indicesCount      = model->indices.size() - indicesStart;
                mesh_info.indicesSlice = { indicesStart, indicesCount };

                mesh_info.materialIndex = static_cast<size_t>(mesh->mMaterialIndex);
            }

            unvisited_nodes.pop();
        }

        return model;
    }

    auto GltfImporter::CanImport(std::string p_Path) -> bool
    {
        return m_Importer->IsExtensionSupported(p_Path.substr(p_Path.find_last_of('.')));
    }
} // namespace polos::resource