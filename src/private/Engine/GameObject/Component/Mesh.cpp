#include "Engine/GameObject/Component/Mesh.h"
#include "Engine/Core.h"

Mesh::Mesh() : Component::Component() {
	this->core = Core::GetInstance();
	this->core->GetDeviceAndCommandList(this->dev, this->list);

	this->bMeshLoaded = false;
}

void Mesh::Start() {
	Component::Start();
}

void Mesh::Update() {
	Component::Update();

	if (this->bMeshLoaded) {

	}
}

/*!
	This method will load a mesh from file
		Note: We only support FBX by the moment.
*/
void Mesh::LoadFromFile(std::string file) {
	if (this->bMeshLoaded) return;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file, NULL);

	aiMesh* mesh = scene->mMeshes[0]; // TODO: Add multi-mesh support.
	aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vec = mesh->mVertices[i];
		RGB pos = { vec.x, vec.y, vec.z };
		RGB nml = { 0.f, 0.f, 0.f };
		RG uv = { 0.f, 0.f };

		if (mesh->HasNormals()) {
			nml[0] = mesh->mNormals[i].x;
			nml[1] = mesh->mNormals[i].y;
			nml[2] = mesh->mNormals[i].z;
		}

		if (mesh->HasTextureCoords(0)) {
			uv[0] = mesh->mTextureCoords[0][i].x;
			uv[0] = mesh->mTextureCoords[0][i].x;
		}

		Vertex vertex = { { pos[0], pos[1], pos[2] }, { nml[0], nml[1], nml[2] }, { uv[0], uv[1] } };
		this->vertices.push_back(vertex);
	}

	this->bMeshLoaded = true;
}