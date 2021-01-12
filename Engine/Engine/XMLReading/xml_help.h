
#pragma once

#include "string_help.h"
#include "SimpleMath.h"
#include "pugixml.hpp"

typedef pugi::xml_document	TEXMLDoc;
typedef pugi::xml_node		TEXMLNode;
typedef pugi::xml_node&		PXMLNode;

typedef pugi::xml_attribute	TEXMLAttribute;

using namespace DirectX::SimpleMath;
//-----------------------------------------------------------------------
// TO READ AND WRITE XML FILES
// using PugiXML
//-----------------------------------------------------------------------

	// string load
	extern __forceinline testring	TEXMLGetAttributeString( PXMLNode xmlNode, TELPCSTR cAttributeName );
	extern __forceinline bool		TEXMLGetAttributeStringF( PXMLNode xmlNode, TELPCSTR cAttributeName, testring& strOut );

	// bool
	extern __forceinline bool		TEXMLGetAttributeBool( PXMLNode xmlNode, TELPCSTR cAttributName, bool bDefault = false );
	// int
	extern __forceinline int		TEXMLGetAttributeInt( PXMLNode xmlNode, TELPCSTR cAttributName, int Default = -1 );
	
	// float
	extern __forceinline float		TEXMLGetAttributeFloat( PXMLNode xmlNode, TELPCSTR cAttributName, float Default = -1.0f );

	// vector3
	extern __forceinline Vector3	TEXMLGetAttributesVector3( PXMLNode xmlNode );
	

	// получить ребенка с номером
	extern __forceinline TEXMLNode		TEXMLGetElementChild(PXMLNode xmlNode, size_t sChildIndex);
	// получить число детей
	extern __forceinline size_t			TEXMLGetElementChildsCount(PXMLNode xmlNode);
	// получить число аттрибутов 
	extern __forceinline size_t			TEXMLGetElementAttributesCount(PXMLNode xmlNode);

