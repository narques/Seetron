//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#define LX_NODETYPE_GROUP	LX_BIT(0)
#define LX_NODETYPE_ACTOR	LX_BIT(1)
#define LX_NODETYPE_MESH	LX_BIT(2)
#define LX_NODETYPE_CAMERA	LX_BIT(3)
#define LX_NODETYPE_LIGHT	LX_BIT(4)
#define LX_NODETYPE_ANCHOR	LX_BIT(5)
#define LX_NODETYPE_CS		LX_BIT(6)
#define LX_NODETYPE_RENDERTOTTEXTURE LX_BIT(7)
#define LX_NODES_UI ( LX_NODETYPE_LIGHT | LX_NODETYPE_ANCHOR | LX_NODETYPE_CS )
