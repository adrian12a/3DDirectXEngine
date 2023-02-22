#pragma once
#include <DirectXMath.h>

struct ConstantBufferStruct
{
	DirectX::XMMATRIX MVP;
	DirectX::XMMATRIX worldMVP;
};