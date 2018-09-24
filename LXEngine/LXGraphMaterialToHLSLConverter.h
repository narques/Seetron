//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once
#include "LXObject.h"

class LXGraph;
class LXNode;
class LXConstantBuffer;
class LXConnector;
class LXTextureD3D11;
class LXTexture;
class LXMaterialD3D11;
enum class ERenderPass;

class LXGraphMaterialToHLSLConverter : public LXObject
{

public:

	LXGraphMaterialToHLSLConverter();
	virtual ~LXGraphMaterialToHLSLConverter();

	LXStringA GenerateCode(const LXMaterialD3D11* materialD3D11, ERenderPass renderPass);
	static bool GenerateConstanBuffer(const LXGraph* graph, LXConstantBuffer& outConstantBuffer);
	bool GatherTextures(const LXGraph* graph, list<LXTextureD3D11*>& listTextures);

private:

	// 
	// connector : the output connector from where we enter in the node.
	//
	LXStringA ParseNode(const LXMaterialD3D11* materialD3D11, int outputConnectorIndex, const LXConnector* connector, const LXNode* node, ERenderPass renderPass);
	LXStringA ParseNodeVariable(const LXMaterialD3D11* materialD3D11, const LXNode* node);
	LXStringA ParseNodeConstant(const LXNode* node);
	static bool ParseNodeCB(const LXNode& node, LXConstantBuffer& outConstantBuffer);
	bool ParseNodeTexture(const LXNode& node, list<LXTextureD3D11*>& listTextures);
	LXStringA GenerateHeaderComment() const;

private:

	set<LXStringA> _includes;
	set<const LXNode*> _addedNode;
	set<const LXTexture*> _addedTextures;
	map<const LXNode*, int> _textureIndices;
	list<LXStringA> _variableDeclarations;

};

