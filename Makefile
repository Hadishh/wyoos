GPPPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = obj/loader.o \
		obj/gdt.o obj/drivers/driver.o \
		obj/hardwarecommunication/port.o \
		obj/hardwarecommunication/interruptstubs.o \
		obj/hardwarecommunication/interrupts.o \
		obj/hardwarecommunication/pci.o \
		obj/multitasking.o \
		obj/drivers/keyboard.o \
		obj/drivers/mouse.o \
		obj/drivers/vga.o \
		obj/gui/widget.o \
		obj/gui/window.o \
		obj/gui/desktop.o \
		obj/kernel.o



obj/%.o: src/%.cpp
	mkdir -p $(@D)
	g++ $(GPPPARAMS) -c -o $@ $<

obj/%.o: src/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

kernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

install: kernel.bin
	sudo cp $< /boot/kernel.bin

kernel.iso : kernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $< iso/boot
	echo 'set timeout=0' >> iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo 'menuentry "My Operating System" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/kernel.bin' >> iso/boot/grub/grub.cfg
	echo '	boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso
run: kernel.iso
	(killall VirtualBoxVM && sleep 1) || true
	VirtualBoxVM --startvm "My OS" &

.PHONY : clean
clean:
	rm -rf obj kernel.iso kernel.bin