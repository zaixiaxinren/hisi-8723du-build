/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Backport of <linux/bitfield.h> for Linux 3.18
 *
 * The Realtek rtl8723du driver (v5.2.15.2) includes <linux/bitfield.h>
 * unconditionally and uses:
 *
 *     u8p_replace_bits()      (many places)
 *     u8_get_bits()
 *     le16p_replace_bits()
 *
 * Those helpers only appeared in Linux 4.16; <linux/bitfield.h> itself
 * only appeared in 4.12.  On 3.18 neither exists.
 *
 * This file is reached via  #include "linux/bitfield.h"  from the driver's
 * own include/ directory, so it does not depend on -I ordering.
 * The #ifndef guards make it a no-op on kernels that already provide the
 * real macros.
 */
#ifndef __LINUX_BITFIELD_COMPAT_H
#define __LINUX_BITFIELD_COMPAT_H

#include <linux/types.h>
#include <linux/byteorder/generic.h>

#ifndef __bf_shf
#define __bf_shf(x) (__builtin_ffsll(x) - 1)
#endif

#ifndef FIELD_PREP
#define FIELD_PREP(_mask, _val)						\
	({								\
		typeof(_mask) ____mask = (_mask);			\
		((typeof(____mask))(_val) << __bf_shf(____mask)) & ____mask; \
	})
#endif

#ifndef FIELD_GET
#define FIELD_GET(_mask, _reg)						\
	({								\
		typeof(_mask) ____mask = (_mask);			\
		typeof(_reg) ____reg = (_reg);				\
		(typeof(____mask))((____reg & ____mask) >> __bf_shf(____mask)); \
	})
#endif

#ifndef FIELD_MAX
#define FIELD_MAX(_mask)						\
	({								\
		typeof(_mask) ____mask = (_mask);			\
		(typeof(____mask))((____mask) >> __bf_shf(____mask));	\
	})
#endif

/* 8 bit */
#ifndef u8_get_bits
#define u8_get_bits(v, mask)		FIELD_GET(mask, v)
#endif
#ifndef u8_encode_bits
#define u8_encode_bits(v, mask)		FIELD_PREP(mask, v)
#endif
#ifndef u8p_replace_bits
#define u8p_replace_bits(p, v, mask)					\
	(*(p) = (*(p) & ~(mask)) | FIELD_PREP(mask, v))
#endif

/* little endian 16 bit */
#ifndef le16_get_bits
#define le16_get_bits(v, mask)		FIELD_GET(mask, le16_to_cpu(v))
#endif
#ifndef le16_encode_bits
#define le16_encode_bits(v, mask)	cpu_to_le16(FIELD_PREP(mask, v))
#endif
#ifndef le16p_replace_bits
#define le16p_replace_bits(p, v, mask)					\
	(*(p) = cpu_to_le16((le16_to_cpu(*(p)) & ~(mask)) | FIELD_PREP(mask, v)))
#endif

/* little endian 32 bit */
#ifndef le32_get_bits
#define le32_get_bits(v, mask)		FIELD_GET(mask, le32_to_cpu(v))
#endif
#ifndef le32_encode_bits
#define le32_encode_bits(v, mask)	cpu_to_le32(FIELD_PREP(mask, v))
#endif
#ifndef le32p_replace_bits
#define le32p_replace_bits(p, v, mask)					\
	(*(p) = cpu_to_le32((le32_to_cpu(*(p)) & ~(mask)) | FIELD_PREP(mask, v)))
#endif

#endif /* __LINUX_BITFIELD_COMPAT_H */
