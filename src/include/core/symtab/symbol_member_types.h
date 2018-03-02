#pragma once
#include <cstdint>
namespace N_Core
{
	namespace N_Symbol
	{
		enum Binding
		{
			STB_LOCAL = 0
			,STB_GLOBAL = 1
			,STB_WEAK = 2
		};

		enum Type
		{
			STT_NOTYPE = 0
			,STT_OBJECT = 1
			,STT_FUNC = 2
			,STT_SECTION = 3
			,STT_FILE = 4
			,STT_COMMON = 5
		};

		struct Info
		{
			Type type : 4;
			Binding binding : 4;
		};

		struct Elf32_Sym
		{
			uint32_t  st_name;
			uint32_t  st_value;
			uint32_t  st_size;
			Info   st_info;
			uint8_t   st_other;
			uint16_t  st_shndx;
		};

		struct Elf64_Sym
		{
			uint32_t  st_name;
			Info	  st_info;
			uint8_t   st_other;
			uint16_t  st_shndx; 
			uint64_t  st_value;
			uint64_t  st_size;
		};



	}
}
