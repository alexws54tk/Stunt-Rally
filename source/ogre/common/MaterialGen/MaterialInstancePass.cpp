#include "MaterialInstancePass.hpp"

namespace sh
{

	MaterialInstanceTextureUnit* MaterialInstancePass::createTextureUnit (const std::string& name)
	{
		mTexUnits [name] = MaterialInstanceTextureUnit(name);
		return &mTexUnits[name];
	}

	std::map <std::string, MaterialInstanceTextureUnit> MaterialInstancePass::getTexUnits ()
	{
		return mTexUnits;
	}
}