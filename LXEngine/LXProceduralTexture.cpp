//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXProceduralTexture.h"
#include "LXGraph.h"

LXProceduralTexture::LXProceduralTexture()
{
}

LXProceduralTexture::~LXProceduralTexture()
{
}

// LXGraphTexture::LXGraphTexture()
// {
// 	
// 
// 	//--------------------------------------------------------------------------------------------------------------------------------
// 	LXProperty::SetCurrentGroup(L"ProceduralTexture");
// 	//--------------------------------------------------------------------------------------------------------------------------------
// }
// 
// LXGraphTexture::~LXGraphTexture()
// {
// }
// 
// bool LXGraphTexture::Load()
// {
// 	return LoadWithMSXML(_filepath);
// }
// 
// void LXGraphTexture::AddLink(LXGraphNode* Node0, LXGraphNode* Node1)
// {
// 	Node0->LinkTo(Node1);
// }
// 
// const std::set<LXGraphNodeOutput*>& LXGraphTexture::GetOutput2() const
// {
// 	return Outputs;
// }
// 
// LXGraphNode* LXGraphNodeOutputTexture::GetNodeIn()
// {
// 	return From;
// }
// 
// void LXGraphNodeOutputTexture::LinkTo(LXGraphNode* Node)
// {
// 	CHK(0); // NodeOutputTexture doesn't have output !
// }
// 
// void LXGraphNodeMaterial::LinkTo(LXGraphNode* Node)
// {
// 	if (auto NodeTextureOutput = dynamic_cast<LXGraphNodeOutputTexture*>(Node))
// 	{
// 		To.insert(NodeTextureOutput);
// 		CHK(!NodeTextureOutput->From);
// 		NodeTextureOutput->From = this;
// 	}
// 	else if (auto NodeBitmapOutput = dynamic_cast<LXGraphNodeOutputBitmap*>(Node))
// 	{
// 		To.insert(NodeBitmapOutput);
// 		CHK(!NodeBitmapOutput->From);
// 		NodeBitmapOutput->From = this;
// 	}
// 	else if (auto NodeMaterial = dynamic_cast<LXGraphNodeMaterial*>(Node))
// 	{
// 		To.insert(NodeMaterial);
// 		CHK(!NodeMaterial->From);
// 		NodeMaterial->From = this;
// 	}
// 	else
// 	{
// 		CHK(0);
// 	}
// }
// 
// void LXGraphNodeOutputBitmap::LinkTo(LXGraphNode*)
// {
// 	CHK(0); // LXGraphNodeOutputBitmap< doesn't have output !
// }
