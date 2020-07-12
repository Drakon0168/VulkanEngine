#include "pch.h"
#include "Material.h"

#pragma region Constructor

Material::Material(char* vertexShaderPath, char* fragmentShaderPath)
{
    this->vertexShaderPath = vertexShaderPath;
    this->fragmentShaderPath = fragmentShaderPath;
}

#pragma endregion

#pragma region Accessors

char* Material::GetVertexShaderPath()
{
    return vertexShaderPath;
}

void Material::SetVertexShaderPath(char* value)
{
    vertexShaderPath = value;
}

char* Material::GetFragmentShaderPath()
{
    return fragmentShaderPath;
}

void Material::SetFragmentShaderPath(char* value)
{
    fragmentShaderPath = value;
}

#pragma endregion