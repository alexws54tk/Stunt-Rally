#include "pch.h"
#include "../ogre/common/Defines.h"
#include "OgreApp.h"
#include "../vdrift/pathmanager.h"

#include "../ogre/common/Gui_Def.h"
#include "../ogre/common/MultiList2.h"
#include "../ogre/common/Slider.h"
#include <boost/filesystem.hpp>

#include "../shiny/Main/Factory.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace MyGUI;
using namespace Ogre;


///  gui tweak page, material properties
//------------------------------------------------------------------------------------------------------------
const std::string sMtr = "Water_cyan";  //"Mud_orange";  //"Water_blue";  //combo, changable..

void App::CreateGUITweakMtr()
{
	ScrollView* view = mGUI->findWidget<ScrollView>("TweakView",false);
	if (!view)  return;
	
	// clear last view ..

	int y = 36;

	#define setOrigPos(widget) \
		widget->setUserString("origPosX", toStr(widget->getPosition().left)); \
		widget->setUserString("origPosY", toStr(widget->getPosition().top)); \
		widget->setUserString("origSizeX", toStr(widget->getSize().width)); \
		widget->setUserString("origSizeY", toStr(widget->getSize().height)); \
		widget->setUserString("RelativeTo", "OptionsWnd");

	sh::MaterialInstance* mat = mFactory->getMaterialInstance(sMtr);
	
	const sh::PropertyMap& props = mat->listProperties();
	for (sh::PropertyMap::const_iterator it = props.begin(); it != props.end(); ++it)
	{
		sh::PropertyValuePtr pv = (*it).second;
		std::string name = (*it).first;
		
		//  get type
		std::string sVal = pv->_getStringValue();
		//? if (boost::is_alnum(sVal))  continue;
		bool isStr = false;
		for (int c=0; c < sVal.length(); ++c)  if (sVal[c] >= 'a' && sVal[c] <= 'z')
			isStr = true;

		if (!isStr)
		{
			//  get size
			int size = -1;
			std::vector<std::string> tokens;
			boost::split(tokens, sVal, boost::is_any_of(" "));
			size = tokens.size();

			//LogO("PROP: " + name + "  val: " + sVal + "  type:" + toStr(type));
			const static char ch[6] = "rgbau";
			const static Colour clrsType[5] = {Colour(0.9,0.9,0.7),Colour(0.8,1.0,0.8),
						Colour(0.7,0.85,1.0),Colour(0.7,1.0,1.0),Colour(1.0,1.0,1.0)};

			//  for each component (xy,rgb..)
			for (int i=0; i < size; ++i)
			{
				String nameSi = name + ":" + toStr(size) + "." + toStr(i);  // size and id in name
				float val = boost::lexical_cast<float> (tokens[i]);
				int t = std::min(4,i);  const Colour& clr = clrsType[std::max(0,std::min(4,size-1))];

				//  name text
				int x = 0, xs = 150;
				TextBox* txt = view->createWidget<TextBox>("TextBox", x,y, xs,20, Align::Default, nameSi + ".txt");
				setOrigPos(txt);  txt->setTextColour(clr);
				txt->setCaption(size == 1 ? name : name + "." + ch[t]);

				//  val edit
				x += xs;  xs = 60;
				EditBox* edit = view->createWidget<EditBox>("EditBox", x,y, xs,20, Align::Default, nameSi + "E");
				setOrigPos(edit);  edit->setTextColour(clr);  edit->setColour(clr);
				edit->setCaption(fToStr(val,3,6));
				if (edit->eventEditTextChange.empty())  edit->eventEditTextChange += newDelegate(this, &App::edTweak);
				
				//  slider
				x += xs + 10;  xs = 400;
				Slider* sl = view->createWidget<Slider>("Slider", x,y-1, xs,19, Align::Default, nameSi);
				setOrigPos(sl);  sl->setColour(clr);
				sl->setValue(val);  //powf(val * 1.f/2.f, 1.f/2.f));  //v
				if (sl->eventValueChanged.empty())  sl->eventValueChanged += newDelegate(this, &App::slTweak);

				y += 22;
			}
			y += 8;
		}
	}
	view->setCanvasSize(1100, y+500);  //?..
	view->setCanvasAlign(Align::Default);
}

//  gui change val events
//-----------------------------------------------------------------
void App::slTweak(Slider* sl, float val)
{
	std::string name = sl->getName();

	EditBox* edit = mGUI->findWidget<EditBox>(name + "E");
	if (edit)
		edit->setCaption(fToStr(val,3,6));

	TweakSetMtrPar(name, val);
}

void App::edTweak(MyGUI::EditPtr ed)
{
	std::string name = ed->getName();  name = name.substr(0,name.length()-1);  // ends with E
	float val = s2r(ed->getCaption());
	
	Slider* sl = mGUI->findWidget<Slider>(name);
	if (sl)
		sl->setValue(val);

	TweakSetMtrPar(name, val);
}

///  change material property (float component)
//-----------------------------------------------------------------
void App::TweakSetMtrPar(std::string name, float val)
{
	std::string prop = name.substr(0,name.length()-4);  // cut ending, eg :2.1
	
	int id = -1, size = 1;
	if (name.substr(name.length()-2,1) == ".")  // more than 1 float
	{
		id = s2i(name.substr(name.length()-1,1));
		size = s2i(name.substr(name.length()-3,1));
	}
	//val = powf(val * 2.f, 2.f);  //v

	sh::MaterialInstance* mat = mFactory->getMaterialInstance(sMtr);
	if (id == -1)  // 1 float
		mat->setProperty(prop, sh::makeProperty<sh::FloatValue>(new sh::FloatValue(val)));
	else
	{
		sh::PropertyValuePtr& vp = mat->getProperty(prop);
		switch (size)
		{
			case 2:
			{	sh::Vector2 v = sh::retrieveValue<sh::Vector2>(vp,0);
				((float*)&v.mX)[id] = val;
				mat->setProperty(prop, sh::makeProperty<sh::Vector2>(new sh::Vector2(v.mX, v.mY)));
			}	break;
			case 3:
			{	sh::Vector3 v = sh::retrieveValue<sh::Vector3>(vp,0);
				((float*)&v.mX)[id] = val;
				mat->setProperty(prop, sh::makeProperty<sh::Vector3>(new sh::Vector3(v.mX, v.mY, v.mZ)));
			}	break;
			case 4:
			{	sh::Vector4 v = sh::retrieveValue<sh::Vector4>(vp,0);
				((float*)&v.mX)[id] = val;
				mat->setProperty(prop, sh::makeProperty<sh::Vector4>(new sh::Vector4(v.mX, v.mY, v.mZ, v.mW)));
			}	break;
		}
	}
}
