#pragma once


namespace BaseObject
{
    using VM = RE::BSScript::IVirtualMachine;

    //auto const formType = RE::FormType::Quest;
    auto const formType = RE::FormType::Script;

    std::vector<RE::BSTSmartPointer<RE::BSScript::Object>> GetAttatchedScripts (RE::TESForm* form);
	
	RE::TESForm* MakeNewBoundObject(RE::StaticFunctionTag*, std::string classtype)
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

		auto handle = virtualmachine->GetObjectHandlePolicy1()->GetHandleForObject(form->GetFormType(), form);
		
		virtualmachine->RemoveAllBoundObjects(handle);

		// dangerous??
		delete(form);

	}

	void UnbindObjects(RE::StaticFunctionTag*, RE::TESForm* form){ // Generally speaking, don't use this it kind of sucks
		//auto virtualmachine = RE::BSScript::Internal::VirtualMachine::GetSingleton();

		//for (auto& i : BaseObject::GetAttatchedScripts(form)){
		//	virtualmachine->UnbindObject(i);
		//}
		auto virtualmachine = RE::BSScript::Internal::VirtualMachine::GetSingleton();
		
		auto handle = virtualmachine->GetObjectHandlePolicy1()->GetHandleForObject(form->GetFormType(), form);
		
		virtualmachine->RemoveAllBoundObjects(handle);

		// dangerous??
		delete(form);

	}


	std::string GetVMHandle(RE::StaticFunctionTag*, RE::TESForm* boundform, std::string classstr){
		auto virtualmachine = RE::BSScript::Internal::VirtualMachine::GetSingleton();

		auto handle = virtualmachine->GetObjectHandlePolicy1()->GetHandleForObject(boundform->GetFormType(), boundform);

		RE::BSTSmartPointer<RE::BSScript::Object> result;

		virtualmachine->FindBoundObject(handle, classstr.c_str(), result);

		return std::to_string(result->GetHandle());
	}

	void Scan(RE::StaticFunctionTag*, RE::TESForm* form)
	{

		auto scripts = BaseObject::GetAttatchedScripts(form);

		//scripts[0]->~Object()// calls destructor

		logger::info("{} script attatched", scripts.size());

		for (auto& i : scripts){
			logger::info("{} references", i->refCountAndHandleLock);
		}

		
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

	void CallEventOnForm(RE::StaticFunctionTag*, RE::TESForm* input, std::string eventname){
		if (!input){
			return;
		}

		SKSE::RegistrationSet<> Event(eventname);

		Event.Register(input);
		Event.SendEvent();
		Event.Unregister(input);

	}


	bool Bind(VM* a_vm)
	{
		const auto obj = "BaseObject"sv;

		BIND(MakeNewBoundObject);
		BIND(DeleteObject);
		BIND(Scan);
		BIND(GetVMHandle, true);
		BIND(UnbindObjects);
		BIND(CallEventOnForm, true); // needs to have tasklet?

		return true;
	}
}
