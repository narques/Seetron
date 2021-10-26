//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXShader.h"
#include "LXLogger.h"
#include "LXPlatform.h"
#include "LXFile.h"

LXShader::LXShader()
{
}

LXShader::~LXShader()
{
}

bool LXShader::Load()
{
	State = EResourceState::LXResourceState_Loaded;
	return true;
}

bool LXShader::Save()
{
	LogI(Shader, L"Shader can't be open/save as SmartObject. It's just a text file");
	return true;
}

void LXShader::SaveDefault()
{
	LXStringA ShaderBuffer;

	ShaderBuffer += "#include \"common.hlsl\"\n";
	ShaderBuffer += "\n";
	ShaderBuffer += "//--------------------------------------------------------------------------------------\n";
	ShaderBuffer += "// Pixel Shader\n";
	ShaderBuffer += "//--------------------------------------------------------------------------------------\n";
	ShaderBuffer += "\n";
	ShaderBuffer += "PS_OUTPUT PS(VS_OUTPUT input)\n";
	ShaderBuffer += "{\n";
	ShaderBuffer += "	PS_OUTPUT output = (PS_OUTPUT)0;\n";
	ShaderBuffer += "	output.OutColor0 = input.Norm;\n";
	ShaderBuffer += "	output.OutColor1 = float4(1.0, 1.0, 1.0, 0.0);\n";
	ShaderBuffer += "	return output;\n";
	ShaderBuffer += "}\n";
	
	LXPlatform::DeleteFile(_filepath);

	LXFile File;
	if (File.Open(_filepath, "wt") == true)
	{
		File.Write(ShaderBuffer.GetBuffer(), ShaderBuffer.size());
		File.Close();
	}
	else
	{
		LogE(Shader, L"Unable to open shader file: %s", _filepath.GetBuffer());
	}
}

