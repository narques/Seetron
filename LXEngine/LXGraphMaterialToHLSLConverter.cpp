//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXGraphMaterialToHLSLConverter.h"
#include "LXAssetManager.h"
#include "LXConstantBuffer.h"
#include "LXConnection.h"
#include "LXConnector.h"
#include "LXCore.h"
#include "LXFile.h"
#include "LXGraph.h"
#include "LXGraphMaterial.h"
#include "LXShaderFactory.h"
#include "LXGraphTemplate.h"
#include "LXMaterial.h"
#include "LXNode.h"
#include "LXRenderPass.h"
#include "LXTexture.h"
#include "LXTextureD3D11.h"
#include "LXMaterialD3D11.h"
#include "LXReference.h"
#include "LXMemory.h" // --- Must be the last included ---

LX_PRAGMA_OPTIM_OFF

LXGraphMaterialToHLSLConverter::LXGraphMaterialToHLSLConverter()
{
}

LXGraphMaterialToHLSLConverter::~LXGraphMaterialToHLSLConverter()
{
}

LXStringA LXGraphMaterialToHLSLConverter::GenerateCode(const LXMaterialD3D11* materialD3D11, ERenderPass renderPass)
{
	const LXGraph* graph = (const LXGraph*)materialD3D11->GetMaterial()->GetGraph();
		
	// Start from the "main function node"
	const LXNode* nodeRoot = graph->GetMain();

	if (!nodeRoot)
		return "";

	CHK(nodeRoot->Outputs.size() == 0);
	if (!nodeRoot)
		return "";

	LXStringA ShaderBuffer;
	ShaderBuffer = GenerateHeaderComment();
	
	// Prepare the "sections"

	LXStringA defaultInlclude;

	defaultInlclude = "common.hlsl";
	if (renderPass == ERenderPass::GBuffer)
	{
		defaultInlclude = "common.hlsl";
	}
	else if (renderPass == ERenderPass::Transparency)
	{
		defaultInlclude = "Forward.hlsl";
	}
	else
	{
		CHK(0);
	}
	
	ShaderBuffer += "//INCLUDES\n\n";
	ShaderBuffer += "//CONSTANT_BUFFER\n\n";
	ShaderBuffer += "//TEXTURES\n\n";
	
	// Traverse the graph
	materialD3D11->HLSLTextureIndex = 0;
	ShaderBuffer += ParseNode(materialD3D11, -1,  nullptr, nodeRoot, renderPass);

	// Insert the includes file
	int start = ShaderBuffer.Find("//INCLUDES");
	if (start == -1)
	{
		CHK(0);
	}
	else
	{
		if (_includes.size() > 1)
		{
			int foo = 0;
		}
			//_includes.insert(defaultInlclude);

		start += (int)strlen("//INCLUDES") + 1;
		for (const LXStringA& fileToinclude : _includes)
		{
			LXStringA include = "#include \"" + fileToinclude + "\"\n";
			ShaderBuffer.Insert(start, include.GetBuffer());
		}

		LXStringA include = "#include \"" + defaultInlclude + "\"\n";
		ShaderBuffer.Insert(start, include.GetBuffer());
	}

	// Insert the variable declarations
	start = ShaderBuffer.Find("//VARIABLES");
	if (start == -1)
	{
		CHK(0);
	}
	else
	{
		start += (int)strlen("//VARIABLES") + 1;
		for (const LXStringA& variable : _variableDeclarations)
		{
			LXStringA line = "\t" + variable + "\n";
			ShaderBuffer.Insert(start, line);
		}
	}
	
	return ShaderBuffer;
}

bool LXGraphMaterialToHLSLConverter::GenerateConstanBuffer(const LXGraph* graph, LXConstantBuffer& outConstantBuffer)
{
	const LXNode* nodeRoot = graph->GetMain();

	if (!nodeRoot)
		return false;

	CHK(nodeRoot->Outputs.size() == 0);
	if (!nodeRoot)
		return false;

	return ParseNodeCB(*nodeRoot, outConstantBuffer);
}

bool LXGraphMaterialToHLSLConverter::GatherTextures(const LXGraph* graph, list<LXTextureD3D11*>& listTextures)
{
	const LXNode* nodeRoot = graph->GetMain();

	if (!nodeRoot)
		return false;

	CHK(nodeRoot->Outputs.size() == 0);
	if (!nodeRoot)
		return false;

	return ParseNodeTexture(*nodeRoot, listTextures);
}

