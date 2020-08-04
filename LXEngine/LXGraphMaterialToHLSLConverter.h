//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once
#include "LXObject.h"

class LXConstantBuffer;
class LXConnector;
class LXGraph;
class LXMaterialBase;
class LXMaterialD3D11;
class LXNode;
class LXTexture;

enum class ERenderPass;
enum class EShader;

class LXGraphMaterialToHLSLConverter : public LXObject
{

public:

	LXGraphMaterialToHLSLConverter(const LXMaterialBase* material);
	virtual ~LXGraphMaterialToHLSLConverter();

	LXStringA GenerateCode(const LXMaterialD3D11* materialD3D11, ERenderPass renderPass, EShader Shader, int layoutMask = -1);
	
	// Parse the GraphMaterial to gather the ConstantBuffers
	bool GenerateConstanBuffer(const LXGraph* graph, EShader Shader, LXConstantBuffer& outConstantBuffer);

	// Parse the GraphMaterial to gather the Textures
	void GatherTextures(const LXGraph* graph, EShader shader, list<LXTexture*>& outTextures);

private:

	// 
	// connector : the output connector from where we enter in the node.
	//
	LXStringA ParseNode(const LXMaterialD3D11* materialD3D11, int outputConnectorIndex, const LXConnector* connector, const LXNode* node, ERenderPass renderPass);
	LXStringA ParseNodeVariable(const LXMaterialD3D11* materialD3D11, const LXNode* node);
	LXStringA ParseNodeConstant(const LXNode* node);
	
	LXStringA CreateVertexShaderEntryPoint(int LayoutMask);
	LXStringA CreatePixelShaderEntryPoint();

	bool IsConnectorUsableInContext(const LXConnector* connector);

	bool ParseNodeCB(const LXNode& node, LXConstantBuffer& outConstantBuffer);
	void ParseNodeTexture(const LXNode& node, list<LXTexture*>& outTextures);

	LXStringA GenerateHeaderComment() const;

public:

	// When a texture node input is not linked.
	bool FoundUnboundTexture = false;
	
private:

	int _layoutMask = 0;
	const LXMaterialBase* _material = nullptr;
	ERenderPass _renderPass;
	EShader _shader;

	set<LXStringA> _includes;
	set<const LXNode*> _addedNode;
	
	list<LXStringA> _variableDeclarations;

	int _HLSLVSTextureIndex = 0;
	int _HLSLPSTextureIndex = 0;
};

