### Build zCore ELF to Emulator zbi package (原生)

* 处理编译生成的zCore elf镜像
`../prebuilt/third_party/clang/linux-x64/bin/llvm-objcopy -O binary ./zcore.elf zcore.bin`

* 对重定位段进行处理，以在内核地址随机化的正确引导
`./gen-kaslr-fixups.sh ../prebuilt/third_party/clang/linux-x64/bin/llvm-readelf ../prebuilt/third_party/clang/linux-x64/bin/llvm-objdump ./zcore.elf ./kernel-fixups.inc`

* 这里具体是显示重定位段relocation section的地址符号等内容
`../../prebuilt/third_party/clang/linux-x64/bin/llvm-readelf -W -r kernel-x64-clang/obj/kernel/zircon.elf`
如：
```
Relocation section '.rela.text' at offset 0x21b710 contains 71804 entries:
    Offset             Info             Type               Symbol's Value  Symbol's Name + Addend
ffffffff80101199  00003a7e00000004 R_X86_64_PLT32         ffffffff8028ba40 current_time - 4
ffffffff801011b1  00003a7e00000004 R_X86_64_PLT32         ffffffff8028ba40 current_time - 4
ffffffff8010122e  00001ed600000004 R_X86_64_PLT32         ffffffff801c1d10 platform_panic_start - 4
```

* 然后把处理过的重定位段的地址，放入叫image的汇编中，以再次处理。后生成kernel.zbi
```
cd default.zircon
../../prebuilt/third_party/clang/linux-x64/bin/clang -o ../image.image.S.o -DTINY -D_LIBCPP_DISABLE_EXTERN_TEMPLATE -D_LIBCPP_HAS_NO_PLATFORM_WAIT -D_LIBCPP_HAS_THREAD_API_EXTERNAL -D_LIBCPP_HAS_THREAD_LIBRARY_EXTERNAL -D_LIBCPP_STD_VER=20 -DTOOLCHAIN_VERSION=MQXdartdnGbQico1_hNwXV_Z0AhkIdQMCQBwd-ycQ5wC -DZX_ASSERT_LEVEL=2 -D_LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS=1 -D_LIBCPP_DISABLE_VISIBILITY_ANNOTATIONS -DKERNEL_BASE=0xffffffff80100000 -DSMP_MAX_CPUS=32 -D_KERNEL -DLK -DENABLE_PANIC_SHELL -DZIRCON_TOOLCHAIN -DLK_DEBUGLEVEL=2 -DDEBUG_PRINT_LEVEL=2 -DWITH_KERNEL_PCIE -DSCHEDULER_TRACING_LEVEL=0 -DKERNEL_RETPOLINE=1 -DARCH_X86 -DKERNEL_LOAD_OFFSET=0x00100000 -DWITH_FRAME_POINTERS=1 -DWITH_FRAME_POINTERS=1 -DBOOT_HEADER_SIZE=0x50 -DFIDL_TRACE_LEVEL=0 -I../../zircon/kernel/include -I../../zircon/kernel/arch/x86/include -I../../zircon/system/public -I../ --target=x86_64-fuchsia -mcx16 -march=x86-64 -fcrash-diagnostics-dir=clang-crashreports -fcolor-diagnostics -fdebug-compilation-dir . -no-canonical-prefixes -g3 -c ../../zircon/kernel/arch/x86/image.S
../../prebuilt/third_party/clang/linux-x64/bin/clang++ -o ../image -Wl,-T,../../zircon/kernel/image.ld -Wl,--just-symbols,.././zcore.elf -fuse-ld=lld -Wl,-z,combreloc -Wl,-z,relro -Wl,-z,now -Wl,-z,text -Wl,--pack-dyn-relocs=relr --target=x86_64-fuchsia -mcx16 -march=x86-64 -fcrash-diagnostics-dir=clang-crashreports -fcolor-diagnostics -Wl,--color-diagnostics -Wl,-z,max-page-size=4096 -fdebug-compilation-dir . -no-canonical-prefixes -O2 -g3 -Wl,--icf=all -fno-exceptions -fno-rtti -nostdlib -static -Wl,--no-pie -noprofilelib -fno-sanitize-link-runtime -fno-exceptions -fdata-sections -ffunction-sections -Wl,--gc-sections -fdata-sections -Wl,--build-id=none -Wl,-defsym,KERNEL_BASE=0xffffffff80100000 -Wl,-defsym,SMP_MAX_CPUS=32 -Wl,-defsym,BOOT_HEADER_SIZE=0x50 -Wl,-Map,../image.map -Wl,--start-group ../image.image.S.o -Wl,--end-group
../../prebuilt/third_party/clang/linux-x64/bin/llvm-objcopy -O binary ../image ../kernel.zbi
```

* 最后生成fuchsia.zbi用于原生模拟器运行：
```
cd ../default
../../build/unification/zbi/run_zbi.py --zbi host_x64/zbi --depfile ../fuchsia.zbi.d --rspfile ../fuchsia.zbi.rsp --output ../fuchsia.zbi --json-output ../fuchsia.zbi.json --complete=x64 --compressed=zstd
```

### 非原生方式，使用zCore EFI的方式
* Boot issues
```
dso: id=8e0b430167c47618 base=0x0000018200000000 name=libc.so
{{{module:0xa:libunwind.so.1:elf:dc66c3d80e22e1eb}}}
{{{mmap:0x18200280000:0x4000:load:0xa:r:0}}}
{{{mmap:0x18200284000:0x6000:load:0xa:rx:0x4000}}}
{{{mmap:0x1820028a000:0x1000:load:0xa:rw:0xa000}}}
{{{mmap:0x1820028b000:0x1000:load:0xa:rw:0xb000}}}
dso: id=dc66c3d80e22e1eb base=0x0000018200280000 name=libunwind.so.1
[11.629838098s ERROR 0 0:0]

panicked at 'Unhandled interrupt 6 TrapFrame {
    rax: 0xffffff00015aa880,
    rbx: 0x11,
    rcx: 0xffffff01001fc501,
    rdx: 0x1,
    rsi: 0x1,
    rdi: 0xffffff01001fc566,
    rbp: 0xffffff01001fc510,
    rsp: 0xffffff01001fc4d8,
    r8: 0x0,
    r9: 0x2,
    r10: 0xfffffffffffffffe,
    r11: 0xffffff00015b5228,
    r12: 0x0,
    r13: 0x0,
    r14: 0xffffffffffffff48,
    r15: 0xfefefefefefefeff,
    _pad: 0x0,
    trap_num: 0x6,
    error_code: 0x0,
    rip: 0xffffff000021cb8c,
    cs: 0x58,
    rflags: 0x86,
}', /home/os/rust/zCore/kernel-hal-bare/src/arch/x86_64/interrupt.rs:74:14
[11.636896749s ERROR 0 0:0] KCounters {
    "exceptions.user": 508,
    "exceptions.timer": 95,
    "exceptions.pgfault": 0,
    "Channel.create": 6,
    "Channel.destroy": 3,
    "Timer.create": 0,
    "Timer.destroy": 0,
    "Event.create": 0,
    "Event.destroy": 0,
    "Thread.create": 2,
    "Thread.destroy": 0,
    "Process.create": 2,
    "Process.destroy": 0,
    "EventPair.create": 0,
    "EventPair.destroy": 0,
    "Job.create": 1,
    "Job.destroy": 0,
    "vmo.page_alloc": 14419,
    "vmo.page_dealloc": 3901,
    "VmObject.create": 45,
    "VmObject.destroy": 2,
    "VmAddressRegion.create": 16,
    "VmAddressRegion.destroy": 1,
}
```
