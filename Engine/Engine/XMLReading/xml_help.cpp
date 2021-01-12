

#include "xml_help.h"

#include <cassert>

//-----------------------------------------------------------------------
// TO READ AND WRITE XML FILES
//-----------------------------------------------------------------------

testring TEXMLGetAttributeString( PXMLNode xmlNode, TELPCSTR cAttributName )
{
	assert( cAttributName != NULL );

	testring str;

	if( xmlNode.empty() )
		return str;

	TEXMLAttribute attribute;

	attribute = xmlNode.attribute( cAttributName );
	if( attribute.empty() )
		return str;

	str = attribute.value();

	// ищем локальное имя
//	TE3LocalManager::Get().GetLocalName( str.c_str(), &str );

	return str;
}


bool TEXMLGetAttributeStringF( PXMLNode xmlNode, TELPCSTR cAttributName, testring& strOut )
{
	assert( cAttributName != NULL );

	if( xmlNode.empty() )
	{
		strOut.clear();
		return false;
	}

	TEXMLAttribute attribute;

	attribute = xmlNode.attribute( cAttributName );
	if( attribute.empty() )
	{
		strOut.clear();
		return false;
	}

	strOut = attribute.value();

	return true;
}



//-----------------------------------------------------------------------
// bool
//-----------------------------------------------------------------------
bool TEXMLGetAttributeBool( PXMLNode xmlNode, TELPCSTR cAttributName, bool bDefault )
{
	if( xmlNode.empty() || cAttributName == NULL )
		return bDefault;

	TEXMLAttribute attribute;

	attribute = xmlNode.attribute( cAttributName );
	if( attribute.empty() || IsEqualString( attribute.value(), TEStr("") ) )
		return bDefault;
	
	return attribute.as_int() == 0 ? false : true;
}

//-----------------------------------------------------------------------
// int
//-----------------------------------------------------------------------
int TEXMLGetAttributeInt( PXMLNode xmlNode, TELPCSTR cAttributName, int iDefault )
{
	if( xmlNode.empty() || cAttributName == NULL )
		return iDefault;

	TEXMLAttribute attribute;

	attribute = xmlNode.attribute( cAttributName );
	if( attribute.empty() || IsEqualString( attribute.value(), TEStr("") ) )
		return iDefault;

	return attribute.as_int();
}

//-----------------------------------------------------------------------
// float
//-----------------------------------------------------------------------
float TEXMLGetAttributeFloat( PXMLNode xmlNode, TELPCSTR cAttributName, float Default )
{
	if( xmlNode.empty() || cAttributName == NULL )
		return Default;

	TEXMLAttribute attribute;

	attribute = xmlNode.attribute( cAttributName );
	if( attribute.empty() || IsEqualString( attribute.value(), TEStr("") ) )
		return Default;

	return attribute.as_float();
}



//-----------------------------------------------------------------------
// загрузить трехмерный вектор
//-----------------------------------------------------------------------
Vector3 TEXMLGetAttributesVector3( PXMLNode xmlNode )
{
	Vector3 v = Vector3( 0.0f, 0.0f, 0.0f );

	if( xmlNode.empty() )
		return v;

	TEXMLAttribute attribute;

	// по атрибутам
	attribute = xmlNode.attribute( TEStr("x") );
	v.x =  attribute.as_float();

	attribute = xmlNode.attribute( TEStr("y") );
	v.y =  attribute.as_float();

	attribute = xmlNode.attribute( TEStr("z") );
	v.z =  attribute.as_float();

	return v;
}


//-----------------------------------------------------------------------
// получить ребенка с номером
//-----------------------------------------------------------------------
TEXMLNode TEXMLGetElementChild(PXMLNode xmlNode, size_t sChildNum)
{
	assert(!xmlNode.empty());

	TEXMLNode xmlNodeEmpty;
	TEXMLNode xmlNodeChild = xmlNode.first_child();

	size_t i = -1;

	while (!xmlNodeChild.empty())
	{
		// пропускаем комментарии
		if (xmlNodeChild.type() == pugi::node_comment)
		{
			xmlNodeChild = xmlNodeChild.next_sibling();
			continue;
		}

		i++;

		if (i == sChildNum)
			return xmlNodeChild;

		xmlNodeChild = xmlNodeChild.next_sibling();
	}

	return xmlNodeEmpty;
}

//-----------------------------------------------------------------------
// получить число детей
//-----------------------------------------------------------------------
size_t TEXMLGetElementChildsCount(PXMLNode xmlNode)
{
	assert(!xmlNode.empty());

	size_t sCount = 0;
	TEXMLNode xmlNodeChild = xmlNode.first_child();

	while (!xmlNodeChild.empty())
	{
		sCount += xmlNodeChild.type() == pugi::node_comment ? 0 : 1;
		xmlNodeChild = xmlNodeChild.next_sibling();
	}

	return sCount;
}

//-----------------------------------------------------------------------
// получить число аттрибутов 
//-----------------------------------------------------------------------
size_t TEXMLGetElementAttributesCount(PXMLNode xmlNode)
{
	assert(!xmlNode.empty());

	size_t sCount = 0;
	TEXMLAttribute xmlAttribute = xmlNode.first_attribute();

	while (!xmlAttribute.empty())
	{
		++sCount;
		xmlAttribute = xmlAttribute.next_attribute();
	}

	return sCount;
}
