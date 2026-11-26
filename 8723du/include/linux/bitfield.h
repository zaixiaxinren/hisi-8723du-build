/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Backport of <linux/bitfield.h> for kernels older than 4.12
 *
 * The Realtek rtl8723du driver includes <linux/bitfield.h> unconditionally,
 * but that header only exists in Linux >= 4.12.  On older kernels the
 * #include would be a hard compile error.
 *
 * This file lives in the driver's own include/ directory, which is on the
 * include path via -I$(src)/include.  On kernels that do provide the real
 * header, the kernel's -I paths come first, so this file is never reached.
 * The include guard and the per-macro #ifndef checks keep it harmless either
 * way.
 */
#ifndef __LINUX_BITFIELD_COMPAT_H
#define __LINUX_BITFIELD_COMPAT_H

#include <linux/types.h>
#include <linux/compiler.h>

/*
 * Compute the shift of a single-bit (or contiguous) mask.
 * Same definition as the mainline header.
 */
#ifndef __bf_shf
#define __bf_shf(x) (__builtin_ffsll(x) - 1)
#endif

#ifndef FIELD_GET
#define FIELD_GET(_mask, _reg)						\
	({								\
		typeof(_mask) ____mask = (_mask);			\
		typeof(_reg) ____reg = (_reg);				\
		(typeof(____mask))((____reg & ____mask) >> __bf_shf(____mask)); \
	})
#endif

#ifndef FIELD_PREP
#define FIELD_PREP(_mask, _val)						\
	({								\
		typeof(_mask) ____mask = (_mask);			\
		((typeof(____mask))(_val) << __bf_shf(____mask)) & ____mask; \
	})
#endif

#ifndef FIELD_MAX
#define FIELD_MAX(_mask)						\
	({								\
		typeof(_mask) ____mask = (_mask);			\
		(typeof(____mask))((____mask) >> __bf_shf(____mask));	\
	})
#endif

#endif /* __LINUX_BITFIELD_COMPAT_H */
