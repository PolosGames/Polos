#pragma once
#ifndef POLOS_CORE_ENGINE_SUBSYSTEM_H_
#define POLOS_CORE_ENGINE_SUBSYSTEM_H_

#include "utils/meta.h"
#include "utils/alias.h"
#include "utils/type_util.h"

namespace polos
{
	template<class SubsystemType>
	class Subsystem
	{
		struct Data
		{
			union
			{
				SubsystemType Instance;
				byte placeholder;
			};

			Data() : placeholder(0)
			{}
			~Data()
			{}
		};

		static Data m_Data;
	public:
		static void Initialize();
		static void Shutdown();

		static SubsystemType& Get();
	protected:
		template<typename... Args>
		static SubsystemType& Create(Args&&... args);
	private:
		static bool m_Created;
		static bool m_IsInitialized;
		static bool m_IsShutdown;
	};
	
	template<typename SubsystemType>
	Subsystem<SubsystemType>::Data Subsystem<SubsystemType>::m_Data;

	template<typename SubsystemType>
	bool Subsystem<SubsystemType>::m_Created = false;

	template<typename SubsystemType>
	bool Subsystem<SubsystemType>::m_IsInitialized = false;

	template<typename SubsystemType>
	bool Subsystem<SubsystemType>::m_IsShutdown = false;

	template<class SubsystemType>
	void Subsystem<SubsystemType>::Initialize()
	{
		if constexpr (!has_OnInit<SubsystemType>) return;

		if (m_IsInitialized)
		{
			LOG_CORE_WARN("Subsystem {} is already initialized, returning.", type_name<SubsystemType>());
			return;
		}
		LOG_CORE_TRACE("====== Initializing Subsytem: {} ======", type_name<SubsystemType>());
		SubsystemType::OnInitialize();

		if (m_Created) return;

		if constexpr (std::is_default_constructible_v<SubsystemType>)
			Create();
		else
			LOG_CORE_ERROR("Subsystem {} could not be initialized.", type_name<SubsystemType>());
	}

	template<class SubsystemType>
	void Subsystem<SubsystemType>::Shutdown()
	{
		if constexpr (has_OnShutdown<SubsystemType>)
		{
			if (!m_Created) return;

			if (m_IsShutdown)
			{
				LOG_CORE_WARN("Subsystem {} is already shutdown, returning.", type_name<SubsystemType>());
				return;
			}
			
			LOG_CORE_TRACE("====== Shutting down Subsytem: {} ======", type_name<SubsystemType>());
			SubsystemType::OnShutdown();
		}
	}

	template<class SubsystemType>
	inline SubsystemType& Subsystem<SubsystemType>::Get()
	{
		return m_Data.Instance;
	}
	
	template<class SubsystemType>
	template<typename... Args>
	inline SubsystemType& Subsystem<SubsystemType>::Create(Args&&... args)
	{
		new (&m_Data.Instance) SubsystemType(std::forward<Args>(args)...);
		m_Created = true;
		return m_Data.Instance;
	}
} // namespace polos

#endif /* POLOS_CORE_ENGINE_SUBSYSTEM_H_ */