LXStringA LXGraphMaterialToHLSLConverter::ParseNode(const LXMaterialD3D11* materialD3D11, int outputConnectorIndex, const LXConnector* connector, const LXNode* node, ERenderPass renderPass)
{
	
	//
	// Retrieve the NodeTemplate 
	//

	const LXGraphTemplate* graphTemplate = GetAssetManager()->GetGraphMaterialTemplate();
	const LXNodeTemplate* nodeTemplate = graphTemplate->GetNodeTemplate(node->TemplateID);
	const LXConnectorTemplate* connectorTemplate = nodeTemplate->GetOutputConnectorTemplate(outputConnectorIndex);
	
	//
	// Variable -> ConstantBuffer
	//

	if (nodeTemplate->Type == L"Variable")
	{
		return ParseNodeVariable(materialD3D11, node);
	}
	else if (nodeTemplate->Type == L"Constant")
	{
		return ParseNodeConstant(node);
	}

	//
	// Function
	//

	LXStringA code;
	
	// Node may use a intermediate variable.
	
	// Generate a free var name
	LXStringA variableName = "toto";

	// fix the var name declaration
	LXStringA* variableDeclaration = nullptr;

	// Local variables
	if (nodeTemplate->Variables.size() > 0)
	{
		// Node instance local variable only declared one time.
		if (_addedNode.find(node) == _addedNode.end())
		{
			// Local variable to add.
//			variableName = nodeTemplate->LocalVariables.ToStringA();

			LXStringA v = "const float4 toto = $x;";
			_variableDeclarations.push_back(v);

			variableDeclaration = &_variableDeclarations.back();
			

			_addedNode.insert(node);
		}
	}

	if (node->Main == true)
	{
		// the main function is generated according the "Role"

		if (renderPass == ERenderPass::GBuffer)
		{
			if (materialD3D11->GetMaterial()->GetLightingModel() == EMaterialLightingModel::Unlit)
			{
				code = "static const float lightModel = 1.0;\n"; // Unlit
			}
			else
			{
				code = "static const float lightModel = 0.0;\n"; // Lit
			}

			code +=
				"//--------------------------------------------------------------------------------------\n"
				"// Pixel Shader - GBuffer\n"
				"//--------------------------------------------------------------------------------------\n"
				"PS_OUTPUT PS(VS_OUTPUT input, uint IsFrontFace : SV_IsFrontFace)\n"
				"{\n"
				"	//VARIABLES\n"
				"	PS_OUTPUT output = (PS_OUTPUT)0;\n"
				"	output.OutColor0 = float4($Albedo, $Opacity); // Albedo\n"
				"	output.OutColor1 = float4($Normal, 0.0); // Normal\n"
				"	output.OutColor2 = float4($Metal, $Rough, 0.0, lightModel); // MRUL\n"
				"	output.OutColor3 = float4($Emissive, 0.0); // Emissive\n"
				"	return output;\n"
				"}\n";
		}
		else if (renderPass == ERenderPass::Transparency)
		{
			if (materialD3D11->GetMaterial()->GetLightingModel() == EMaterialLightingModel::Unlit)
			{
				code = 
					"//--------------------------------------------------------------------------------------\n"
					"// Pixel Shader - Unlit transparency \n"
					"//--------------------------------------------------------------------------------------\n"
					"float4 PS(VS_OUTPUT input, uint IsFrontFace : SV_IsFrontFace) : SV_Target\n"
					"{\n"
					"	//VARIABLES\n"
					"	return float4($Albedo, $Opacity);\n"
					"}\n";
			}
			else
			{
				code =
					"//--------------------------------------------------------------------------------------\n"
					"// Pixel Shader - Lit Transparency \n"
					"//--------------------------------------------------------------------------------------\n"
					"float4 PS(VS_OUTPUT input, uint IsFrontFace : SV_IsFrontFace) : SV_Target\n"
					"{\n"
					"	//VARIABLES\n"
					"	float3 EyeDir = normalize(input.WorldPosition.xyz - CameraPosition);\n"
					"	return ComputeColor(float4($Albedo, $Opacity), float4($Metal, $Rough, 0.0, 0.0), EyeDir, input);\n"
					"}\n";
			}
		}
		else
		{
			CHK(0);
			return "";
		}
	}
	else
	{
		//code = nodeTemplate->Declaration.ToStringA();

		code = connectorTemplate->Code.ToStringA();

		if (nodeTemplate->GetName() == L"CustomFunction")
		{
			// Add include
			LXPropertyAssetPtr* propertyShader = dynamic_cast<LXPropertyAssetPtr*>(node->GetProperty(L"Shader"));
			LXStringA include = propertyShader->GetValue()->GetFilepath().GetFilename().ToStringA();
			_includes.insert(include);

			// Function name
			LXPropertyString* propertyFunction = dynamic_cast<LXPropertyString*>(node->GetProperty(L"Function"));
			LXStringA function = propertyFunction->GetValue().ToStringA();
			
			// Parameters
			ListProperties properties;
			node->GetUserProperties(properties);

			LXStringA userParameters;

			for (const LXConnector* connector : node->Inputs)
			{
				if ((connector->GetName() == L"Shader") || (connector->GetName() == L"Function"))
					continue;

				if (connector->Type == EConnectorType::Float)
				{
					userParameters += ",/*float*/ $" + connector->GetName().ToStringA();
				}
				else if (connector->Type == EConnectorType::Float2)
				{
					userParameters += ",/*float2*/ $" + connector->GetName().ToStringA();
				}
				else if (connector->Type == EConnectorType::Float3)
				{
					userParameters += ",/*float3*/ $" + connector->GetName().ToStringA();
				}
				else if (connector->Type == EConnectorType::Float4)
				{
					userParameters += ",/*float4*/ $" + connector->GetName().ToStringA();
				}
				else
				{
					CHK(0);
				}
			}

			code.Replace("$CustomFunction", function);
			code.Replace("$Parameters", userParameters);
		}
	}

	// Try to replace $XXX by the inputs
	for (const LXConnector* connector : node->Inputs)
	{
		const LXStringA connectorName = "$" + connector->GetName().ToStringA();
		const int i = code.Find(connectorName);
		const int j = variableDeclaration ? variableDeclaration->Find(connectorName): string::npos;
		
		if (i == string::npos && j == string::npos)
		{
			// The connector is not used at all.
			continue;
		}
		
		LXStringA connectorValue;
						
		// Input is unique
		CHK(connector->Connections.size() <= 1);
 
		if (connector->Connections.size() == 1)
		{
			const LXConnection* connection  = connector->Connections.back();
			if(connection->Source != nullptr)
			{
				LXNode* callerNode = connection->Source->GetOwner();
				LXConnector* connectorSource = connection->Source.get();
				int connectorSourceIndex = callerNode->GetOuputConnectorIndex(connectorSource);

				if (connectorSourceIndex != -1)
				{
					connectorValue = ParseNode(materialD3D11, connectorSourceIndex, connectorSource, callerNode, renderPass);
				}
				else
				{
					LogE(LXGraphMaterialToHLSLConverter, L"Unable to retrieve connector source index. Node is %s", callerNode->GetName().GetBuffer());
					return "";
				}

				if (node->Main && connector->GetName() == L"Normal")
				{
					connectorValue  = "ComputeNormal(input," + connectorValue + ")";
				}
				
				CHK(!connectorValue.IsEmpty());
			}
		}
		else
		{
			// Default
			if (connector->Type == EConnectorType::Float)
			{
				if (node->Main && connector->GetName() == L"Opacity")
				{
					connectorValue = "1.0";
				}
				else if (node->Main && connector->GetName() == L"Metal")
				{
					connectorValue = "0.5";
				}
				else
				{
					connectorValue = "0.0";
				}
			}
			else if (connector->Type == EConnectorType::Float2)
			{
				if (connector->GetName() == L"UV")
				{
					connectorValue = "input.Tex";
				}
				else
				{
					connectorValue = "float2(0.0, 0.0)";
				}
			}
			else if (connector->Type == EConnectorType::Float3)
			{
				if (node->Main && connector->GetName() == L"Normal")
				{
					connectorValue = "GetDefaultNormal(input, IsFrontFace)";
				}
				else if (node->Main && connector->GetName() == L"Color")
				{
					connectorValue = "float3(0.5, 0.5, 0.5)";
				}
				else
				{
					connectorValue = "float3(0.0, 0.0, 0.0)";
				}
			}
			else if (connector->Type == EConnectorType::Float4)
			{
				connectorValue = "float4(0.0, 0.0, 0.0, 0.0)";
			}
			else if (connector->Type == EConnectorType::SamplerState)
			{
				connectorValue = "sampler" + LXStringA::Number(materialD3D11->HLSLTextureIndex-1);
			}
			else
			{
				CHK(0);
			}
		}

		CHK(!connectorValue.IsEmpty());
		code.Replace(connectorName, connectorValue);

		if (variableDeclaration)
		{
			variableDeclaration->Replace(connectorName, connectorValue);
		}
	}

	// Try to replace $XXX by the variables
	for (const LXVariableTemplate& variable : nodeTemplate->Variables)
	{
		LXStringA macroVariableName = "$" + variable.Name.ToStringA();

		int i = code.Find(macroVariableName);

		if (i == string::npos)
		{
			// The connector is not used.
			continue;
		}

		CHK(!variableName.IsEmpty());
		code.Replace(macroVariableName, variableName);
	}
	
	return code;
}

