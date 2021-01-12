#include "AIObjectManager.h"
#include "../XMLReading/xml_help.h"
#include "../BTActions.h"


SPBlackboard AIObjectManager::CreateBlackboard(std::wstring FileName)
{
	SPBlackboard Blackboard = std::make_shared<BrainTree::Blackboard>();

	std::wstring FullPath = DataPath;
	FullPath.append(FileName);

	pugi::xml_document XMLDoc;

	// загружаем файл
	pugi::xml_parse_result result = XMLDoc.load_file(FullPath.c_str());

	if (result.status != pugi::status_ok)
	{
		return nullptr;
	}

	// выходим на корень
	TEXMLNode xmlNodeRoot = XMLDoc.child(TEStr("Blackboard"));

	if (xmlNodeRoot.empty())
		return nullptr;

	char* c = new char[50];
	testring Name;

	// BOOLS 

	// выходим на корень
	auto xmlNodeBools = xmlNodeRoot.child(TEStr("Bools"));

	if (xmlNodeBools.empty())
		return nullptr;

	// загружаем переменные
	pugi::xml_node xmlNode = xmlNodeBools.first_child();

	while (!xmlNode.empty())
	{
		// пропускаем комментарии
		if (xmlNode.type() == pugi::node_comment)
		{
			xmlNode = xmlNode.next_sibling();
			continue;
		}

		bool Value;

		TEXMLGetAttributeStringF(xmlNode, TEStr("name"), Name);
		Value = TEXMLGetAttributeBool(xmlNode, TEStr("value"), false);

		testringToMBCS(Name.c_str(), c, sizeof(Name));
		
		Blackboard->setBool(c, Value);
		// следующий
		xmlNode = xmlNode.next_sibling();
	}

	// INTS 

	// выходим на корень
	auto xmlNodeInts = xmlNodeRoot.child(TEStr("Ints"));

	if (xmlNodeInts.empty())
		return nullptr;

	// загружаем переменные
	xmlNode = xmlNodeInts.first_child();

	while (!xmlNode.empty())
	{
		// пропускаем комментарии
		if (xmlNode.type() == pugi::node_comment)
		{
			xmlNode = xmlNode.next_sibling();
			continue;
		}

		int Value;

		TEXMLGetAttributeStringF(xmlNode, TEStr("name"), Name);
		Value = TEXMLGetAttributeInt(xmlNode, TEStr("value"), 0);

		testringToMBCS(Name.c_str(), c, sizeof(Name));
		Blackboard->setInt(c, Value);

		// следующий
		xmlNode = xmlNode.next_sibling();
	}

	// FLOATS 

	// выходим на корень
	auto xmlNodeFloats = xmlNodeRoot.child(TEStr("Floats"));

	if (xmlNodeFloats.empty())
		return nullptr;

	// загружаем переменные
	xmlNode = xmlNodeFloats.first_child();

	while (!xmlNode.empty())
	{
		// пропускаем комментарии
		if (xmlNode.type() == pugi::node_comment)
		{
			xmlNode = xmlNode.next_sibling();
			continue;
		}

		float Value;

		TEXMLGetAttributeStringF(xmlNode, TEStr("name"), Name);
		Value = TEXMLGetAttributeFloat(xmlNode, TEStr("value"), 0.0f);

		testringToMBCS(Name.c_str(), c, sizeof(Name));
		Blackboard->setFloat(c, Value);

		// следующий
		xmlNode = xmlNode.next_sibling();
	}

	// STRINGS 

	// выходим на корень
	auto xmlNodeStrings = xmlNodeRoot.child(TEStr("Strings"));

	if (xmlNodeStrings.empty())
		return nullptr;

	// загружаем переменные
	xmlNode = xmlNodeStrings.first_child();

	

	while (!xmlNode.empty())
	{
		// пропускаем комментарии
		if (xmlNode.type() == pugi::node_comment)
		{
			xmlNode = xmlNode.next_sibling();
			continue;
		}

		char* c_Value = new char[50];
		testring Value;

		TEXMLGetAttributeStringF(xmlNode, TEStr("name"), Name);
		TEXMLGetAttributeStringF(xmlNode, TEStr("value"), Value);
		testringToMBCS(Name.c_str(), c, sizeof(Name));
		testringToMBCS(Value.c_str(), c_Value, sizeof(Value));
		Blackboard->setString(c, c_Value);

		delete[] c_Value;

		// следующий
		xmlNode = xmlNode.next_sibling();
	}

	

	// VECTORS 

	// выходим на корень
	auto xmlNodeVectors = xmlNodeRoot.child(TEStr("Vectors"));

	if (xmlNodeVectors.empty())
		return nullptr;

	// загружаем переменные
	xmlNode = xmlNodeVectors.first_child();

	while (!xmlNode.empty())
	{
		// пропускаем комментарии
		if (xmlNode.type() == pugi::node_comment)
		{
			xmlNode = xmlNode.next_sibling();
			continue;
		}

		Vector3 Value;

		TEXMLGetAttributeStringF(xmlNode, TEStr("name"), Name);
		Value = TEXMLGetAttributesVector3(xmlNode);

		testringToMBCS(Name.c_str(), c, sizeof(Name));
		Blackboard->setVector(c, Value);

		// следующий
		xmlNode = xmlNode.next_sibling();
	}

	delete[] c;

	return Blackboard;
}

