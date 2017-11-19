#pragma once
#include "src/include/core/header/header.h"
#include "src/include/core/general.h"

#include <string>
#include <optional>
#include <memory>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/iterator.hpp>
#include <boost/type_erasure/operators.hpp>
#include <boost/type_erasure/tuple.hpp>
#include <boost/type_erasure/same_type.hpp>
#include <boost/type_erasure/member.hpp>
namespace te = boost::type_erasure;

BOOST_TYPE_ERASURE_MEMBER((has_member_store), store, 1)

using Linkable = te::any<boost::mpl::vector<
	te::copy_constructible<>,
	te::relaxed
>>;

namespace N_Core
{
	template <typename T>
	class Elf
	{
	public:
		// Name of source elf or elf to create.
		std::string _file_name;

		// The header is templated because based on type type of elf(64 bit vs 32bit) another memory map (different 
		// member variables) is selected.
		N_Header::Header<T> _header;

		// Shared ptr because an output elf containing sections of this elf will need
		// to extend the lifetime of the memory mapped region.
		std::shared_ptr<boost::interprocess::mapped_region> _region; 

		Elf(std::shared_ptr<boost::interprocess::mapped_region>&& mapped_region) :
			_region(std::move(mapped_region))
			, _header(BinaryBlob(reinterpret_cast<uint8_t*>(_region->get_address()), reinterpret_cast<uint8_t*>(_region->get_address()) + _region->get_size()))
		{
		}

		// Create new elf from existing elf. elf will be stored under file_name.
		Elf(Elf const& elf, std::string file_name) :
			_region(elf._region)
			, _header(_header)
			, _file_name(file_name)
		{

		}
		
	};

	// Build elf from path. Will map file into memory.
	Linkable create_elf(std::string const& path_to_elf)
	{
		boost::interprocess::file_mapping m_file(path_to_elf.c_str(), boost::interprocess::read_only);
		auto&& memory_region = std::make_shared<boost::interprocess::mapped_region>(m_file, boost::interprocess::read_only);

		uint8_t* base_address = static_cast<uint8_t*>((*memory_region).get_address());
		N_Core::N_Header::Header<N_Core::Bit32>* some_header = reinterpret_cast<N_Core::N_Header::Header<N_Core::Bit32>*>(base_address);

		if (some_header->is_64bit_header())
		{
			return Elf<N_Core::Bit64>(std::move(memory_region));
		}
		else
		{
			return Elf<N_Core::Bit32>(std::move(memory_region));
		}

	}

	// @brief create elf from an existing elf.
	// 
	template <typename T>
	Linkable create_elf(N_Core::Elf<T> elf, std::string const& path_to_elf)
	{
		return N_Core::Elf<T>(elf, path_to_elf);
	}
}