bool LXGraphMaterialToHLSLConverter::ParseNodeCB(const LXNode& node, LXConstantBuffer& outConstantBuffer)
{

	//
	// Retrieve the NodeTemplate 
	//

	const LXGraphTemplate* graphTemplate = GetAssetManager()->GetGraphMaterialTemplate();
	const LXNodeTemplate* nodeTemplate = graphTemplate->GetNodeTemplate(node.TemplateID);

	//
	// Variable
	//

	if (nodeTemplate->Type == L"Variable")
	{
		// Variables have not input.
		CHK(node.Inputs.size() == 0);

		// Retrieve the UserProperty where is stored the "Variable"
		LXProperty* property = node.GetProperty(L"Value");
		if (property->GetType() == EPropertyType::Float)
		{
			LXPropertyFloat* propertyFloat = (LXPropertyFloat*)property;
			const float& value = propertyFloat->GetValue();
			outConstantBuffer.AddFloat(node.GetName(), value);
			return true;
		}
		else if (property->GetType() == EPropertyType::Float2)
		{
			LXPropertyVec2f* propertyFloat = (LXPropertyVec2f*)property;
			const vec2f& value = propertyFloat->GetValue();
			outConstantBuffer.AddFloat2(node.GetName(), value);
			return true;
		}
		else if (property->GetType() == EPropertyType::Float3)
		{
			LXPropertyVec3f* propertyFloat = (LXPropertyVec3f*)property;
			const vec3f& value = propertyFloat->GetValue();
			outConstantBuffer.AddFloat3(node.GetName(), value);
			return true;
		}
		else if (property->GetType() == EPropertyType::Float4)
		{
			LXPropertyVec4f* propertyFloat = (LXPropertyVec4f*)property;
			const vec4f& value = propertyFloat->GetValue();
			outConstantBuffer.AddFloat4(node.GetName(), value);
			return true;
		}
		else if (property->GetType() == EPropertyType::AssetPtr)
		{
			// Textures are ignored.	
			return true;
		}
		else
		{
			CHK(0);
			return false;
		}
	}

	//
	// Function
	//

	bool res = true;

	for (const LXConnector* connector : node.Inputs)
	{
		LXStringA connectorName = "$" + connector->GetName().ToStringA();

		// Input is unique
		CHK(connector->Connections.size() <= 1);

		if (connector->Connections.size() == 1)
		{
			const LXConnection* connection = connector->Connections.back();
			if (connection->Source != nullptr)
			{
				res &= ParseNodeCB(*connection->Source->GetOwner(), outConstantBuffer);
			}
		}
	}

	return res;
}

