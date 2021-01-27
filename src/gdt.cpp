#include <gdt.h>
using namespace myos;
using namespace myos::common;
void printf(char* str);
//details on https://wiki.osdev.org/Global_Descriptor_Table
GlobalDescriptorTable::GlobalDescriptorTable()
  : nullSegmentSelector(0, 0, 0),
    unusedSegmentSelector(0, 0, 0),
    codeSegmentSelector(0, 64*1024*1024, 0x9A),
    dataSegmentSelector(0, 64*1024*1024, 0x92)
{
    uint32_t i[2];
    i[1] = (uint32_t)this;
    i[0] = sizeof(GlobalDescriptorTable) << 16;
    asm volatile("lgdt (%0)": :"p" (((uint8_t *) i)+2));
    printf("[OK] Initilized Global Descriptor Table.\n");
}

GlobalDescriptorTable::~GlobalDescriptorTable(){

}

// returns the offset of data segment selector
uint16_t GlobalDescriptorTable::DataSegmentSelector(){
    return (uint8_t*)&dataSegmentSelector - (uint8_t*)this;
}

// returns the offset of code segment selector
uint16_t GlobalDescriptorTable::CodeSegmentSelector(){
    return (uint8_t*)&codeSegmentSelector - (uint8_t*)this;
}


GlobalDescriptorTable::SegmentDescriptor :: SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t flags){
    uint8_t* target = (uint8_t*)this;
    if(limit < 0x10000){
        target[6] = 0x40;
    }
    else{
        if((limit & 0xfff) != 0xfff)
            limit = (limit >> 12) - 1;
        else
            limit = limit >> 12;            
        target[6] = 0xc0;
    }
    target[0] = limit & 0xff;
    target[1] = (limit >> 8) & 0xff;
    target[6] |= (limit >> 16) & 0xf;

    target[2] = base & 0xff;
    target[3] = (base >> 8) & 0xff;
    target[4] = (base >> 16) & 0xff;
    target[7] = (base >> 24) & 0xff;

    target[5] = flags;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::Base(){
    uint8_t* target = (uint8_t*)this;
    uint32_t base = 0;
    base = target[2] + (target[3] << 8) + (target[4] << 16) + (target[7] << 24);
    return base;
}
uint32_t GlobalDescriptorTable::SegmentDescriptor::Limit(){
    uint8_t* target = (uint8_t*)this;
    uint32_t limit = 0;
    limit = target[0] + (target[1] << 8) + ((target[6] & 0xf) << 16);
    if((target[6] & 0xc0) == 0xc0)
        limit = (limit << 12) | 0xfff;
    return limit;
}