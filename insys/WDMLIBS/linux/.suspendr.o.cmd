cmd_/home/karak/insys/bardy.git/ADP201/L201/../../WDMLIBS/linux/suspendr.o := aarch64-linux-gnu-gcc -Wp,-MMD,/home/karak/insys/bardy.git/ADP201/L201/../../WDMLIBS/linux/.suspendr.o.d  -nostdinc -isystem /opt/xilinx/Vitis/2020.2/gnu/aarch64/lin/aarch64-linux/x86_64-petalinux-linux/usr/bin/aarch64-xilinx-linux/../../lib/aarch64-xilinx-linux/gcc/aarch64-xilinx-linux/9.2.0/include -I/home/karak/xilsrc/linux-xlnx/arch/arm64/include -I./arch/arm64/include/generated -I/home/karak/xilsrc/linux-xlnx/include -I./include -I/home/karak/xilsrc/linux-xlnx/arch/arm64/include/uapi -I./arch/arm64/include/generated/uapi -I/home/karak/xilsrc/linux-xlnx/include/uapi -I./include/generated/uapi -include /home/karak/xilsrc/linux-xlnx/include/linux/kconfig.h -include /home/karak/xilsrc/linux-xlnx/include/linux/compiler_types.h -D__KERNEL__ -mlittle-endian -DKASAN_SHADOW_SCALE_SHIFT=3 -fmacro-prefix-map=/home/karak/xilsrc/linux-xlnx/= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu89 -mgeneral-regs-only -DCONFIG_CC_HAS_K_CONSTRAINT=1 -Wno-psabi -mabi=lp64 -fno-asynchronous-unwind-tables -fno-unwind-tables -mbranch-protection=none -Wa,-march=armv8.4-a -DARM64_ASM_ARCH='"armv8.4-a"' -DKASAN_SHADOW_SCALE_SHIFT=3 -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 --param=allow-store-data-races=0 -Wframe-larger-than=2048 -fstack-protector-strong -Wno-unused-but-set-variable -Wimplicit-fallthrough -Wno-unused-const-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wno-stringop-truncation -Wno-array-bounds -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned -mstack-protector-guard=sysreg -mstack-protector-guard-reg=sp_el0 -mstack-protector-guard-offset=976 -O2 -I/home/karak/insys/bardy.git/ADP201/L201/../../BRDINC -I/home/karak/insys/bardy.git/ADP201/L201/../../WDMLIBS/linux -I/home/karak/insys/bardy.git/ADP201/L201/../../BRDLIBS -I/home/karak/insys/bardy.git/ADP201/L201/..  -DMODULE  -DKBUILD_BASENAME='"suspendr"' -DKBUILD_MODNAME='"l201"' -c -o /home/karak/insys/bardy.git/ADP201/L201/../../WDMLIBS/linux/suspendr.o /home/karak/insys/bardy.git/ADP201/L201/../../WDMLIBS/linux/suspendr.c

source_/home/karak/insys/bardy.git/ADP201/L201/../../WDMLIBS/linux/suspendr.o := /home/karak/insys/bardy.git/ADP201/L201/../../WDMLIBS/linux/suspendr.c

