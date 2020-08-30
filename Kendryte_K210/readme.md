### kendryte-standalone-sdk build
```
export PATH=/xxx/cmake-3.16.8-Linux-x86_64/bin:/xxx/kendryte-toolchain/bin:$PATH

cd build

#cmake .. -DPROJ=hello_world -DTOOLCHAIN=/xxx/kendryte-toolchain/bin && make -j8

cmake -DTOOLCHAIN=/xxx/kendryte-toolchain/bin -DPROJ=dock_psram ..
make -j8
```

### opensbi
```
export PATH=/xxx/kendryte-toolchain/bin:$PATH
#export CROSS_COMPILE=riscv64-unknown-elf-

make clean
rm -rf build
rm -rf install

make CROSS_COMPILE=riscv64-unknown-elf- PLATFORM=kendryte/k210 -j8
#make CROSS_COMPILE=riscv64-unknown-elf- PLATFORM=kendryte/k210 install

#make CROSS_COMPILE=riscv64-unknown-elf- PLATFORM=generic FW_PAYLOAD_PATH=/xxx/linux-4.20.2/arch/riscv/boot/Image -j8
#make CROSS_COMPILE=riscv64-unknown-elf- PLATFORM=generic FW_PAYLOAD_PATH=/xxx/linux-4.20.2/arch/riscv/boot/Image install

exit 0
###
#FW_PAYLOAD
qemu-system-riscv64 -M virt -m 256M -nographic \
-kernel install/usr/share/opensbi/lp64/generic/firmware/fw_payload.elf \
-drive file=<path_to_linux_rootfs>,format=raw,id=hd0 \
-device virtio-blk-device,drive=hd0 \
-append "root=/dev/vda rw console=ttyS0"

#FW_JUMP
qemu-system-riscv64 -M virt -m 256M -nographic \
	-bios build/platform/generic/firmware/fw_jump.bin \
	-kernel <linux_build_directory>/arch/riscv/boot/Image \
	-drive file=<path_to_linux_rootfs>,format=raw,id=hd0 \
	-device virtio-blk-device,drive=hd0 \
	-append "root=/dev/vda rw console=ttyS0"

# flash firmware
# seek fw_jump.bin 0xc000 = 49152
# dd if=test.bin of=fw_jump_k210.bin bs=1 seek=49152
# OR
# dd if=/xxx/target/riscv64imac-unknown-none-elf/debug/kernel.bin of=fw_jump_k210.bin bs=1 seek=49152

```

### Reference

K210 PSRAM Driver:
https://github.com/QinYUN575/kendryte-standalone-sdk/tree/develop/src/spi_psram

MaixPy MMU test:
https://github.com/sipeed/MaixPy/tree/mmu_test

K210 run GBA emulator with sdcard mmio ceSetupMMU():
https://github.com/44670/mgba-k210/blob/master/k210-port/src/mgba/main.c

