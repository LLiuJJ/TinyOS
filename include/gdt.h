
#ifndef __TINYOS__GDT_H
#define __TINYOS__GDT_H

#include <common/types.h>
namespace tinyos
{
	class GlobalDescriptorTable
	{
	public:
		class SegmentDescriptor
		{
		private:
			tinyos::common::uint16_t limit_lo; //限制低字节
			tinyos::common::uint16_t base_lo;  //point低字节
			tinyos::common::uint8_t base_hi; //excess pipes
			tinyos::common::uint8_t type;  //
			tinyos::common::uint8_t flags_limit_hi; //这里包含了两个limit和flag
			tinyos::common::uint8_t base_vhi;  //
		public:
			SegmentDescriptor(tinyos::common::uint32_t base, tinyos::common::uint32_t limit, tinyos::common::uint8_t type);
			tinyos::common::uint32_t Base();
			tinyos::common::uint32_t Limit();
		}  __attribute__((packed));

		SegmentDescriptor nullSegmentSelector;
		SegmentDescriptor unusedSegmentSelector;
		SegmentDescriptor codeSegmentSelector;
		SegmentDescriptor dataSegmentSelector;

	public:
		GlobalDescriptorTable();
		~GlobalDescriptorTable();
		tinyos::common::uint16_t CodeSegmentSelector();
		tinyos::common::uint16_t DataSegmentSelector();

	};
}


#endif