SPBehaviorTree AIObjectManager::CreateBehaviorTree(std::wstring FileName, AIAgent* owner)
{
	SPBehaviorTree Tree = std::make_shared<BrainTree::BehaviorTree>();

	std::unordered_map<int, SPNode> tempAssosiation;

	std::wstring FullPath = DataPath;
	FullPath.append(FileName);

	pugi::xml_document XMLDoc;

	// загружаем файл
	pugi::xml_parse_result result = XMLDoc.load_file(FullPath.c_str());

	if (result.status != pugi::status_ok)
	{
		return nullptr;
	}

	// выходим на корень
	TEXMLNode xmlNodeRoot = XMLDoc.child(TEStr("BehaviorTree"));

	if (xmlNodeRoot.empty())
		return nullptr;

	// создаем бб

	testring BBFilename;
	TEXMLGetAttributeStringF(xmlNodeRoot, TEStr("BlackboardFilename"), BBFilename);
	
	SPBlackboard BB = CreateBlackboard(BBFilename);
	Tree->blackboard = BB;

	pugi::xml_node xmlNode = xmlNodeRoot.first_child();

	// создаем объекты нодов
	while (!xmlNode.empty())
	{
		// пропускаем комментарии
		if (xmlNode.type() == pugi::node_comment)
		{
			xmlNode = xmlNode.next_sibling();
			continue;
		}

		int id;
		id = TEXMLGetAttributeInt(xmlNode, TEStr("id"), 0);

		testring Name;
		TEXMLGetAttributeStringF(xmlNode, TEStr("Name"), Name);

		if (Name == L"Sequence")
		{
			std::shared_ptr<BrainTree::Sequence> Sequence = std::make_shared<BrainTree::Sequence>();
			tempAssosiation[id] = Sequence;
			xmlNode = xmlNode.next_sibling();
			continue;
		}

		if (Name == L"Selector")
		{
			std::shared_ptr<BrainTree::Selector> Selector = std::make_shared<BrainTree::Selector>();
			tempAssosiation[id] = Selector;
			xmlNode = xmlNode.next_sibling();
			continue;
		}

		if (Name == L"ParallelSequence")
		{
			std::shared_ptr<BrainTree::ParallelSequence> ParallelSequence = std::make_shared<BrainTree::ParallelSequence>();
			tempAssosiation[id] = ParallelSequence;
			xmlNode = xmlNode.next_sibling();
			continue;
		}

		if (Name == L"Inverter")
		{
			std::shared_ptr<BrainTree::Inverter> Inverter = std::make_shared<BrainTree::Inverter>();
			tempAssosiation[id] = Inverter;
			xmlNode = xmlNode.next_sibling();
			continue;
		}

		if (Name == L"Repeater")
		{
			int limit = TEXMLGetAttributeInt(xmlNode, TEStr("limit"), 0);

			std::shared_ptr<BrainTree::Repeater> Repeater = std::make_shared<BrainTree::Repeater>(limit);
			tempAssosiation[id] = Repeater;
			xmlNode = xmlNode.next_sibling();
			continue;
		}

		if (Name == L"UntilSuccess")
		{
			std::shared_ptr<BrainTree::UntilSuccess> UntilSuccess = std::make_shared<BrainTree::UntilSuccess>();
			tempAssosiation[id] = UntilSuccess;
			xmlNode = xmlNode.next_sibling();
			continue;
		}

		if (Name == L"UntilFailure")
		{
			std::shared_ptr<BrainTree::UntilFailure> UntilFailure = std::make_shared<BrainTree::UntilFailure>();
			tempAssosiation[id] = UntilFailure;
			xmlNode = xmlNode.next_sibling();
			continue;
		}

		if (Name == L"MoveTo")
		{
			std::shared_ptr<MoveTo> Action = std::make_shared<MoveTo>();
			Action->blackboard = BB;
			Action->owner = owner;
			tempAssosiation[id] = Action;

			testring BBPositionName;
			char* c_Value = new char[50];
			TEXMLGetAttributeStringF(xmlNode, TEStr("BBPositionName"), BBPositionName);
			testringToMBCS(BBPositionName.c_str(), c_Value, sizeof(BBPositionName));

			Action->BBPositionName = c_Value;
			
			delete[] c_Value;

			xmlNode = xmlNode.next_sibling();
			continue;
		}

		if (Name == L"MoveOut")
		{
			std::shared_ptr<MoveOut> Action = std::make_shared<MoveOut>();
			Action->blackboard = BB;
			Action->owner = owner;
			tempAssosiation[id] = Action;

			xmlNode = xmlNode.next_sibling();
			continue;
		}

		if (Name == L"RandomPosition")
		{
			std::shared_ptr<RandomPosition> Action = std::make_shared<RandomPosition>();
			Action->blackboard = BB;
			Action->owner = owner;
			tempAssosiation[id] = Action;

			testring BBPositionName;
			char* c_Value = new char[50];
			TEXMLGetAttributeStringF(xmlNode, TEStr("BBPositionName"), BBPositionName);
			testringToMBCS(BBPositionName.c_str(), c_Value, sizeof(BBPositionName));

			Action->BBPositionName = c_Value;

			delete[] c_Value;

			float val = TEXMLGetAttributeFloat(xmlNode, TEStr("radius"));

			Action->radius = val;

			xmlNode = xmlNode.next_sibling();
			continue;
		}

		if (Name == L"Wait")
		{
			std::shared_ptr<Wait> Action = std::make_shared<Wait>();
			Action->blackboard = BB;
			Action->owner = owner;
			tempAssosiation[id] = Action;

			xmlNode = xmlNode.next_sibling();
			continue;
		}

		if (Name == L"CheckForHummer")
		{
			std::shared_ptr<CheckForHummer> Action = std::make_shared<CheckForHummer>();
			Action->owner = owner;
			tempAssosiation[id] = Action;

			xmlNode = xmlNode.next_sibling();
			continue;
		}

		if (Name == L"Interrupt")
		{
			std::shared_ptr<Interrupt> Action = std::make_shared<Interrupt>();
			Action->BB = BB;

			testring BBBoolName;
			char* c_Value = new char[50];
			TEXMLGetAttributeStringF(xmlNode, TEStr("BBBoolName"), BBBoolName);
			testringToMBCS(BBBoolName.c_str(), c_Value, sizeof(BBBoolName));

			Action->BBBoolName = c_Value;

			delete[] c_Value;

			tempAssosiation[id] = Action;

			xmlNode = xmlNode.next_sibling();
			continue;
		}

		// следующий
		xmlNode = xmlNode.next_sibling();
	}

	xmlNode = xmlNodeRoot.first_child();

	// связываем детей и родителей
	while (!xmlNode.empty())
	{
		// пропускаем комментарии
		if (xmlNode.type() == pugi::node_comment)
		{
			xmlNode = xmlNode.next_sibling();
			continue;
		}

		int id;
		id = TEXMLGetAttributeInt(xmlNode, TEStr("id"), 0);

		testring Name;
		TEXMLGetAttributeStringF(xmlNode, TEStr("Name"), Name);

		// Composites
		if (Name == L"Sequence" || Name == L"Selector" || Name == L"ParallelSequence")
		{
			SPNode Node = tempAssosiation[id];
			BrainTree::Composite* Composite = (BrainTree::Composite*)Node.get();

			pugi::xml_node xmlNodeChilds = xmlNode.first_child();

			pugi::xml_node xmlNodeChild = xmlNodeChilds.first_child();

			// получаем детей
			while (!xmlNodeChild.empty())
			{
				int ChildID = TEXMLGetAttributeInt(xmlNodeChild, TEStr("id"), 0);
				Composite->addChild(tempAssosiation[ChildID]);

				// следующий
				xmlNodeChild = xmlNodeChild.next_sibling();
			}

			// следующий
			xmlNode = xmlNode.next_sibling();
			continue;
		}

		// Decorators
		if (Name == L"Inverter" || Name == L"Repeater" || Name == L"UntilSuccess" || Name == L"UntilFailure" || Name == L"CheckForHummer" || Name == L"Interrupt")
		{
			SPNode Node = tempAssosiation[id];
			BrainTree::Decorator* Decorator = (BrainTree::Decorator*)Node.get();

			pugi::xml_node xmlNodeChilds = xmlNode.first_child();
			pugi::xml_node xmlNodeChild = xmlNodeChilds.first_child();

			int ChildID = TEXMLGetAttributeInt(xmlNodeChild, TEStr("id"), 0);
			Decorator->setChild(tempAssosiation[ChildID]);
		}

		// следующий
		xmlNode = xmlNode.next_sibling();

	}

	Tree->setRoot(tempAssosiation[0]);

	return Tree;
}
