#include "src/include/core/elf.h"

#include <boost/test/unit_test.hpp> 
#include <boost/filesystem.hpp>

#include <algorithm>
BOOST_AUTO_TEST_SUITE(elf_section_table)

/*
BOOST_AUTO_TEST_CASE(correct_amount_of_sections)
{
	auto elf = N_Core::create_elf<N_Core::Bit64>("testfiles/sleep");
	BOOST_CHECK_EQUAL(std::size(elf._section_table), 27);

	auto elf2(elf);
	BOOST_CHECK_EQUAL(std::size(elf2._section_table), 27);

	N_Core::dump_to_file("testfiles/correct_amount_of_sections", elf2);
	
	auto elf3 = N_Core::create_elf<N_Core::Bit64>("testfiles/correct_amount_of_sections");
	BOOST_CHECK_EQUAL(std::size(elf3._section_table), 27);

}
BOOST_AUTO_TEST_CASE(correct_section_header_after_dump)
{
	auto elf = N_Core::create_elf<N_Core::Bit64>("testfiles/sleep");
	N_Core::dump_to_file("testfiles/correct_header_after_dump", elf);
	auto elf2 = N_Core::create_elf<N_Core::Bit64>("testfiles/correct_header_after_dump");

	std::transform(
		std::begin(elf._section_table),
		std::end(elf._section_table),
		std::begin(elf2._section_table),
		N_Core::VoidIterator<>(),
		[](auto const& section_elf_1, auto const& section_elf_2)
		{
			BOOST_CHECK_EQUAL(section_elf_1.get_name(), section_elf_2.get_name());
			BOOST_CHECK_EQUAL(section_elf_1.get_type(), section_elf_2.get_type());
			BOOST_CHECK_EQUAL(section_elf_1.get_flags(), section_elf_2.get_flags());
			BOOST_CHECK_EQUAL(section_elf_1.get_address(), section_elf_2.get_address());
			BOOST_CHECK_EQUAL(section_elf_1.get_offset(), section_elf_2.get_offset());
			BOOST_CHECK_EQUAL(section_elf_1.get_size(), section_elf_2.get_size());
			BOOST_CHECK_EQUAL(section_elf_1.get_link(), section_elf_2.get_link());
			BOOST_CHECK_EQUAL(section_elf_1.get_info(), section_elf_2.get_info());
			BOOST_CHECK_EQUAL(section_elf_1.get_address_alignment(), section_elf_2.get_address_alignment());
			BOOST_CHECK_EQUAL(section_elf_1.get_entry_size(), section_elf_2.get_entry_size());

			return N_Core::VoidIterator<>::value_type();
		}
	);

}

BOOST_AUTO_TEST_CASE(correct_section_content_after_dump)
{
	auto elf = N_Core::create_elf<N_Core::Bit64>("testfiles/sleep");
	N_Core::dump_to_file("testfiles/correct_content_when_dumping_table", elf);
	auto elf2 = N_Core::create_elf<N_Core::Bit64>("testfiles/correct_content_when_dumping_table");
	BOOST_CHECK_EQUAL(elf._header.get_section_header_number_of_entries(), elf2._header.get_section_header_number_of_entries());

	std::transform(
		std::begin(elf._section_table),
		std::end(elf._section_table),
		std::begin(elf2._section_table),
		N_Core::VoidIterator<>(),
		[](auto const& section_elf_1, auto const& section_elf_2)
		{
		 
			std::size_t size_section_blob_elf_1 = section_elf_1.get_content().get_size();
			std::size_t size_section_blob_elf_2 = section_elf_2.get_content().get_size();

			
			BOOST_CHECK_EQUAL(section_elf_1.get_size(), section_elf_2.get_size());
			BOOST_CHECK_EQUAL(size_section_blob_elf_1, size_section_blob_elf_2);
			BOOST_CHECK_EQUAL(section_elf_1.get_size(), size_section_blob_elf_2);

			int result = std::memcmp(
				&(*std::begin(section_elf_1.get_content())),
				&(*std::begin(section_elf_2.get_content())),
				size_section_blob_elf_1);

			BOOST_CHECK_EQUAL(result, 0);
			return N_Core::VoidIterator<>::value_type();
		}
	);
}

BOOST_AUTO_TEST_CASE(size_in_header_and_in_memory_sanity_check)
{
	auto size_of_section_21 = 0x1d0;
	auto elf = N_Core::create_elf<N_Core::Bit64>("testfiles/sleep");
	
	BOOST_CHECK_EQUAL(elf._section_table[21].get_size(), size_of_section_21);
}



BOOST_AUTO_TEST_CASE(remove_section)
{
	auto elf_to_remove_section_from = N_Core::create_elf<N_Core::Bit64>("testfiles/sleep");
	auto copy_of_original = elf_to_remove_section_from;

	BOOST_CHECK_EQUAL(
		elf_to_remove_section_from._header.get_section_header_number_of_entries(),
		copy_of_original._header.get_section_header_number_of_entries()
	);

	N_Core::dump_to_file("testfiles/remove_section", elf_to_remove_section_from);

	auto index_of_section_to_remove = 6;
	auto size_of_removed_section_check = 0x289;
	auto size_of_removed_section = elf_to_remove_section_from._section_table[index_of_section_to_remove].get_size();
	BOOST_CHECK_EQUAL(size_of_removed_section_check, size_of_removed_section);

	elf_to_remove_section_from.remove_section(index_of_section_to_remove, N_Core::N_Section::SectionRemovalPolicy::COMPACT);

	BOOST_CHECK_EQUAL(
		elf_to_remove_section_from._header.get_section_header_number_of_entries(),
		copy_of_original._header.get_section_header_number_of_entries() - 1
	);

	N_Core::dump_to_file("testfiles/remove_section", N_Core::Elf<N_Core::Bit64>(elf_to_remove_section_from));

	auto verification_elf = N_Core::create_elf<N_Core::Bit64>("testfiles/remove_section");

	BOOST_CHECK_EQUAL(
		verification_elf._header.get_section_header_number_of_entries(),
		copy_of_original._header.get_section_header_number_of_entries() - 1
	);

	BOOST_CHECK_EQUAL(
		boost::filesystem::file_size("testfiles/remove_section"), 
		boost::filesystem::file_size("testfiles/sleep") - sizeof(N_Core::N_Section::Elf64_Shdr) - size_of_removed_section
	);	
}

BOOST_AUTO_TEST_CASE(remove_all_sections_from_elf_beginning_at_start)
{
	auto elf_under_test = N_Core::create_elf<N_Core::Bit64>("testfiles/sleep");
	auto size_with_section = boost::filesystem::file_size("testfiles/sleep");
	auto number_of_sections_in_original_elf = elf_under_test._header.get_section_header_number_of_entries();

	auto tot_size_of_program_headers = elf_under_test._header.get_program_header_number_of_entries() * elf_under_test._header.get_program_header_entry_size();
	
	std::string path_for_this_iteration = "testfiles/sleep_rebuild";
	N_Core::dump_to_file(path_for_this_iteration, N_Core::create_elf<N_Core::Bit64>("testfiles/sleep"));

	for (auto i = 0; i < number_of_sections_in_original_elf; i++)
	{
		auto elf_to_remove_section_from = N_Core::create_elf<N_Core::Bit64>(path_for_this_iteration.c_str());
		auto size_of_removed_section = elf_to_remove_section_from._section_table[0].get_size_in_file();

		elf_to_remove_section_from.remove_section(0, N_Core::N_Section::SectionRemovalPolicy::COMPACT);

		path_for_this_iteration = std::string("testfiles/remove_all_sections_from_elf_iteration_") + std::to_string(i);

		N_Core::dump_to_file(path_for_this_iteration, elf_to_remove_section_from);

		auto size_without_section = boost::filesystem::file_size(path_for_this_iteration);

		// This check only works for all except the last section removed.
		// For one, the program header is still present and padding(sections are not always glued together there may be spacing (in file size) between them)
		// between the sections will have been accumulated ensuring that when all but one sections has been removed there is a gap
		// between the elf header and the first sections.
		// to ensure that when the elf is memory mapped sections can be loaded into memory directly and do not need copying to the correct address (see page alignment).
		// For example
		//
		// [HEADER][SEGMENT TABLE][S1][PADDING1][S2][PADDING2][SECTION TABLE]
		//
		// Remove S1
		//
		// [HEADER][SEGMENT TABLE][PADDING1][S2][PADDING2][SECTION TABLE]
		//
		// Note S1 padding is still present. This padding is actually present in the offset of S2.
		// offset of S2 = offset S1 + size S1 + padding between S1 and S2. 
		// Remove 2
		//
		// [HEADER][SEGMENT TABLE][PADDING1][PADDING2][SECTION TABLE]
		//
		// Note S1 and S2 padding. thsi padding is present in the offset of the section header table.
		//
		// However; if the section table contains no entries it will not be dumped to file and thus the offset is
		// ignored and all padding spacing and segment table spacing is ignored. leading to an elf with a header only.
		if (i != number_of_sections_in_original_elf - 1)
		{
			BOOST_CHECK_EQUAL(
				size_with_section - size_of_removed_section - sizeof(N_Core::N_Section::Elf64_Shdr),
				size_without_section
			);
		} 

		size_with_section = size_without_section;
	}

	//finally only elf header should be left.
	auto size_without_any_sections = size_with_section;
	BOOST_CHECK_EQUAL(
		sizeof(N_Core::N_Header::Elf64_Ehdr),
		size_without_any_sections
	);

}

BOOST_AUTO_TEST_CASE(remove_first_section)
{
	std::string path_for_this_iteration = "testfiles/remove_first_section";

	auto elf_to_remove_section_from = N_Core::create_elf<N_Core::Bit64>("testfiles/sleep");

	BOOST_CHECK_EQUAL(
		elf_to_remove_section_from._section_table[0].get_type()
		, N_Core::N_Section::Type::SHT_NULL
	);

	elf_to_remove_section_from.remove_section(0, N_Core::N_Section::SectionRemovalPolicy::COMPACT);

	for (auto const& section : elf_to_remove_section_from._section_table)
	{
		if (section.get_offset() + section.get_size() > elf_to_remove_section_from._header.get_section_header_offset())
		{
			auto b = 0;
		}
	}

	BOOST_CHECK_EQUAL(
		elf_to_remove_section_from._section_table[0].get_type()
		, N_Core::N_Section::Type::SHT_PROGBITS
	);
	
	N_Core::dump_to_file(path_for_this_iteration, elf_to_remove_section_from);
	auto elf_to_remove_section_from_2 = N_Core::create_elf<N_Core::Bit64>(path_for_this_iteration.c_str());

	BOOST_CHECK_EQUAL(
		elf_to_remove_section_from_2._section_table[0].get_type()
		, N_Core::N_Section::Type::SHT_PROGBITS
	);
}

BOOST_AUTO_TEST_CASE(remove_all_sections_from_elf_beginning_at_start_gap)
{
	auto elf_under_test = N_Core::create_elf<N_Core::Bit64>("testfiles/sleep");
	auto size_with_section = boost::filesystem::file_size("testfiles/sleep");
	auto number_of_sections_in_original_elf = elf_under_test._header.get_section_header_number_of_entries();

	auto tot_size_of_program_headers = elf_under_test._header.get_program_header_number_of_entries() * elf_under_test._header.get_program_header_entry_size();

	std::string path_for_this_iteration = "testfiles/sleep_rebuild";
	N_Core::dump_to_file(path_for_this_iteration, N_Core::create_elf<N_Core::Bit64>("testfiles/sleep"));

	for (auto i = 0; i < number_of_sections_in_original_elf; i++)
	{
		auto elf_to_remove_section_from = N_Core::create_elf<N_Core::Bit64>(path_for_this_iteration.c_str());
		auto size_of_removed_section = elf_to_remove_section_from._section_table[0].get_size_in_file();

		elf_to_remove_section_from.remove_section(0, N_Core::N_Section::SectionRemovalPolicy::GAP);

		path_for_this_iteration = std::string("testfiles/remove_all_sections_from_elf_iteration_gap_") + std::to_string(i);

		N_Core::dump_to_file(path_for_this_iteration, elf_to_remove_section_from);

		auto size_without_section = boost::filesystem::file_size(path_for_this_iteration);

		//When the last section is removed there is no longer any gap in the sections.
		// For example:
		// Say 3 sections exist indicated S1, S2 and S3
		// [HEADER][S1][S2][S3][SECTION_TABLE (3 elements)]
		// remove S1
		// [HEADER][GAP][S2][S3][SECTION_TABLE (2 elements)]
		// remove S3
		// [HEADER][GAP][S2][GAP][SECTION_TABLE (1 element)]
		// remove S2
		// [HEADER][GAP][GAP][GAP][SECTION_TABLE (0 elements)] = [HEADER]
		// GAPS are not written to file nor is a section table without any elements.
		if (i != number_of_sections_in_original_elf - 1)
		{
			BOOST_CHECK_EQUAL(
				size_with_section - sizeof(N_Core::N_Section::Elf64_Shdr)
				, size_without_section
			);
		}
		size_with_section = size_without_section;
	}

	//finally only elf header should be left.
	auto size_without_any_sections = size_with_section;
	BOOST_CHECK_EQUAL(
		size_without_any_sections,
		sizeof(N_Core::N_Header::Elf64_Ehdr)
	);

}



BOOST_AUTO_TEST_CASE(wildcard_add_section)
{
	N_Core::Elf<N_Core::Bit64> elf = N_Core::create_elf<N_Core::Bit64>("testfiles/sleep");
	auto current_size = boost::filesystem::file_size("testfiles/sleep");


	auto original_section_table_size = std::size(elf._section_table);
	auto original_number_of_section_header_entries = elf._header.get_section_header_number_of_entries();

	auto index = elf.add_section(N_Core::N_Section::Section<N_Core::N_Section::Elf64_Shdr>(), N_Core::Index::Wildcard);

	auto additional_offset = 1000;
	elf._section_table[index].set_offset(current_size+ additional_offset);

	std::array<uint8_t, 1234> content;

	std::iota(std::begin(content), std::end(content), 0);

	elf._section_table[index].set_size(content.size());
	
	update(elf._section_table[index], std::begin(content), std::end(content));

	std::transform
	(
		std::begin(content),
		std::end(content),
		std::begin(elf._section_table[index].get_content()),
		N_Core::VoidIterator<>(),
		[](auto const& element, auto const& element2)
		{
			BOOST_CHECK_EQUAL(element, element2);
			return N_Core::VoidIterator<>::value_type();
		}
	);

	N_Core::dump_to_file("testfiles/with", elf);

	auto new_size = boost::filesystem::file_size("testfiles/with");

	BOOST_CHECK_EQUAL(new_size, current_size + content.size()+ additional_offset);

	N_Core::Elf<N_Core::Bit64> elf2 = N_Core::create_elf<N_Core::Bit64>("testfiles/with");

	BOOST_CHECK_EQUAL(original_section_table_size + 1, std::size(elf2._section_table));
	BOOST_CHECK_EQUAL(original_number_of_section_header_entries + 1, elf2._header.get_section_header_number_of_entries());

	N_Core::N_Section::Section<N_Core::N_Section::Elf64_Shdr> const& section = elf2._section_table[index];

	BOOST_CHECK_EQUAL(std::memcmp(&((section.get_content())[0]), &content[0], content.size()), 0);
	
	
}

BOOST_AUTO_TEST_CASE(specific_index_section)
{
	N_Core::Elf<N_Core::Bit64> elf = N_Core::create_elf<N_Core::Bit64>("testfiles/sleep");
	auto current_size = boost::filesystem::file_size("testfiles/sleep");


	auto original_section_table_size = std::size(elf._section_table);
	auto original_number_of_section_header_entries = elf._header.get_section_header_number_of_entries();

	auto index = elf.add_section(N_Core::N_Section::Section<N_Core::N_Section::Elf64_Shdr>(), 10);
	BOOST_CHECK_EQUAL((int)index, 10);

	auto additional_offset = 1000;
	elf._section_table[index].set_offset(current_size + additional_offset);

	std::array<uint8_t, 7777> content;

	std::iota(std::begin(content), std::end(content), 0);

	elf._section_table[index].set_size(content.size());

	update(elf._section_table[index], std::begin(content), std::end(content));

	std::transform
	(
		std::begin(content),
		std::end(content),
		std::begin(elf._section_table[index].get_content()),
		N_Core::VoidIterator<>(),
		[](auto const& element, auto const& element2)
		{
			BOOST_CHECK_EQUAL(element, element2);
			return N_Core::VoidIterator<>::value_type();
		}
	);





	N_Core::dump_to_file("testfiles/with", elf);

	auto new_size = boost::filesystem::file_size("testfiles/with");

	BOOST_CHECK_EQUAL(new_size, current_size + content.size() + additional_offset);

	N_Core::Elf<N_Core::Bit64> elf2 = N_Core::create_elf<N_Core::Bit64>("testfiles/with");

	BOOST_CHECK_EQUAL(original_section_table_size + 1, std::size(elf2._section_table));
	BOOST_CHECK_EQUAL(original_number_of_section_header_entries + 1, elf2._header.get_section_header_number_of_entries());

	N_Core::N_Section::Section<N_Core::N_Section::Elf64_Shdr> const& section = elf2._section_table[index];

	BOOST_CHECK_EQUAL(std::memcmp(&(section.get_content())[0], &content[0], content.size()), 0);

	//Verify if sections before the inserted section are still in tact.
	N_Core::Elf<N_Core::Bit64> elf_original = N_Core::create_elf<N_Core::Bit64>("testfiles/sleep");
	N_Core::Index indx = 0;
	std::transform(
		std::begin(elf_original._section_table),
		std::end(elf_original._section_table),
		std::begin(elf2._section_table),
		N_Core::VoidIterator<>(),
		[&indx,index](auto const& section_elf_1, auto const& section_elf_2) mutable
	{
		if (indx < index)
		{
			std::size_t size_section_blob_elf_1 = section_elf_1.get_content().get_size();
			std::size_t size_section_blob_elf_2 = section_elf_2.get_content().get_size();


			BOOST_CHECK_EQUAL(section_elf_1.get_size(), section_elf_2.get_size());
			BOOST_CHECK_EQUAL(size_section_blob_elf_1, size_section_blob_elf_2);
			BOOST_CHECK_EQUAL(section_elf_1.get_size(), size_section_blob_elf_2);

			int result = std::memcmp(
				&(*std::begin(section_elf_1.get_content())),
				&(*std::begin(section_elf_2.get_content())),
				size_section_blob_elf_1);

			BOOST_CHECK_EQUAL(result, 0);
		}
		indx++;
		return N_Core::VoidIterator<>::value_type();
	}
	);


}
BOOST_AUTO_TEST_CASE(get_section_name)
{
	N_Core::Elf<N_Core::Bit64> elf = N_Core::create_elf<N_Core::Bit64>("testfiles/sleep");

	auto names =
	{
		""
		, ".interp"
		, ".note.ABI-tag"
		, ".note.gnu.build-id"
		, ".gnu.hash"
		, ".dynsym"
		, ".dynstr"
		, ".gnu.version"
		, ".gnu.version_r"
		, ".rela.dyn"
		, ".rela.plt"
		, ".init"
		, ".plt"
		, ".text"
		, ".fini"
		, ".rodata"
		, ".eh_frame_hdr"
		, ".eh_frame"
		, ".init_array"
		, ".fini_array"
		, ".jcr"
		, ".dynamic"
		, ".got"
		, ".got.plt"
		, ".data"
		, ".bss"
		, ".shstrtab"
	};

	auto idx = 0;
	for (auto const& expected_name : names)
	{
		std::string name_in_file = N_Core::get_name(elf, idx++);
		BOOST_CHECK_EQUAL(name_in_file, expected_name);
	}


}
BOOST_AUTO_TEST_CASE(set_section_name_already_exists)
{
	N_Core::Elf<N_Core::Bit64> elf = N_Core::create_elf<N_Core::Bit64>("testfiles/sleep");

	auto idx = elf._header.get_section_index_that_contains_strings();
	auto size_in_file = elf._section_table[idx].get_size_in_file();

	for (auto i = 0; i < std::size(elf._section_table); i++)
	{
		N_Core::set_name(elf, i, ".interp");
		std::string name_in_file = N_Core::get_name(elf, i);
		BOOST_CHECK_EQUAL(name_in_file, ".interp");
	}
	auto size_in_file_2 = elf._section_table[idx].get_size_in_file();

	BOOST_CHECK_EQUAL(size_in_file, size_in_file_2);

	N_Core::dump_to_file("testfiles/interpsleep", elf);

	N_Core::Elf<N_Core::Bit64> elf2 = N_Core::create_elf<N_Core::Bit64>("testfiles/interpsleep");

	for (auto i = 0; i < std::size(elf2._section_table); i++)
	{
		std::string name_in_file = N_Core::get_name(elf2, i);
		BOOST_CHECK_EQUAL(name_in_file, ".interp");
	}
}

BOOST_AUTO_TEST_CASE(set_section_new_names)
{
	N_Core::Elf<N_Core::Bit64> elf = N_Core::create_elf<N_Core::Bit64>("testfiles/sleep");

	auto idx = elf._header.get_section_index_that_contains_strings();
	auto size_in_file = elf._section_table[idx].get_size_in_file();

	for (auto i = 0; i < std::size(elf._section_table); i++)
	{
		std::string reversed_name = N_Core::get_name(elf, i);
		std::reverse(std::begin(reversed_name), std::end(reversed_name));
		N_Core::set_name(elf, i, reversed_name);
	}

	BOOST_CHECK_EQUAL(N_Core::get_name(elf, 1), "pretni."); //sanity check

	N_Core::dump_to_file("testfiles/reversedsleep", elf);

	N_Core::Elf<N_Core::Bit64> elf2 = N_Core::create_elf<N_Core::Bit64>("testfiles/reversedsleep");

	for (auto i = 0; i < std::size(elf2._section_table); i++)
	{
		std::string origin = N_Core::get_name(elf, i);
		std::string fresh = N_Core::get_name(elf2, i);
		BOOST_CHECK_EQUAL(fresh, origin);
	}
}
BOOST_AUTO_TEST_CASE(is_valid_layout)
{
	N_Core::Elf<N_Core::Bit64> elf = N_Core::create_elf<N_Core::Bit64>("testfiles/sleep");

	BOOST_CHECK_EQUAL(N_Core::is_valid_layout(elf).empty(), true);

	std::vector<N_Core::Index> invalid_sections = { 0, 1, 6, 8, 10, 24};
	
	for (const auto& section : invalid_sections)
		elf._section_table[section].set_size(std::numeric_limits<uint64_t>::max()>>1);

	BOOST_CHECK_EQUAL(N_Core::is_valid_layout(elf), invalid_sections);

}*/
//BOOST_AUTO_TEST_CASE(section_table_is_of_valid_size)
//{
//	auto elf = N_Core::create_elf<N_Core::Bit64>("testfiles/sleep");
//	BOOST_CHECK_EQUAL(std::size(elf._section_table), 27);
//}
//BOOST_AUTO_TEST_CASE(iterate_over_code_and_data_sections)
//{
//	auto elf = N_Core::create_elf<N_Core::Bit64>("testfiles/data_empty_bss_global_and_local_symbol");
//	
//
//	auto number_of_code_sections = std::distance(elf._section_table.begin_code(), elf._section_table.end_code());
//	auto number_of_data_sections = std::distance(elf._section_table.begin_data(), elf._section_table.end_data());
//
//	BOOST_CHECK_EQUAL(number_of_code_sections, 1); //.text
//	BOOST_CHECK_EQUAL(number_of_data_sections, 2); //.data and .bss
//
//}
BOOST_AUTO_TEST_SUITE_END()