#pragma once
#include "src/include/core/general.h"
#include "src/include/core/reltab/relocation_member_types.h"
namespace N_Core
{
	namespace N_Relocation
	{
		template <typename V, typename C>
		class Relocation
		{
		private:
			C const& _container;
			using T = std::conditional_t<
				std::is_same_v<V, Bit64>, Elf64_Rela, Elf32_Rela
			>;//add non rela here

			MMap::Container<T> _content;
		public:
			Relocation(C const& container, BinaryBlob range) :_container(container), _content(range.begin()) {}
			Relocation(Relocation&&) = delete;
			Relocation(Relocation const&) = delete;
			MMap::Container<T>& get_content() { return _content; }
			MMap::Container<T> const& get_content() const { return _content; }

			C const& get_parent()const { return _container; }

			uint64_t get_offset()const { return  get(_content, &T::r_offset); }
			uint64_t get_info()const { return  get(_content, &T::r_info); }
			int64_t get_addend()const 
			{
				if constexpr(std::is_same_v<V, Elf64_Rela> || std::is_same_v<V, Elf32_Rela>)
				{
					return  get(_content, &T::r_addend); 	
				} 
				else
				{
					return 0;
				}
			}
		};

		template <typename T, typename C>
		std::ostream& operator<<(std::ostream& stream, Relocation<T, C> const& relocation)
		{
			stream << relocation.get_content();
			return stream;
		}
	}
}

