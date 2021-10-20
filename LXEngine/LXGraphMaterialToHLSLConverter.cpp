//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXGraphMaterialToHLSLConverter.h"

// Seetron
#include "LXAssetManager.h"
#include "LXConstantBuffer.h"
#include "LXConnection.h"
#include "LXConnector.h"
#include "LXEngine.h"
#include "LXFile.h"
#include "LXGraphMaterial.h"
#include "LXShaderFactory.h"
#include "LXGraphTemplate.h"
#include "LXLogger.h"
#include "LXMaterial.h"
#include "LXMaterialInstance.h"
#include "LXMaterialUtility.h"
#include "LXNode.h"
#include "LXRenderPassType.h"
#include "LXTexture.h"
#include "LXMaterialD3D11.h"
#include "LXReference.h"
#include "LXInputElementDescD3D11Factory.h"

LXGraphMaterialToHLSLConverter::LXGraphMaterialToHLSLConverter(const LXMaterialBase* material): _material(material)
{
}

LXGraphMaterialToHLSLConverter::~LXGraphMaterialToHLSLConverter()
{
}

LXStringA LXGraphMaterialToHLSLConverter::GenerateCode(const LXMaterialD3D11* materialD3D11, ERenderPass renderPass, EShader Shader, int layoutMask)
{
	_renderPass = renderPass;
	_shader = Shader;
	_layoutMask = layoutMask;

	const LXMaterial* material =  LXMaterialUtility::GetMaterialFromBase(_material);
	const LXGraphMaterial* graph = material->GetGraph();
		
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
	if (renderPass == ERenderPass::Transparency)
		defaultInlclude = "Forward.hlsl";
	else
		defaultInlclude = "common.hlsl";
	
	ShaderBuffer += "//INCLUDES\n\n";
	ShaderBuffer += "//CONSTANT_BUFFER\n\n";
	ShaderBuffer += "//TEXTURES\n\n";
	
	_HLSLVSTextureIndex = 0;
	_HLSLPSTextureIndex = 0;
	ShaderBuffer += ParseNode(materialD3D11, nullptr, nodeRoot, renderPass);

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

bool LXGraphMaterialToHLSLConverter::GenerateConstanBuffer(const LXGraph* graph, EShader Shader, LXConstantBuffer& outConstantBuffer)
{
	//_renderPass = renderPass;
	_shader = Shader;

	const LXNode* nodeRoot = graph->GetMain();

	if (!nodeRoot)
		return false;

	CHK(nodeRoot->Outputs.size() == 0);
	if (!nodeRoot)
		return false;

	return ParseNodeCB(*nodeRoot, outConstantBuffer);
}

void LXGraphMaterialToHLSLConverter::GatherTextures(const LXGraph* graph, EShader shader, std::list<LXTexture*>& outTextures)
{
	_shader = shader;

	const LXNode* nodeRoot = graph->GetMain();

	if (!nodeRoot)
		return;

	CHK(nodeRoot->Outputs.size() == 0);

	ParseNodeTexture(*nodeRoot, outTextures);
}

LXStringA LXGraphMaterialToHLSLConverter::ParseNode(const LXMaterialD3D11* materialD3D11, const LXConnector* connector, const LXNode* node, ERenderPass renderPass)
{
	
	//
	// Retrieve the NodeTemplate 
	//

	const LXGraphTemplate* graphTemplate = GetAssetManager()->GetGraphMaterialTemplate();
	const LXNodeTemplate* nodeTemplate = graphTemplate->GetNodeTemplate(node->TemplateID);
	const LXConnectorTemplate* connectorTemplate = nodeTemplate->GetOutputConnectorTemplate(connector);

	if (connector && !connectorTemplate)
	{
		LogE(LXGraphMaterialToHLSLConverter, L"Failed to found a ConnectorTemplace for Material %s, Node %s, Connector %s.", _material->GetFilepath().GetBuffer(), node->GetName().GetBuffer(), connector->GetName().GetBuffer());
		return "";
	}
		
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
	LXStringA variableName = "var";

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

			LXStringA v = "const float4 var = $x;";
			_variableDeclarations.push_back(v);

			variableDeclaration = &_variableDeclarations.back();
			

			_addedNode.insert(node);
		}
	}

	if (node->Main == true)
	{
		// the main function is generated according the "Role", 

		switch (_shader)
		{
		case EShader::VertexShader: code = CreateVertexShaderEntryPoint(_layoutMask); break;
		//case EShader::HullShader: 	break;
		//case EShader::DomainShader: break;
		//case EShader::GeometryShader: 	break;
		case EShader::PixelShader: code = CreatePixelShaderEntryPoint(); break;
		default: CHK(0); break;
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
			if (propertyShader->GetValue())
			{
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
						userParameters += ",/*float*/ $" + connector->GetUID()->ToStringA();
					}
					else if (connector->Type == EConnectorType::Float2)
					{
						userParameters += ",/*float2*/ $" + connector->GetUID()->ToStringA();
					}
					else if (connector->Type == EConnectorType::Float3)
					{
						userParameters += ",/*float3*/ $" + connector->GetUID()->ToStringA();
					}
					else if (connector->Type == EConnectorType::Float4)
					{
						userParameters += ",/*float4*/ $" + connector->GetUID()->ToStringA();
					}
					else if (connector->Type == EConnectorType::Texture2D)
					{
						userParameters += ",/*Texture2D*/ $" + connector->GetUID()->ToStringA();
					}
					else if (connector->Type == EConnectorType::SamplerState)
					{
						userParameters += ",/*SamplerState*/ $" + connector->GetUID()->ToStringA();
					}
					else
					{
						CHK(0);
					}
				}

				code.Replace("$CustomFunction", function);
				code.Replace("$Parameters", userParameters);
			}
			else
			{
				LogE(LXGraphMaterialToHLSLConverter, L"Missing shader file for CustomFunction.");
				code = "float4(0,0,0,0)";
			}
		}
	}

	// Try to replace $XXX by the inputs
	for (const LXConnector* connector : node->Inputs)
	{
		const LXStringA connectorName = "$" + connector->GetName().ToStringA();
		const LXStringA connectorUID = "$" + connector->GetUID()->ToStringA();

		int i = code.Find(connectorName);
		if (i == -1)
			i = code.Find(connectorUID);
		
		int j = std::string::npos;
		if (variableDeclaration)
		{
			j = variableDeclaration->Find(connectorName);
			if (j == -1)
				j = variableDeclaration->Find(connectorUID);
		}
		
		
		if (i == std::string::npos && j == std::string::npos)
		{
			// The connector is not used at all.
			continue;
		}

		// Connector can be used with the current shader
		if (node->Main)
		{
			if (!IsConnectorUsableInContext(connector))
			{
				continue;
			}
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
					connectorValue = ParseNode(materialD3D11, connectorSource, callerNode, renderPass);
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
					connectorValue = "input.TexCoord";
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
			else if (connector->Type == EConnectorType::Texture2D)
			{
				connectorValue = "emptyTexture";
				FoundUnboundTexture = true;
			}
			else if (connector->Type == EConnectorType::SamplerState)
			{
				switch (_shader)
				{
				case EShader::VertexShader: connectorValue = "sampler" + LXStringA::Number(_HLSLVSTextureIndex);  break;
				case EShader::PixelShader: connectorValue = "sampler" + LXStringA::Number(_HLSLPSTextureIndex); break;
				default: CHK(0); break;
				}
			}
			else
			{
				CHK(0);
			}
		}

		CHK(!connectorValue.IsEmpty());
		code.Replace(connectorName, connectorValue);
		code.Replace(connectorUID, connectorValue);

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

		if (i == std::string::npos)
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

		// Try to retrieve the texture using the MaterialBase to get the override value.
		const LXProperty* property = _material->GetProperty(node.GetName());

		if (!property)
		{
			// Retrieve the UserProperty where is stored the "Variable"
			property = node.GetProperty(L"Value");
		}
		

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
		LXStringA connectorName = "$" + connector->GetUID()->ToStringA();

		// Input is unique
		CHK(connector->Connections.size() <= 1);

		if (connector->Connections.size() == 1)
		{
			const LXConnection* connection = connector->Connections.back();
			if (connection->Source != nullptr)
			{

				// Connector can be used with the current shader
				if (node.Main)
				{
					if (!IsConnectorUsableInContext(connector))
					{
						continue;
					}
				}

				res &= ParseNodeCB(*connection->Source->GetOwner(), outConstantBuffer);
			}
		}
	}

	return res;
}

