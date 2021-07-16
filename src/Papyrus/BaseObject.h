#pragma once


namespace BaseObject
{
    using VM = RE::BSScript::IVirtualMachine;

    //auto const formType = RE::FormType::Quest;
    auto const formType = RE::FormType::Script;

    std::vector<RE::BSTSmartPointer<RE::BSScript::Object>> GetAttatchedScripts (RE::TESForm* form);
	
	RE::TESForm* MakeNewObject(RE::StaticFunctionTag*, std::string classtype)
	{
		auto factory = RE::IFormFactory::GetFormFactoryByType(formType);
		auto form = factory->Create();


		auto virtualmachine = RE::BSScript::Internal::VirtualMachine::GetSingleton();

		auto handle = virtualmachine->GetObjectHandlePolicy1()->GetHandleForObject(formType, form);
		RE::BSTSmartPointer<RE::BSScript::Object> script;

		virtualmachine->CreateObject2(classtype, script);


		virtualmachine->BindObject(script, handle, false);
		
		return form;
	}

	void DeleteObject(RE::StaticFunctionTag*, RE::TESForm* form)
	{

		auto virtualmachine = RE::BSScript::Internal::VirtualMachine::GetSingleton();

		auto handle = virtualmachine->GetObjectHandlePolicy1()->GetHandleForObject(formType, form);

		virtualmachine->ResetAllBoundObjects(handle); // reset script vars
		//virtualmachine->GetObjectBindPolicy()->bindInterface->RemoveAllBoundObjects(handle); // does nothing?
		
		// dangerous??
		delete(form);

	}

	void Scan(RE::StaticFunctionTag*, RE::TESForm* form)
	{

		auto scripts = BaseObject::GetAttatchedScripts(form);

		logger::info("{} script attatched", scripts.size());

		
	}

	std::vector<RE::BSTSmartPointer<RE::BSScript::Object>> GetAttatchedScripts(RE::TESForm* form){
		std::vector<RE::BSTSmartPointer<RE::BSScript::Object>> ret;

		auto virtualmachine = RE::BSScript::Internal::VirtualMachine::GetSingleton();

		auto handle = virtualmachine->GetObjectHandlePolicy1()->GetHandleForObject(formType, form);

		auto map = &virtualmachine->attachedScripts;
		
		RE::BSTSmallSharedArray<RE::BSScript::Internal::AttachedScript>* attachedscripts = nullptr;
		for (auto& [vmhandle,scripts] : *map) { // todo, better way to get this? this type is giving me hell
    		if (handle == vmhandle){
    			attachedscripts = &scripts;
    		}
			
		}

		if (attachedscripts == nullptr){
			return ret;
		}

		for (auto& i : *attachedscripts){
			RE::BSTSmartPointer<RE::BSScript::Object> result;

			virtualmachine->FindBoundObject(handle, i->GetTypeInfo()->name.c_str(), result);

			ret.push_back(result);
		}

		return ret;
	}


	bool Bind(VM* a_vm)
	{
		const auto obj = "BaseObject"sv;

		BIND(MakeNewObject);
		BIND(DeleteObject);
		BIND(Scan);

		return true;
	}
}