bool LXGraphMaterialToHLSLConverter::ParseNodeTexture(const LXNode& node, list<LXTextureD3D11*>& listTextures)
{

	//
	// Retrieve the NodeTemplate 
	//

	const LXGraphTemplate* graphTemplate = GetAssetManager()->GetGraphMaterialTemplate();
	const LXNodeTemplate* nodeTemplate = graphTemplate->GetNodeTemplate(node.TemplateID);

	//
	// Variable
	//

	if (nodeTemplate->Type == L"Variable")
	{
		// Variables have not input.
		CHK(node.Inputs.size() == 0);

		// Retrieve the UserProperty where is stored the "Variable"
		LXProperty* property = node.GetProperty(L"Value");
		if (property->GetType() == EPropertyType::Float)
		{
			// Ignored
			return true;
		}
		else if (property->GetType() == EPropertyType::Float2)
		{
			// Ignored
			return true;
		}
		else if (property->GetType() == EPropertyType::Float3)
		{
			// Ignored
			return true;
		}
		else if (property->GetType() == EPropertyType::Float4)
		{
			// Ignored
			return true;
		}
		else if (property->GetType() == EPropertyType::AssetPtr)
		{
			LXPropertyAssetPtr* propertyAssetPtr = (LXPropertyAssetPtr*)property;
			if (LXAsset* asset = propertyAssetPtr->GetValue())
			{
				LXTexture* texture = dynamic_cast<LXTexture*>(asset);
				{
					if (_addedTextures.find(texture) == _addedTextures.end())
					{
						LXTextureD3D11* textureD3D11 = LXTextureD3D11::CreateFromTexture(texture);
						textureD3D11->Slot = (uint)listTextures.size();
						listTextures.push_back(textureD3D11);
						_addedTextures.insert(texture);
					}

					return true;
				}
			}
			CHK(0);
			return false;
		}
		else
		{
			CHK(0);
			return false;
		}
	}

	//
	// Function
	//

	bool res = true;

	for (const LXConnector* connector : node.Inputs)
	{
		LXStringA connectorName = "$" + connector->GetName().ToStringA();

		// Input is unique
		CHK(connector->Connections.size() <= 1);

		if (connector->Connections.size() == 1)
		{
			const LXConnection* connection = connector->Connections.back();
			if (connection->Source != nullptr)
			{
				res &= ParseNodeTexture(*connection->Source->GetOwner(), listTextures);
			}
		}
	}

	return res;
}