deps_/home/karak/insys/bardy.git/ADP201/L201/../../WDMLIBS/linux/suspendr.o := \
  /home/karak/xilsrc/linux-xlnx/include/linux/kconfig.h \
    $(wildcard include/config/cc/version/text.h) \
    $(wildcard include/config/cpu/big/endian.h) \
    $(wildcard include/config/booger.h) \
    $(wildcard include/config/foo.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/compiler_types.h \
    $(wildcard include/config/have/arch/compiler/h.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/cc/has/asm/inline.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/compiler_attributes.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/compiler-gcc.h \
    $(wildcard include/config/retpoline.h) \
    $(wildcard include/config/arch/use/builtin/bswap.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/compiler.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/atomic/sleep.h) \
    $(wildcard include/config/preempt/rt.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/panic/timeout.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/limits.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/limits.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/types.h \
    $(wildcard include/config/have/uid16.h) \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/types.h \
  arch/arm64/include/generated/uapi/asm/types.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/types.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/int-ll64.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/int-ll64.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/uapi/asm/bitsperlong.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/bitsperlong.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/bitsperlong.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/posix_types.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/stddef.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/stddef.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/compiler_types.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/uapi/asm/posix_types.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/posix_types.h \
  /home/karak/xilsrc/linux-xlnx/include/vdso/limits.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/linkage.h \
    $(wildcard include/config/arch/use/sym/annotations.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/stringify.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/export.h \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/module/rel/crcs.h) \
    $(wildcard include/config/have/arch/prel32/relocations.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/trim/unused/ksyms.h) \
    $(wildcard include/config/unused/symbols.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/stack/validation.h) \
  arch/arm64/include/generated/asm/rwonce.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/rwonce.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/kasan-checks.h \
    $(wildcard include/config/kasan.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/kcsan-checks.h \
    $(wildcard include/config/kcsan.h) \
    $(wildcard include/config/kcsan/ignore/atomics.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/linkage.h \
    $(wildcard include/config/arm64/bti/kernel.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/bitops.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/bits.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/const.h \
  /home/karak/xilsrc/linux-xlnx/include/vdso/const.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/const.h \
  /home/karak/xilsrc/linux-xlnx/include/vdso/bits.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/build_bug.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/bitops.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/bitops/builtin-__ffs.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/bitops/builtin-ffs.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/bitops/builtin-__fls.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/bitops/builtin-fls.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/bitops/ffz.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/bitops/fls64.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/bitops/find.h \
    $(wildcard include/config/generic/find/first/bit.h) \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/bitops/sched.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/bitops/hweight.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/bitops/arch_hweight.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/bitops/const_hweight.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/bitops/atomic.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/atomic.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/atomic.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/barrier.h \
    $(wildcard include/config/arm64/pseudo/nmi.h) \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/barrier.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/cmpxchg.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/lse.h \
    $(wildcard include/config/arm64/lse/atomics.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/atomic_ll_sc.h \
    $(wildcard include/config/cc/has/k/constraint.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/atomic-arch-fallback.h \
    $(wildcard include/config/generic/atomic64.h) \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/atomic-instrumented.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/instrumented.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/atomic-long.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/bitops/lock.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/bitops/non-atomic.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/bitops/le.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/uapi/asm/byteorder.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/byteorder/little_endian.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/byteorder/little_endian.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/swab.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/swab.h \
  arch/arm64/include/generated/uapi/asm/swab.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/swab.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/byteorder/generic.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/bitops/ext2-atomic-setbit.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/minmax.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/typecheck.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/printk.h \
    $(wildcard include/config/message/loglevel/default.h) \
    $(wildcard include/config/console/loglevel/default.h) \
    $(wildcard include/config/console/loglevel/quiet.h) \
    $(wildcard include/config/early/printk.h) \
    $(wildcard include/config/printk/nmi.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
    $(wildcard include/config/dynamic/debug/core.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/init.h \
    $(wildcard include/config/strict/kernel/rwx.h) \
    $(wildcard include/config/strict/module/rwx.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/kern_levels.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/kernel.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/sysinfo.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/cache.h \
    $(wildcard include/config/kasan/sw/tags.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/cputype.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/sysreg.h \
    $(wildcard include/config/broken/gas/inst.h) \
    $(wildcard include/config/arm64/pa/bits/52.h) \
    $(wildcard include/config/arm64/4k/pages.h) \
    $(wildcard include/config/arm64/16k/pages.h) \
    $(wildcard include/config/arm64/64k/pages.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/alternative.h \
    $(wildcard include/config/arm64/uao.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/cpucaps.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/insn.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/ratelimit_types.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/param.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/uapi/asm/param.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/param.h \
    $(wildcard include/config/hz.h) \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/param.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/spinlock_types.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/spinlock_types.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/nr/cpus.h) \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/qrwlock_types.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/lockdep_types.h \
    $(wildcard include/config/prove/raw/lock/nesting.h) \
    $(wildcard include/config/preempt/lock.h) \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/lock/stat.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/rwlock_types.h \
  arch/arm64/include/generated/asm/div64.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/div64.h \
  include/generated/uapi/linux/version.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/module.h \
    $(wildcard include/config/sysfs.h) \
    $(wildcard include/config/modules/tree/lookup.h) \
    $(wildcard include/config/livepatch.h) \
    $(wildcard include/config/module/sig.h) \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/kallsyms.h) \
    $(wildcard include/config/tracepoints.h) \
    $(wildcard include/config/tree/srcu.h) \
    $(wildcard include/config/bpf/events.h) \
    $(wildcard include/config/jump/label.h) \
    $(wildcard include/config/event/tracing.h) \
    $(wildcard include/config/kprobes.h) \
    $(wildcard include/config/have/static/call/inline.h) \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/constructors.h) \
    $(wildcard include/config/function/error/injection.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
    $(wildcard include/config/page/poisoning/zero.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/stat.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/stat.h \
    $(wildcard include/config/compat.h) \
  arch/arm64/include/generated/uapi/asm/stat.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/stat.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/time.h \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
    $(wildcard include/config/posix/timers.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/math64.h \
    $(wildcard include/config/arch/supports/int128.h) \
  /home/karak/xilsrc/linux-xlnx/include/vdso/math64.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/time64.h \
  /home/karak/xilsrc/linux-xlnx/include/vdso/time64.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/time.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/time_types.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/time32.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/timex.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/timex.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/timex.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/arch_timer.h \
    $(wildcard include/config/arm/arch/timer/ool/workaround.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/hwcap.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/uapi/asm/hwcap.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/cpufeature.h \
    $(wildcard include/config/arm64/sw/ttbr0/pan.h) \
    $(wildcard include/config/arm64/sve.h) \
    $(wildcard include/config/arm64/cnp.h) \
    $(wildcard include/config/arm64/ptr/auth.h) \
    $(wildcard include/config/arm64/mte.h) \
    $(wildcard include/config/arm64/debug/priority/masking.h) \
    $(wildcard include/config/arm64/bti.h) \
    $(wildcard include/config/arm64/tlb/range.h) \
    $(wildcard include/config/arm64/pa/bits.h) \
    $(wildcard include/config/arm64/hw/afdbm.h) \
    $(wildcard include/config/arm64/amu/extn.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/bug.h \
    $(wildcard include/config/bug/on/data/corruption.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/bug.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/asm-bug.h \
    $(wildcard include/config/debug/bugverbose.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/brk-imm.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/instrumentation.h \
    $(wildcard include/config/debug/entry.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/jump_label.h \
    $(wildcard include/config/have/arch/jump/label/relative.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/smp.h \
    $(wildcard include/config/up/late/init.h) \
    $(wildcard include/config/debug/preempt.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/errno.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/errno.h \
  arch/arm64/include/generated/uapi/asm/errno.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/errno.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/errno-base.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/cpumask.h \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/threads.h \
    $(wildcard include/config/base/small.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/bitmap.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
    $(wildcard include/config/fortify/source.h) \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/string.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/string.h \
    $(wildcard include/config/arch/has/uaccess/flushcache.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/smp_types.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/llist.h \
    $(wildcard include/config/arch/have/nmi/safe/cmpxchg.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/preempt.h \
    $(wildcard include/config/preempt/count.h) \
    $(wildcard include/config/trace/preempt/toggle.h) \
    $(wildcard include/config/preemption.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/preempt.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/thread_info.h \
    $(wildcard include/config/thread/info/in/task.h) \
    $(wildcard include/config/have/arch/within/stack/frames.h) \
    $(wildcard include/config/hardened/usercopy.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/restart_block.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/current.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/thread_info.h \
    $(wildcard include/config/shadow/call/stack.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/memory.h \
    $(wildcard include/config/arm64/va/bits.h) \
    $(wildcard include/config/kasan/shadow/offset.h) \
    $(wildcard include/config/vmap/stack.h) \
    $(wildcard include/config/debug/virtual.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
    $(wildcard include/config/efi.h) \
    $(wildcard include/config/arm/gic/v3/its.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/sizes.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/page-def.h \
    $(wildcard include/config/arm64/page/shift.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/mmdebug.h \
    $(wildcard include/config/debug/vm.h) \
    $(wildcard include/config/debug/vm/pgflags.h) \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/memory_model.h \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/pfn.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/stack_pointer.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/smp.h \
    $(wildcard include/config/arm64/acpi/parking/protocol.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/percpu.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/percpu.h \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/percpu-defs.h \
    $(wildcard include/config/debug/force/weak/per/cpu.h) \
    $(wildcard include/config/amd/mem/encrypt.h) \
  /home/karak/xilsrc/linux-xlnx/include/clocksource/arm_arch_timer.h \
    $(wildcard include/config/arm/arch/timer.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/timecounter.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/timex.h \
  /home/karak/xilsrc/linux-xlnx/include/vdso/time32.h \
  /home/karak/xilsrc/linux-xlnx/include/vdso/time.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/compat.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/compat.h \
    $(wildcard include/config/compat/for/u64/alignment.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/sched.h \
    $(wildcard include/config/virt/cpu/accounting/native.h) \
    $(wildcard include/config/sched/info.h) \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/fair/group/sched.h) \
    $(wildcard include/config/rt/group/sched.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/uclamp/task.h) \
    $(wildcard include/config/uclamp/buckets/count.h) \
    $(wildcard include/config/cgroup/sched.h) \
    $(wildcard include/config/blk/dev/io/trace.h) \
    $(wildcard include/config/preempt/rcu.h) \
    $(wildcard include/config/tasks/rcu.h) \
    $(wildcard include/config/tasks/trace/rcu.h) \
    $(wildcard include/config/psi.h) \
    $(wildcard include/config/memcg.h) \
    $(wildcard include/config/compat/brk.h) \
    $(wildcard include/config/cgroups.h) \
    $(wildcard include/config/blk/cgroup.h) \
    $(wildcard include/config/stackprotector.h) \
    $(wildcard include/config/arch/has/scaled/cputime.h) \
    $(wildcard include/config/virt/cpu/accounting/gen.h) \
    $(wildcard include/config/no/hz/full.h) \
    $(wildcard include/config/posix/cputimers.h) \
    $(wildcard include/config/posix/cpu/timers/task/work.h) \
    $(wildcard include/config/keys.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/detect/hung/task.h) \
    $(wildcard include/config/io/uring.h) \
    $(wildcard include/config/audit.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/debug/mutexes.h) \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/ubsan.h) \
    $(wildcard include/config/ubsan/trap.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/compaction.h) \
    $(wildcard include/config/task/xacct.h) \
    $(wildcard include/config/cpusets.h) \
    $(wildcard include/config/x86/cpu/resctrl.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/perf/events.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/numa/balancing.h) \
    $(wildcard include/config/rseq.h) \
    $(wildcard include/config/task/delay/acct.h) \
    $(wildcard include/config/fault/injection.h) \
    $(wildcard include/config/latencytop.h) \
    $(wildcard include/config/kunit.h) \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/kcov.h) \
    $(wildcard include/config/uprobes.h) \
    $(wildcard include/config/bcache.h) \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/gcc/plugin/stackleak.h) \
    $(wildcard include/config/x86/mce.h) \
    $(wildcard include/config/arch/task/struct/on/stack.h) \
    $(wildcard include/config/debug/rseq.h) \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/sched.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/pid.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/rculist.h \
    $(wildcard include/config/prove/rcu/list.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/rcupdate.h \
    $(wildcard include/config/tiny/rcu.h) \
    $(wildcard include/config/rcu/stall/common.h) \
    $(wildcard include/config/rcu/nocb/cpu.h) \
    $(wildcard include/config/tasks/rcu/generic.h) \
    $(wildcard include/config/tasks/rcu/trace.h) \
    $(wildcard include/config/tasks/rude/rcu.h) \
    $(wildcard include/config/tree/rcu.h) \
    $(wildcard include/config/debug/objects/rcu/head.h) \
    $(wildcard include/config/prove/rcu.h) \
    $(wildcard include/config/rcu/boost.h) \
    $(wildcard include/config/arch/weak/release/acquire.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/irqflags.h \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/irqflags.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/ptrace.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/uapi/asm/ptrace.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/uapi/asm/sve_context.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/bottom_half.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/lockdep.h \
    $(wildcard include/config/debug/locking/api/selftests.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/processor.h \
    $(wildcard include/config/kuser/helpers.h) \
    $(wildcard include/config/arm64/force/52bit.h) \
    $(wildcard include/config/have/hw/breakpoint.h) \
    $(wildcard include/config/arm64/tagged/addr/abi.h) \
  /home/karak/xilsrc/linux-xlnx/include/vdso/processor.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/vdso/processor.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/hw_breakpoint.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/virt.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/sections.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/sections.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/kasan.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/pgtable-types.h \
    $(wildcard include/config/pgtable/levels.h) \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/pgtable-nopud.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/pgtable-nop4d.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/pgtable-hwdef.h \
    $(wildcard include/config/arm64/cont/pte/shift.h) \
    $(wildcard include/config/arm64/cont/pmd/shift.h) \
    $(wildcard include/config/arm64/va/bits/52.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/pointer_auth.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/random.h \
    $(wildcard include/config/arch/random.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/once.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/random.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/ioctl.h \
  arch/arm64/include/generated/uapi/asm/ioctl.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/ioctl.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/ioctl.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/irqnr.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/irqnr.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/prandom.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/percpu.h \
    $(wildcard include/config/need/per/cpu/embed/first/chunk.h) \
    $(wildcard include/config/need/per/cpu/page/first/chunk.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/archrandom.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/spectre.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/fpsimd.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/uapi/asm/sigcontext.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/rcutree.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/wait.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/spinlock.h \
  arch/arm64/include/generated/asm/mmiowb.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/mmiowb.h \
    $(wildcard include/config/mmiowb.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/spinlock.h \
  arch/arm64/include/generated/asm/qrwlock.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/qrwlock.h \
  arch/arm64/include/generated/asm/qspinlock.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/qspinlock.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/rwlock.h \
    $(wildcard include/config/preempt.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/spinlock_api_smp.h \
    $(wildcard include/config/inline/spin/lock.h) \
    $(wildcard include/config/inline/spin/lock/bh.h) \
    $(wildcard include/config/inline/spin/lock/irq.h) \
    $(wildcard include/config/inline/spin/lock/irqsave.h) \
    $(wildcard include/config/inline/spin/trylock.h) \
    $(wildcard include/config/inline/spin/trylock/bh.h) \
    $(wildcard include/config/uninline/spin/unlock.h) \
    $(wildcard include/config/inline/spin/unlock/bh.h) \
    $(wildcard include/config/inline/spin/unlock/irq.h) \
    $(wildcard include/config/inline/spin/unlock/irqrestore.h) \
    $(wildcard include/config/generic/lockbreak.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/rwlock_api_smp.h \
    $(wildcard include/config/inline/read/lock.h) \
    $(wildcard include/config/inline/write/lock.h) \
    $(wildcard include/config/inline/read/lock/bh.h) \
    $(wildcard include/config/inline/write/lock/bh.h) \
    $(wildcard include/config/inline/read/lock/irq.h) \
    $(wildcard include/config/inline/write/lock/irq.h) \
    $(wildcard include/config/inline/read/lock/irqsave.h) \
    $(wildcard include/config/inline/write/lock/irqsave.h) \
    $(wildcard include/config/inline/read/trylock.h) \
    $(wildcard include/config/inline/write/trylock.h) \
    $(wildcard include/config/inline/read/unlock.h) \
    $(wildcard include/config/inline/write/unlock.h) \
    $(wildcard include/config/inline/read/unlock/bh.h) \
    $(wildcard include/config/inline/write/unlock/bh.h) \
    $(wildcard include/config/inline/read/unlock/irq.h) \
    $(wildcard include/config/inline/write/unlock/irq.h) \
    $(wildcard include/config/inline/read/unlock/irqrestore.h) \
    $(wildcard include/config/inline/write/unlock/irqrestore.h) \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/wait.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/refcount.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/sem.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/sem.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/ipc.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/uidgid.h \
    $(wildcard include/config/multiuser.h) \
    $(wildcard include/config/user/ns.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/highuid.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/rhashtable-types.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/mutex.h \
    $(wildcard include/config/mutex/spin/on/owner.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/osq_lock.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/debug_locks.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/workqueue.h \
    $(wildcard include/config/debug/objects/work.h) \
    $(wildcard include/config/freezer.h) \
    $(wildcard include/config/wq/watchdog.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/timer.h \
    $(wildcard include/config/debug/objects/timers.h) \
    $(wildcard include/config/no/hz/common.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/ktime.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/jiffies.h \
  /home/karak/xilsrc/linux-xlnx/include/vdso/jiffies.h \
  include/generated/timeconst.h \
  /home/karak/xilsrc/linux-xlnx/include/vdso/ktime.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/timekeeping.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/timekeeping32.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/debug/objects/free.h) \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/ipc.h \
  arch/arm64/include/generated/uapi/asm/ipcbuf.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/ipcbuf.h \
  arch/arm64/include/generated/uapi/asm/sembuf.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/sembuf.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/shm.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/page.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/personality.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/personality.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/getorder.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/shm.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/hugetlb_encode.h \
  arch/arm64/include/generated/uapi/asm/shmbuf.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/shmbuf.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/shmparam.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/shmparam.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/kcov.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/kcov.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/plist.h \
    $(wildcard include/config/debug/plist.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/hrtimer.h \
    $(wildcard include/config/high/res/timers.h) \
    $(wildcard include/config/time/low/res.h) \
    $(wildcard include/config/timerfd.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/hrtimer_defs.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/rbtree.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/seqlock.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/ww_mutex.h \
    $(wildcard include/config/debug/ww/mutex/slowpath.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/timerqueue.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/seccomp.h \
    $(wildcard include/config/seccomp.h) \
    $(wildcard include/config/have/arch/seccomp/filter.h) \
    $(wildcard include/config/seccomp/filter.h) \
    $(wildcard include/config/checkpoint/restore.h) \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/seccomp.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/seccomp.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/unistd.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/uapi/asm/unistd.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/unistd.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/seccomp.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/unistd.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/nodemask.h \
    $(wildcard include/config/highmem.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
    $(wildcard include/config/numa/keep/meminfo.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/resource.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/resource.h \
  arch/arm64/include/generated/uapi/asm/resource.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/resource.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/resource.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/latencytop.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/sched/prio.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/sched/types.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/signal_types.h \
    $(wildcard include/config/old/sigaction.h) \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/signal.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/uapi/asm/signal.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/signal.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/signal.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/signal-defs.h \
  arch/arm64/include/generated/uapi/asm/siginfo.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/siginfo.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/mm_types_task.h \
    $(wildcard include/config/arch/want/batched/unmap/tlb/flush.h) \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/arch/enable/split/pmd/ptlock.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/task_io_accounting.h \
    $(wildcard include/config/task/io/accounting.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/posix-timers.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/alarmtimer.h \
    $(wildcard include/config/rtc/class.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/task_work.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/rseq.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/kcsan.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/sched/task_stack.h \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/debug/stack/usage.h) \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/magic.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/stat.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/kmod.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/umh.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/gfp.h \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/zone/device.h) \
    $(wildcard include/config/pm/sleep.h) \
    $(wildcard include/config/contig/alloc.h) \
    $(wildcard include/config/cma.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/memory/isolation.h) \
    $(wildcard include/config/zsmalloc.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/transparent/hugepage.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/page/extension.h) \
    $(wildcard include/config/deferred/struct/page/init.h) \
    $(wildcard include/config/have/memoryless/nodes.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/memory/hotremove.h) \
    $(wildcard include/config/have/arch/pfn/valid.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/page-flags-layout.h \
  include/generated/bounds.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/sparsemem.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/mm_types.h \
    $(wildcard include/config/have/aligned/struct/page.h) \
    $(wildcard include/config/userfaultfd.h) \
    $(wildcard include/config/swap.h) \
    $(wildcard include/config/have/arch/compat/mmap/bases.h) \
    $(wildcard include/config/membarrier.h) \
    $(wildcard include/config/aio.h) \
    $(wildcard include/config/mmu/notifier.h) \
    $(wildcard include/config/iommu/support.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/auxvec.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/auxvec.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/uapi/asm/auxvec.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/rwsem.h \
    $(wildcard include/config/rwsem/spin/on/owner.h) \
    $(wildcard include/config/debug/rwsems.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/err.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/completion.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/swait.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/uprobes.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/mmu.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/page-flags.h \
    $(wildcard include/config/arch/uses/pg/uncached.h) \
    $(wildcard include/config/memory/failure.h) \
    $(wildcard include/config/idle/page/tracking.h) \
    $(wildcard include/config/thp/swap.h) \
    $(wildcard include/config/ksm.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/memory_hotplug.h \
    $(wildcard include/config/arch/has/add/pages.h) \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
    $(wildcard include/config/have/bootmem/info/node.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/notifier.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/srcu.h \
    $(wildcard include/config/tiny/srcu.h) \
    $(wildcard include/config/srcu.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/rcu_segcblist.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/srcutree.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/rcu_node_tree.h \
    $(wildcard include/config/rcu/fanout.h) \
    $(wildcard include/config/rcu/fanout/leaf.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/topology.h \
    $(wildcard include/config/use/percpu/numa/node/id.h) \
    $(wildcard include/config/sched/smt.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/arch_topology.h \
    $(wildcard include/config/generic/arch/topology.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/topology.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/topology.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/sysctl.h \
    $(wildcard include/config/sysctl.h) \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/sysctl.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/elf.h \
    $(wildcard include/config/arch/use/gnu/property.h) \
    $(wildcard include/config/arch/have/elf/prot.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/elf.h \
    $(wildcard include/config/compat/vdso.h) \
  arch/arm64/include/generated/asm/user.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/user.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/elf.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/elf-em.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/fs.h \
    $(wildcard include/config/read/only/thp/for/fs.h) \
    $(wildcard include/config/fs/posix/acl.h) \
    $(wildcard include/config/cgroup/writeback.h) \
    $(wildcard include/config/ima.h) \
    $(wildcard include/config/file/locking.h) \
    $(wildcard include/config/fsnotify.h) \
    $(wildcard include/config/fs/encryption.h) \
    $(wildcard include/config/fs/verity.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/unicode.h) \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/fs/dax.h) \
    $(wildcard include/config/mandatory/file/locking.h) \
    $(wildcard include/config/migration.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/wait_bit.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/kdev_t.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/kdev_t.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/dcache.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/rculist_bl.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/list_bl.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/bit_spinlock.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/lockref.h \
    $(wildcard include/config/arch/use/cmpxchg/lockref.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/stringhash.h \
    $(wildcard include/config/dcache/word/access.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/hash.h \
    $(wildcard include/config/have/arch/hash.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/path.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/list_lru.h \
    $(wildcard include/config/memcg/kmem.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/shrinker.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/radix-tree.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/xarray.h \
    $(wildcard include/config/xarray/multi.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/kconfig.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/local_lock.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/local_lock_internal.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/capability.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/capability.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/semaphore.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/fcntl.h \
    $(wildcard include/config/arch/32bit/off/t.h) \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/fcntl.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/uapi/asm/fcntl.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/fcntl.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/openat2.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/migrate_mode.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/percpu-rwsem.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/rcuwait.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/sched/signal.h \
    $(wildcard include/config/sched/autogroup.h) \
    $(wildcard include/config/bsd/process/acct.h) \
    $(wildcard include/config/taskstats.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/signal.h \
    $(wildcard include/config/proc/fs.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/sched/jobctl.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/sched/task.h \
    $(wildcard include/config/have/exit/thread.h) \
    $(wildcard include/config/arch/wants/dynamic/task/struct.h) \
    $(wildcard include/config/have/arch/thread/struct/whitelist.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/uaccess.h \
    $(wildcard include/config/set/fs.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/fault-inject-usercopy.h \
    $(wildcard include/config/fault/injection/usercopy.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/uaccess.h \
    $(wildcard include/config/arm64/pan.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/kernel-pgtable.h \
    $(wildcard include/config/randomize/base.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/extable.h \
    $(wildcard include/config/bpf/jit.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/cred.h \
    $(wildcard include/config/debug/credentials.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/key.h \
    $(wildcard include/config/key/notifications.h) \
    $(wildcard include/config/net.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/assoc_array.h \
    $(wildcard include/config/associative/array.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/sched/user.h \
    $(wildcard include/config/fanotify.h) \
    $(wildcard include/config/posix/mqueue.h) \
    $(wildcard include/config/bpf/syscall.h) \
    $(wildcard include/config/watch/queue.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/ratelimit.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/rcu_sync.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/delayed_call.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/uuid.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/uuid.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/errseq.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/ioprio.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/sched/rt.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/iocontext.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/fs_types.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/fs.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/quota.h \
    $(wildcard include/config/quota/netlink/interface.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/percpu_counter.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/dqblk_xfs.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/dqblk_v1.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/dqblk_v2.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/dqblk_qtree.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/projid.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/quota.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/nfs_fs_i.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/kobject.h \
    $(wildcard include/config/uevent/helper.h) \
    $(wildcard include/config/debug/kobject/release.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/sysfs.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/kernfs.h \
    $(wildcard include/config/kernfs.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/idr.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/kobject_ns.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/kref.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/moduleparam.h \
    $(wildcard include/config/alpha.h) \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/ppc64.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/rbtree_latch.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/error-injection.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/error-injection.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/tracepoint-defs.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/static_key.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/static_call_types.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/module.h \
    $(wildcard include/config/arm64/module/plts.h) \
    $(wildcard include/config/dynamic/ftrace.h) \
    $(wildcard include/config/arm64/erratum/843419.h) \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/module.h \
    $(wildcard include/config/have/mod/arch/specific.h) \
    $(wildcard include/config/modules/use/elf/rel.h) \
    $(wildcard include/config/modules/use/elf/rela.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/pagemap.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/mm.h \
    $(wildcard include/config/have/arch/mmap/rnd/bits.h) \
    $(wildcard include/config/have/arch/mmap/rnd/compat/bits.h) \
    $(wildcard include/config/mem/soft/dirty.h) \
    $(wildcard include/config/arch/uses/high/vma/flags.h) \
    $(wildcard include/config/arch/has/pkeys.h) \
    $(wildcard include/config/ppc.h) \
    $(wildcard include/config/x86.h) \
    $(wildcard include/config/parisc.h) \
    $(wildcard include/config/sparc64.h) \
    $(wildcard include/config/arm64.h) \
    $(wildcard include/config/shmem.h) \
    $(wildcard include/config/dev/pagemap/ops.h) \
    $(wildcard include/config/device/private.h) \
    $(wildcard include/config/pci/p2pdma.h) \
    $(wildcard include/config/arch/has/pte/special.h) \
    $(wildcard include/config/arch/has/pte/devmap.h) \
    $(wildcard include/config/debug/vm/rb.h) \
    $(wildcard include/config/page/poisoning.h) \
    $(wildcard include/config/init/on/alloc/default/on.h) \
    $(wildcard include/config/init/on/free/default/on.h) \
    $(wildcard include/config/debug/pagealloc.h) \
    $(wildcard include/config/arch/has/set/direct/map.h) \
    $(wildcard include/config/hibernation.h) \
    $(wildcard include/config/hugetlbfs.h) \
    $(wildcard include/config/mapping/dirty/helpers.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/mmap_lock.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/range.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/percpu-refcount.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/page_ext.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/stacktrace.h \
    $(wildcard include/config/stacktrace.h) \
    $(wildcard include/config/arch/stackwalk.h) \
    $(wildcard include/config/have/reliable/stacktrace.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/stackdepot.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/page_ref.h \
    $(wildcard include/config/debug/page/ref.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/memremap.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/ioport.h \
    $(wildcard include/config/io/strict/devmem.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/overflow.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/pgtable.h \
    $(wildcard include/config/highpte.h) \
    $(wildcard include/config/have/arch/transparent/hugepage/pud.h) \
    $(wildcard include/config/have/arch/soft/dirty.h) \
    $(wildcard include/config/arch/enable/thp/migration.h) \
    $(wildcard include/config/have/arch/huge/vmap.h) \
    $(wildcard include/config/x86/espfix64.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/pgtable.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/proc-fns.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/mte.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/pgtable-prot.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/tlbflush.h \
    $(wildcard include/config/arm64/workaround/repeat/tlbi.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/bitfield.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/fixmap.h \
    $(wildcard include/config/acpi/apei/ghes.h) \
    $(wildcard include/config/arm/sde/interface.h) \
    $(wildcard include/config/unmap/kernel/at/el0.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/boot.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/fixmap.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/pgtable_uffd.h \
    $(wildcard include/config/have/arch/userfaultfd/wp.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/huge_mm.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/sched/coredump.h \
    $(wildcard include/config/core/dump/default/elf/headers.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/vmstat.h \
    $(wildcard include/config/vm/event/counters.h) \
    $(wildcard include/config/debug/tlbflush.h) \
    $(wildcard include/config/debug/vm/vmacache.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/vm_event_item.h \
    $(wildcard include/config/memory/balloon.h) \
    $(wildcard include/config/balloon/compaction.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/highmem.h \
    $(wildcard include/config/x86/32.h) \
    $(wildcard include/config/debug/highmem.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/hardirq.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/context_tracking_state.h \
    $(wildcard include/config/context/tracking.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/ftrace_irq.h \
    $(wildcard include/config/hwlat/tracer.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/vtime.h \
    $(wildcard include/config/virt/cpu/accounting.h) \
    $(wildcard include/config/irq/time/accounting.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/hardirq.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/irq.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/irq.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/kvm_arm.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/esr.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/irq_cpustat.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/cacheflush.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/kgdb.h \
    $(wildcard include/config/have/arch/kgdb.h) \
    $(wildcard include/config/kgdb.h) \
    $(wildcard include/config/serial/kgdb/nmi.h) \
    $(wildcard include/config/kgdb/honour/blocklist.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/kprobes.h \
    $(wildcard include/config/kretprobes.h) \
    $(wildcard include/config/kprobes/sanity/test.h) \
    $(wildcard include/config/optprobes.h) \
    $(wildcard include/config/kprobes/on/ftrace.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/ftrace.h \
    $(wildcard include/config/function/tracer.h) \
    $(wildcard include/config/dynamic/ftrace/with/regs.h) \
    $(wildcard include/config/dynamic/ftrace/with/direct/calls.h) \
    $(wildcard include/config/have/dynamic/ftrace/with/direct/calls.h) \
    $(wildcard include/config/stack/tracer.h) \
    $(wildcard include/config/frame/pointer.h) \
    $(wildcard include/config/function/profiler.h) \
    $(wildcard include/config/ftrace/syscalls.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/trace_clock.h \
  arch/arm64/include/generated/asm/trace_clock.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/trace_clock.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/kallsyms.h \
    $(wildcard include/config/kallsyms/all.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/ptrace.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/pid_namespace.h \
    $(wildcard include/config/pid/ns.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/nsproxy.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/ns_common.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/ptrace.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/ftrace.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/compat.h \
    $(wildcard include/config/arch/has/syscall/wrapper.h) \
    $(wildcard include/config/x86/x32/abi.h) \
    $(wildcard include/config/compat/old/sigaction.h) \
    $(wildcard include/config/odd/rt/sigaction.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/socket.h \
  arch/arm64/include/generated/uapi/asm/socket.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/socket.h \
  arch/arm64/include/generated/uapi/asm/sockios.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/asm-generic/sockios.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/sockios.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/uio.h \
    $(wildcard include/config/arch/has/copy/mc.h) \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/uio.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/socket.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/if.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/libc-compat.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/hdlc/ioctl.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/aio_abi.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/syscall_wrapper.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/kprobes.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/kprobes.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/kgdb.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/debug-monitors.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/cacheflush.h \
  arch/arm64/include/generated/asm/kmap_types.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/kmap_types.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/hugetlb_inline.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/pci.h \
    $(wildcard include/config/pci/iov.h) \
    $(wildcard include/config/pcieaer.h) \
    $(wildcard include/config/pcieaspm.h) \
    $(wildcard include/config/hotplug/pci/pcie.h) \
    $(wildcard include/config/pcie/ptm.h) \
    $(wildcard include/config/pci/msi.h) \
    $(wildcard include/config/pcie/dpc.h) \
    $(wildcard include/config/pci/ats.h) \
    $(wildcard include/config/pci/pri.h) \
    $(wildcard include/config/pci/pasid.h) \
    $(wildcard include/config/pci/domains/generic.h) \
    $(wildcard include/config/pci.h) \
    $(wildcard include/config/pcieportbus.h) \
    $(wildcard include/config/pci/domains.h) \
    $(wildcard include/config/acpi.h) \
    $(wildcard include/config/pci/quirks.h) \
    $(wildcard include/config/pci/mmconfig.h) \
    $(wildcard include/config/acpi/mcfg.h) \
    $(wildcard include/config/hotplug/pci.h) \
    $(wildcard include/config/of.h) \
    $(wildcard include/config/eeh.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/mod_devicetable.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/device.h \
    $(wildcard include/config/debug/devres.h) \
    $(wildcard include/config/energy/model.h) \
    $(wildcard include/config/generic/msi/irq/domain.h) \
    $(wildcard include/config/pinctrl.h) \
    $(wildcard include/config/generic/msi/irq.h) \
    $(wildcard include/config/dma/ops.h) \
    $(wildcard include/config/dma/declare/coherent.h) \
    $(wildcard include/config/dma/cma.h) \
    $(wildcard include/config/arch/has/sync/dma/for/device.h) \
    $(wildcard include/config/arch/has/sync/dma/for/cpu.h) \
    $(wildcard include/config/arch/has/sync/dma/for/cpu/all.h) \
    $(wildcard include/config/dma/ops/bypass.h) \
    $(wildcard include/config/devtmpfs.h) \
    $(wildcard include/config/sysfs/deprecated.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/dev_printk.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/energy_model.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/sched/cpufreq.h \
    $(wildcard include/config/cpu/freq.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/sched/topology.h \
    $(wildcard include/config/sched/debug.h) \
    $(wildcard include/config/sched/mc.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/sched/idle.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/sched/sd_flags.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/klist.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/pm.h \
    $(wildcard include/config/vt/console/sleep.h) \
    $(wildcard include/config/pm.h) \
    $(wildcard include/config/pm/clk.h) \
    $(wildcard include/config/pm/generic/domains.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/device/bus.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/device/class.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/device/driver.h \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/device.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/pm_wakeup.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/interrupt.h \
    $(wildcard include/config/irq/forced/threading.h) \
    $(wildcard include/config/generic/irq/probe.h) \
    $(wildcard include/config/irq/timings.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/irqreturn.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/io.h \
    $(wildcard include/config/has/ioport/map.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/io.h \
  arch/arm64/include/generated/asm/early_ioremap.h \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/early_ioremap.h \
    $(wildcard include/config/generic/early/ioremap.h) \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/io.h \
    $(wildcard include/config/generic/iomap.h) \
    $(wildcard include/config/generic/ioremap.h) \
    $(wildcard include/config/virt/to/bus.h) \
  /home/karak/xilsrc/linux-xlnx/include/asm-generic/pci_iomap.h \
    $(wildcard include/config/no/generic/pci/ioport/map.h) \
    $(wildcard include/config/generic/pci/iomap.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/logic_pio.h \
    $(wildcard include/config/indirect/pio.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/fwnode.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/vmalloc.h \
    $(wildcard include/config/kasan/vmalloc.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/vmalloc.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/resource_ext.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/slab.h \
    $(wildcard include/config/debug/slab.h) \
    $(wildcard include/config/failslab.h) \
    $(wildcard include/config/have/hardened/usercopy/allocator.h) \
    $(wildcard include/config/slab.h) \
    $(wildcard include/config/slub.h) \
    $(wildcard include/config/slob.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/kasan.h \
    $(wildcard include/config/kasan/generic.h) \
    $(wildcard include/config/kasan/inline.h) \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/pci.h \
  /home/karak/xilsrc/linux-xlnx/include/uapi/linux/pci_regs.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/pci_ids.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/dmapool.h \
    $(wildcard include/config/has/dma.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/scatterlist.h \
    $(wildcard include/config/need/sg/dma/length.h) \
    $(wildcard include/config/debug/sg.h) \
    $(wildcard include/config/sgl/alloc.h) \
    $(wildcard include/config/arch/no/sg/chain.h) \
    $(wildcard include/config/sg/pool.h) \
  /home/karak/xilsrc/linux-xlnx/arch/arm64/include/asm/pci.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/dma-mapping.h \
    $(wildcard include/config/dma/api/debug.h) \
    $(wildcard include/config/need/dma/map/state.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/dma-direction.h \
  /home/karak/xilsrc/linux-xlnx/include/linux/mem_encrypt.h \
    $(wildcard include/config/arch/has/mem/encrypt.h) \
  /home/karak/xilsrc/linux-xlnx/include/linux/pci-dma-compat.h \
  /home/karak/insys/bardy.git/ADP201/L201/../../WDMLIBS/linux/suspendr.h \

/home/karak/insys/bardy.git/ADP201/L201/../../WDMLIBS/linux/suspendr.o: $(deps_/home/karak/insys/bardy.git/ADP201/L201/../../WDMLIBS/linux/suspendr.o)

$(deps_/home/karak/insys/bardy.git/ADP201/L201/../../WDMLIBS/linux/suspendr.o):
