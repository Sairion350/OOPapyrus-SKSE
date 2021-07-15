#pragma once


namespace BaseObject
{
    using VM = RE::BSScript::IVirtualMachine;

    //auto const formType = RE::FormType::Quest;
    auto const formType = RE::FormType::Script;
	
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

		virtualmachine->ResetAllBoundObjects(handle);
		virtualmachine->GetObjectBindPolicy()->bindInterface->RemoveAllBoundObjects(handle);
		
		// dangerous??
		delete(form);
	}


	bool Bind(VM* a_vm)
	{
		const auto obj = "BaseObject"sv;

		BIND(MakeNewObject);
		BIND(DeleteObject);

		return true;
	}
}
