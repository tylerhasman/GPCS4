#pragma once

#include "GnmCommon.h"
#include "GnmShaderMeta.h"

#include "../Pssl/PsslEnums.h"
#include "../Pssl/PsslShaderStructure.h"

namespace gve
{;
class GveShader;
class GveImageView;
class GveSharpResourceManager;
}  // namespace gve


struct GnmShaderContext
{
	const void*                           code   = nullptr;
	RcPtr<gve::GveShader>                 shader = nullptr;
	std::vector<pssl::PsslShaderResource> userDataSlotTable;
	GnmShaderMeta                         meta;
};
