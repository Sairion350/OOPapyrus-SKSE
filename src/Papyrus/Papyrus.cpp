#include "Papyrus\Papyrus.h"

#include "Util.h"

#define BIND(a_method, ...) a_vm->RegisterFunction(#a_method##sv, obj, a_method __VA_OPT__(, ) __VA_ARGS__)
#include "Papyrus/BaseObject.h"
#undef BIND

namespace Papyrus
{

	template <class T>
	RE::VMTypeID vm_cast()
	{
		return static_cast<RE::VMTypeID>(T::FORMTYPE);
	}

	void Bind()
	{
		auto papyrus = SKSE::GetPapyrusInterface();
		if (papyrus) {

			//can only pass back forms
			//RE::BSScript::Internal::VirtualMachine::GetSingleton()->RegisterObjectType(vm_cast<RE::BSScript::Object>(), "ScriptInstance");

			papyrus->Register(BaseObject::Bind);
		}
	}
}