void LXGraphMaterialToHLSLConverter::ParseNodeTexture(const LXNode& node, std::list<LXTexture*>& outTextures)
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

		// Try to retrieve the texture using the MaterialBase to get the override value.
		const LXProperty* property = _material->GetProperty(node.GetName());

		if (!property)
		{
			// Retrieve the UserProperty where is stored the "Variable"
			property = node.GetProperty(L"Value");
		}

		if (property->GetType() == EPropertyType::AssetPtr)
		{
			// Retrieve the texture using the MaterialBase to get the override value.
			const LXProperty* propertyTEMP = _material->GetProperty(node.GetName());
			LXPropertyAssetPtr* propertyAssetPtr = (LXPropertyAssetPtr*)propertyTEMP;
			LXTexture* texture = dynamic_cast<LXTexture*>(propertyAssetPtr->GetValue().get());
			if (!texture)
				texture = GetAssetManager()->GetDefaultTexture().get();

			if (texture)
			{
				
				int index = -1;
				int i = 0;

				for (const LXTexture* itTexture : outTextures)
				{
					if (itTexture == texture)
					{
						index = i;
						break;
					}
					i++;
				}

				if (index == -1)
				{
					outTextures.push_back(texture);
				}
			}
		}
		
		return;
	}

	//
	// Function
	//

	for (const LXConnector* connector : node.Inputs)
	{
		LXStringA connectorName = "$" + connector->GetUID()->ToStringA();

		// Input is unique
		CHK(connector->Connections.size() <= 1);

		if (connector->Connections.size() == 1)
		{
			const LXConnection* connection = connector->Connections.back();
			if (connection->Source != nullptr)
			{

				// Connector can be used with the current shader
				if (node.Main)
				{
					if (!IsConnectorUsableInContext(connector))
					{
						continue;
					}
				}

				ParseNodeTexture(*connection->Source->GetOwner(), outTextures);
			}
		}
	}
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
		// Retrieve the texture using the MaterialBase to get the override value.
		const LXProperty* propertyTEMP = _material->GetProperty(node->GetName());
		LXPropertyAssetPtr* propertyAssetPtr = (LXPropertyAssetPtr*)propertyTEMP;
		LXTexture* texture = dynamic_cast<LXTexture*>(propertyAssetPtr->GetValue().get());
		if (!texture)
			texture = GetAssetManager()->GetDefaultTexture().get();

		if (texture)
		{
			std::list<LXTexture*>* listTextures = nullptr;
			switch (_shader)
			{
			case EShader::VertexShader:
				listTextures = const_cast<std::list<LXTexture*>*>(&materialD3D11->GetTexturesVS()); break;
			case EShader::PixelShader:
				listTextures = const_cast<std::list<LXTexture*>*>(&materialD3D11->GetTexturesPS()); break;
			default: CHK(0);
			}

			if (listTextures)
			{
				int index = -1;
				int i = 0;

				for (const LXTexture* itTexture : *listTextures)
				{
					if (itTexture == texture)
					{
						index = i;
						break;
					}
					i++;
				}

				if (index == -1)
				{
					index = (int)listTextures->size() - 1;
				}
				
				switch (_shader)
				{
				case EShader::VertexShader: _HLSLVSTextureIndex = index; break;
				case EShader::PixelShader: _HLSLPSTextureIndex = index; break;
				default: CHK(0);
				}

				return "texture" + LXStringA::Number(index);
			}
		}
	}
	
	CHK(0);
	return "";
}

