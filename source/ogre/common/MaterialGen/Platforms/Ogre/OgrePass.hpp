#ifndef SH_OGREPASS_H
#define SH_OGREPASS_H

#include <OgrePass.h>

#include "../../Platform.hpp"

namespace sh
{
	class OgreMaterial;

	class OgrePass : public Pass
	{
	public:
		OgrePass (OgreMaterial* parent);

		virtual boost::shared_ptr<TextureUnitState> createTextureUnitState ();
		virtual void assignVertexProgram (const VertexProgram& program);
		virtual void assignFragmentProgram (const FragmentProgram& program);
		virtual void assignGeometryProgram(const GeometryProgram& program);

		Ogre::Pass* getOgrePass();

	private:
		Ogre::Pass* mPass;

	protected:
		virtual bool setPropertyOverride (const std::string &name, PropertyValuePtr& value);
	};
}

#endif