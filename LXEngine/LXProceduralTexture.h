//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXAsset.h"
#include "LXGraph.h"

class LXProceduralTexture : public LXAsset
{
public:

	LXProceduralTexture();
	virtual ~LXProceduralTexture();
};

//------------------------------------------------------------------------------------------------------
// GraphNodes
//------------------------------------------------------------------------------------------------------

// class LXGraphNodeOutput : public LXGraphNode
// {
// public:
// 	~LXGraphNodeOutput() {}
// };
// 
// class LXGraphNodeOutputBitmap : public LXGraphNodeOutput
// {
// public:
// 	LXGraphNodeOutputBitmap() { }
// 	~LXGraphNodeOutputBitmap() {}
// 	void LinkTo(LXGraphNode*) override;
// 
// 	// Input
// 	LXGraphNode* From = nullptr;
// };
// 
// class LXGraphNodeOutputTexture : public LXGraphNodeOutput
// {
// public:
// 	LXGraphNodeOutputTexture(LXTexture* InTexture) :Texture(InTexture) { }
// 	~LXGraphNodeOutputTexture() {}
// 	LXGraphNode* GetNodeIn();
// 	void LinkTo(LXGraphNode*) override;
// 
// 	// Input
// 	LXGraphNode* From = nullptr;
// 
// 	LXTexture* Texture;
// };

//------------------------------------------------------------------------------------------------------
// Misc
//------------------------------------------------------------------------------------------------------

// class LXGraphNodeMaterial : public LXGraphNode
// {
// public:
// 	LXGraphNodeMaterial(LXMaterial* InMaterial) :Material(InMaterial) { }
// 	void LinkTo(LXGraphNode*) override;
// 	LXMaterial* Material;
// 
// 	// Input (Binded to Color)
// 	LXGraphNode* From = nullptr;
// 
// 	// Outputs
// 	std::set<LXGraphNode*> To;
// 
// 	// Parameters
// 	int Iteration = 1;
// };

// class LXGraphNodeLoop : public LXGraphNode
// {
// public:
// 	LXGraphNodeLoop() { }
// 
// 	// Input
// 	LXGraphNode* From = nullptr;
// 	
// 	// Outputs
// 	std::set<LXGraphNode*> Body; 
// 	std::set<LXGraphNode*> Completed;
// 
// 	// Parameters
// 	int start = 0;
// 	int end = 1000;
// };

//------------------------------------------------------------------------------------------------------


// class LXGraphTexture : public LXGraph, public LXAsset
// {
// 
// public:
// 
// 	LXGraphTexture();
// 	virtual ~LXGraphTexture();
// 
// 	//
// 	// Overridden From LXResource
// 	//
// 
// 	bool			Load() override;
// 	LXString		GetFileExtension() override { return LX_PROCEDURALTEXTURE_EXT; }
// 
// 	virtual void AddLink(LXGraphNode* Node0, LXGraphNode* Node1) override;
// 	void AddOutputNode(LXGraphNodeOutput* Node) { Outputs.insert(Node); AddNode(Node); }
// 	const std::set<LXGraphNodeOutput*>& GetOutput2() const;
// 
// private:
// 
// 	std::set<LXGraphNodeOutput*> Outputs;
// };
// 