LXStringA LXGraphMaterialToHLSLConverter::ParseNodeConstant(const LXNode* node)
{
	// Retrieve the UserProperty where is stored the constant value;
	ListProperties properties;
	node->GetUserProperties(properties);

	// Constant node must contain only one user property to store the value.
	LX_CHK_RETV(properties.size() == 1, "");
		
	const LXProperty* property = *properties.begin();
	
	
	if (property->GetType() == EPropertyType::Float)
	{
		const LXPropertyFloat* propertyFloat =  static_cast<const LXPropertyFloat*>(property);
		float value = propertyFloat->GetValue();
		return LXStringA::Format("%f", value);
	}
	else if (property->GetType() == EPropertyType::Float2)
	{
		const LXPropertyVec2f* propertyFloat2 = static_cast<const LXPropertyVec2f*>(property);
		vec2f value = propertyFloat2->GetValue();
		return LXStringA::Format("float2(%f,%f)", value.x, value.y);
	}
	else if (property->GetType() == EPropertyType::Float3)
	{
		const LXPropertyVec3f* propertyFloat3 = static_cast<const LXPropertyVec3f*>(property);
		vec3f value = propertyFloat3->GetValue();
		return LXStringA::Format("float3(%f, %f, %f)", value.x, value.y, value.z);
	}
	else if (property->GetType() == EPropertyType::Float4)
	{
		const LXPropertyVec4f* propertyFloat4 = static_cast<const LXPropertyVec4f*>(property);
		vec4f value = propertyFloat4->GetValue();
		return LXStringA::Format("float3(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
	}
	else
	{
		CHK(0);
		return "";
	}
}

bool LXGraphMaterialToHLSLConverter::IsConnectorUsableInContext(const LXConnector* connector)
{
	switch (_shader)
	{
	case EShader::VertexShader:
		return connector->GetName() == L"Displacement";
		break;

	case EShader::PixelShader:
		return (connector->GetName() == L"Albedo" ||
			connector->GetName() == L"Opacity" ||
			connector->GetName() == L"Normal" ||
			connector->GetName() == L"Metal" ||
			connector->GetName() == L"Rough" ||
			connector->GetName() == L"Emissive");
		break;

	default:
		CHK(0);
		return false;
		break;
	}
}


LXStringA LXGraphMaterialToHLSLConverter::CreateVertexShaderEntryPoint(int LayoutMask)
{
	LXStringA code;

	if (_renderPass == ERenderPass::RenderToTexture)
	{
		if (LayoutMask == (int)EPrimitiveLayout::PT)
		{
			code =
				"//--------------------------------------------------------------------------------------\n"
				"// Vertex Shader - RenderToTexture - 'PT' layout \n"
				"//--------------------------------------------------------------------------------------\n"
				"VS_OUTPUT_PT VS(VS_INPUT_PT input)\n"
				"{\n"
				"	VS_OUTPUT_PT vertex = (VS_OUTPUT_PT)0;\n"
				"	vertex.Pos = input.Pos;\n"
				"	vertex.TexCoord = input.TexCoord;\n"
				"	//VARIABLES\n"
				"	return vertex;\n"
				"}\n";

			return code;
		}
		else
		{

		}
	
	}


	if (LayoutMask == (int)EPrimitiveLayout::P)
	{
		code = 
			"//--------------------------------------------------------------------------------------\n"
			"// Vertex Shader - GBuffer - 'P' layout \n"
			"//--------------------------------------------------------------------------------------\n"
			"VS_OUTPUT VS(VS_INPUT_P input)\n"
			"{\n"
			"	VS_VERTEX vertex = (VS_VERTEX)0;\n"
			"	vertex.Pos = input.Pos;\n"
			"	vertex.Normal = float3(0.0, 0.0, 1.0);\n"
			"	vertex.Tangent = float3(1.0, 0.0, 0.0);\n"
			"	vertex.Binormal = float3(0.0, 1.0, 0.0);\n"
			"	vertex.TexCoord = float2(0.0, 0.0);\n"
			"	vertex.InstanceID = 0;\n"
			"	vertex.InstancePos = float3(0.0, 0.0, 0.0);\n"
			"	//VARIABLES\n"
			"	return ComputeVertex(vertex, $Displacement);\n"
			"}\n";
	}
	else if (LayoutMask == (int)EPrimitiveLayout::PT)
	{
		code =
			"//--------------------------------------------------------------------------------------\n"
			"// Vertex Shader - GBuffer - 'PT' layout \n"
			"//--------------------------------------------------------------------------------------\n"
			"VS_OUTPUT VS(VS_INPUT_PT input)\n"
			"{\n"
			"	VS_VERTEX vertex = (VS_VERTEX)0;\n"
			"	vertex.Pos = input.Pos;\n"
			"	vertex.Normal = float3(0.0, 0.0, 1.0);\n"
			"	vertex.Tangent = float3(1.0, 0.0, 0.0);\n"
			"	vertex.Binormal = float3(0.0, 1.0, 0.0);\n"
			"	vertex.TexCoord = input.TexCoord;\n"
			"	vertex.InstanceID = 0;\n"
			"	vertex.InstancePos = float3(0.0, 0.0, 0.0);\n"
			"	//VARIABLES\n"
			"	return ComputeVertex(vertex, $Displacement);\n"
			"}\n";
	}
	else if (LayoutMask == (int)EPrimitiveLayout::PN)
	{
		code =
			"//--------------------------------------------------------------------------------------\n"
			"// Vertex Shader - GBuffer - 'PN' layout \n"
			"//--------------------------------------------------------------------------------------\n"
			"VS_OUTPUT VS(VS_INPUT_PN input)\n"
			"{\n"
			"	VS_VERTEX vertex = (VS_VERTEX)0;\n"
			"	vertex.Pos = input.Pos;\n"
			"	vertex.Normal = input.Normal; \n"
			"	vertex.Tangent = float3(1.0, 0.0, 0.0);\n"
			"	vertex.Binormal = float3(0.0, 1.0, 0.0);\n"
			"	vertex.TexCoord = float2(0.0, 0.0);\n"
			"	vertex.InstanceID = 0;\n"
			"	vertex.InstancePos = float3(0.0, 0.0, 0.0);\n"
			"	//VARIABLES\n"
			"	return ComputeVertex(vertex, $Displacement);\n"
			"}\n";
	}
	else if (LayoutMask == (int)EPrimitiveLayout::PNT)
	{
		code =
			"//--------------------------------------------------------------------------------------\n"
			"// Vertex Shader - GBuffer - 'PNT' layout \n"
			"//--------------------------------------------------------------------------------------\n"
			"VS_OUTPUT VS(VS_INPUT_PNT input)\n"
			"{\n"
			"	VS_VERTEX vertex = (VS_VERTEX)0;\n"
			"	vertex.Pos = input.Pos;\n"
			"	vertex.Normal = input.Normal; \n"
			"	vertex.TexCoord = input.TexCoord;\n"
			"	vertex.Tangent = float3(1.0, 0.0, 0.0);\n"
			"	vertex.Binormal = float3(0.0, 1.0, 0.0);\n"
			"	vertex.InstanceID = 0;\n"
			"	vertex.InstancePos = float3(0.0, 0.0, 0.0);\n"
			"	//VARIABLES\n"
			"	return ComputeVertex(vertex, $Displacement);\n"
			"}\n";
	}
	else if (LayoutMask == (int)EPrimitiveLayout::PNABT)
	{
		code =
			"//--------------------------------------------------------------------------------------\n"
			"// Vertex Shader - GBuffer - 'PNABT' layout \n"
			"//--------------------------------------------------------------------------------------\n"
			"VS_OUTPUT VS(VS_INPUT_PNABT input)\n"
			"{\n"
			"	VS_VERTEX vertex = (VS_VERTEX)0;\n"
			"	vertex.Pos = input.Pos; \n"
			"	vertex.Normal = input.Normal; \n"
			"	vertex.Tangent = input.Tangent;\n"
			"	vertex.Binormal = input.Binormal;\n"
			"	vertex.TexCoord = input.TexCoord;\n"
			"	vertex.SupportNormalMap = true;\n"
			"	vertex.InstanceID = 0;\n"
			"	vertex.InstancePos = float3(0.0, 0.0, 0.0);\n"
			"	//VARIABLES\n"
			"	return ComputeVertex(vertex, $Displacement);\n"
			"}\n";
	}
	else if (LayoutMask == (int)EPrimitiveLayout::PNABTI)
	{
		code =
			"//--------------------------------------------------------------------------------------\n"
			"// Vertex Shader - GBuffer - 'PNABTI' layout \n"
			"//--------------------------------------------------------------------------------------\n"
			"VS_OUTPUT VS(VS_INPUT_PNABTI input, uint instanceID : SV_InstanceID)\n"
			"{\n"
			"	VS_VERTEX vertex = (VS_VERTEX)0;\n"
			"	vertex.Pos = input.Pos;\n"
			"	vertex.Normal = input.Normal; \n"
			"	vertex.Tangent = input.Tangent;\n"
			"	vertex.Binormal = input.Binormal;\n"
			"	vertex.TexCoord = input.TexCoord;\n"
			"	vertex.SupportNormalMap = true;\n"
			"	vertex.InstanceID = instanceID;\n"
			"	vertex.InstancePos = input.InstancePos;\n"
			"	//VARIABLES\n"
			"	return ComputeVertex(vertex, $Displacement);\n"
			"}\n";
	}
	else
	{
		// Unsupported layout 
		LXString Layouts = L"Unsupported layout:";
		if (LayoutMask & LX_PRIMITIVE_INDICES) { Layouts += L" LX_PRIMITIVE_INDICES"; }
		if (LayoutMask & LX_PRIMITIVE_POSITIONS) { Layouts += L" LX_PRIMITIVE_POSITIONS"; }
		if (LayoutMask & LX_PRIMITIVE_NORMALS) { Layouts += L" LX_PRIMITIVE_NORMALS"; }
		if (LayoutMask & LX_PRIMITIVE_TANGENTS) { Layouts += L" LX_PRIMITIVE_TANGENTS"; }
		if (LayoutMask & LX_PRIMITIVE_TEXCOORDS) { Layouts += L" LX_PRIMITIVE_TEXCOORDS"; }
		if (LayoutMask & LX_PRIMITIVE_BINORMALS) { Layouts += L" LX_PRIMITIVE_BINORMALS"; }
		if (LayoutMask & LX_PRIMITIVE_INSTANCEPOSITIONS) { Layouts += L" LX_PRIMITIVE_INSTANCEPOSITIONS"; }
		LogE(LXShaderManager, Layouts.GetBuffer());
		CHK(0);
	}

	return code;
}

LXStringA LXGraphMaterialToHLSLConverter::CreatePixelShaderEntryPoint()
{
	LXStringA code;
	
	const LXMaterial* material = LXMaterialUtility::GetMaterialFromBase(_material);

	if (_renderPass == ERenderPass::GBuffer)
	{
		if (material->GetLightingModel() == EMaterialLightingModel::Unlit)
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
	else if ((_renderPass == ERenderPass::Depth) || (_renderPass == ERenderPass::Shadow))
	{
		code =
			"//--------------------------------------------------------------------------------------\n"
			"// Pixel Shader - Shadow, Depth\n"
			"//--------------------------------------------------------------------------------------\n"
			"void PS_DEPTHONLY(VS_OUTPUT input)\n"
			"{\n"
			"}\n";

	}
	else if (_renderPass == ERenderPass::Transparency)
	{
		if (material->GetLightingModel() == EMaterialLightingModel::Unlit)
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
	else if (_renderPass == ERenderPass::RenderToTexture)
	{
		code =
			"//--------------------------------------------------------------------------------------\n"
			"// Pixel Shader - RenderToTexture \n"
			"//--------------------------------------------------------------------------------------\n"
			"float4 PS(VS_OUTPUT_PT input, uint IsFrontFace : SV_IsFrontFace) : SV_Target\n"
			"{\n"
			"	//VARIABLES\n"
			"	return float4($Albedo, $Opacity);\n"
			"}\n";
	}
	else
	{
		CHK(0);
	}

	return code;
}