LXStringA LXGraphMaterialToHLSLConverter::GenerateHeaderComment() const
{
	return "// *** Generated by seetron engine ***\n\n";
}

LXStringA LXGraphMaterialToHLSLConverter::ParseNodeVariable(const LXMaterialD3D11* materialD3D11, const LXNode* node)
{

	// Variables have not input.
	CHK(node->Inputs.size() == 0);

	// Retrieve the UserProperty where is stored the "Variable"
	LXProperty* property = node->GetProperty(L"Value");
	if (property->GetType() == EPropertyType::Float)
	{
		return node->GetName().ToStringA();
	}
	else if (property->GetType() == EPropertyType::Float2)
	{
		return node->GetName().ToStringA();
	}
	else if (property->GetType() == EPropertyType::Float3)
	{
		return node->GetName().ToStringA();
	}
	else if (property->GetType() == EPropertyType::Float4)
	{
		return node->GetName().ToStringA();
	}
	else if (property->GetType() == EPropertyType::AssetPtr)
	{
		// Texture name is automatically generated.
		int index = 0;
		if (_textureIndices.find(node) == _textureIndices.end())
		{
			index = materialD3D11->HLSLTextureIndex++;
			_textureIndices[node] = index;
		}
		else
		{
			index = _textureIndices[node];
		}

		return "texture" + LXStringA::Number(index);
	}
	else
	{
		CHK(0);
		return "";
	}
}

LXStringA LXGraphMaterialToHLSLConverter::ParseNodeConstant(const LXNode* node)
{
	// Retrieve the UserProperty where is stored the constant value;
	LXProperty* property = node->GetProperty(L"Value");
	
	if (property->GetType() == EPropertyType::Float)
	{
		LXPropertyFloat* propertyFloat =  static_cast<LXPropertyFloat*>(property);
		float value = propertyFloat->GetValue();
		return LXStringA::Format("%f", value);
	}
	else if (property->GetType() == EPropertyType::Float2)
	{
		LXPropertyVec2f* propertyFloat2 = static_cast<LXPropertyVec2f*>(property);
		vec2f value = propertyFloat2->GetValue();
		return LXStringA::Format("float2(%f,%f)", value.x, value.y);
	}
	else if (property->GetType() == EPropertyType::Float3)
	{
		LXPropertyVec3f* propertyFloat3 = static_cast<LXPropertyVec3f*>(property);
		vec3f value = propertyFloat3->GetValue();
		return LXStringA::Format("float3(%f, %f, %f)", value.x, value.y, value.z);
	}
	else if (property->GetType() == EPropertyType::Float4)
	{
		LXPropertyVec4f* propertyFloat4 = static_cast<LXPropertyVec4f*>(property);
		vec4f value = propertyFloat4->GetValue();
		return LXStringA::Format("float3(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
	}
	else
	{
		CHK(0);
		return "";
	}
}

LX_PRAGMA_OPTIM_ON

