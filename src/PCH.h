#pragma once

#define NOMMNOSOUND

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

namespace logger = SKSE::log;

using namespace std::literals;

namespace stl
{
	using namespace SKSE::stl;

	void asm_replace(std::uintptr_t a_from, std::size_t a_size, std::uintptr_t a_to);

	template <class T>
	void asm_replace(std::uintptr_t a_from)
	{
		asm_replace(a_from, T::size, reinterpret_cast<std::uintptr_t>(T::func));
	}

	template <class T>
	void write_thunk_call(std::uintptr_t a_src)
	{
		auto& trampoline = SKSE::GetTrampoline();
		T::func = trampoline.write_call<5>(a_src, T::thunk);
	}

	template <class F, size_t offset, class T>
	void write_vfunc()
	{
		REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[offset] };
		T::func = vtbl.write_vfunc(T::idx, T::thunk);
	}

	template <class F, class T>
	void write_vfunc()
	{
		write_vfunc<F, 0, T>();
	}

	inline std::string as_string(std::string_view a_view)
	{
		return { a_view.data(), a_view.size() };
	}

	inline std::vector<std::string> splitLines(std::string a_string)
	{
		std::vector<std::string> lines;

		size_t last = 0;
		size_t next = 0;
		std::string delimiter = "\n";

		while ((next = a_string.find(delimiter, last)) != std::string::npos) {
			auto line = a_string.substr(last, next - last);
			last = next + 1;
			lines.emplace_back(line);
		}

		// Insert last line without /n
		auto line = a_string.substr(last, a_string.size() - last);
		lines.emplace_back(line);
		
		return lines;
	}

	inline std::string combineLines(std::vector<std::string> a_lines)
	{
		std::string result = "";
		for (auto line : a_lines) {
			result = result + line + '\n';
		}
		if (a_lines.size() > 0) {
			result.pop_back();  // Remove trailing \n
		}
		return result;
	}

	namespace string
	{
		inline bool icontains(std::string_view a_str1, std::string_view a_str2)
		{
			if (a_str2.length() > a_str1.length())
				return false;

			auto found = std::ranges::search(a_str1, a_str2,
				[](char ch1, char ch2) {
					return std::toupper(static_cast<unsigned char>(ch1)) == std::toupper(static_cast<unsigned char>(ch2));
				});

			return !found.empty();
		}
	}
}

#define DLLEXPORT __declspec(dllexport)