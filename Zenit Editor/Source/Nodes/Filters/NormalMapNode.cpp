#include "NormalMapNode.h"

namespace Zenit {

	NormalMapNode::NormalMapNode(int id, const char* name, NodeOutputType outputType)
		: ComputeShaderNode(id, name, outputType)
	{
		type = NodeType::NORMAL_MAP;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/normal_map.shader");
		texture = std::make_shared<Texture2D>(nullptr, 512, 512);
		inputTexture = std::make_unique<Texture2D>("Settings/white.png");
	}

	NormalMapNode::~NormalMapNode()
	{
	}

	void NormalMapNode::Update(TimeStep ts)
	{
		if (!regenerate)
			return;

		BindCoreData();
		inputTexture->Bind(1);
		computeShader->SetUniform1i("inputTexture", 1);
		computeShader->SetUniform1f("zoom", zoom);
		computeShader->SetUniform1f("bumpness", bumpness);
		//inputTexture->Unbind();
		DispatchCompute(1, 1);
		//inputTexture->UnbindImage();
	}

	void NormalMapNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void NormalMapNode::OnImGuiInspectorRender()
	{
		bool changedZoom = ImGui::DragFloat("Zoom", &zoom, 0.05f, 0.0f);
		bool changedBumbpness = ImGui::DragFloat("Bumpness", &bumpness, 0.005f, -1.0f, 1.0f);
		
		if (changedZoom || changedBumbpness)
			regenerate = true;

		ImGui::Image((void*)texture->GetId(), { 256,256 }, { 0,1 }, { 1,0 });
		
	}

	void NormalMapNode::SetTexture(std::shared_ptr<Texture2D> texture)
	{
		inputTexture = texture;
		regenerate = true;
	}

	SerializerValue NormalMapNode::Save()
	{
		SerializerValue value = JSONSerializer::CreateValue();
		SerializerObject object = JSONSerializer::CreateObjectFromValue(value);

		JSONSerializer::SetNumber(object, "id", id.Get());
		JSONSerializer::SetNumber(object, "type", (int)type);
		JSONSerializer::SetNumber(object, "color", zoom);
		JSONSerializer::SetNumber(object, "bumpness", bumpness);
		JSONSerializer::SetString(object, "inputTexture", inputTexture->GetName().c_str());

		return value;
	}
}