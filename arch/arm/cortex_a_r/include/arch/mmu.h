/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: ARMv7 Mmu HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

/**
 * @defgroup mmu
 * @ingroup kernel
 */

#ifndef _ARCH_MMU_H
#define _ARCH_MMU_H

#include "los_typedef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define UNCACHEABLE    0
#define CACHEABLE      1
#define UNBUFFERABLE   0
#define BUFFERABLE     1
#define EXECUTABLE     0
#define NON_EXECUTABLE 1
#define ACCESS_RW      3U /* ap = 0 ap1 = 0b11 */
#define ACCESS_RO      7U /* ap = 1 ap1 = 0b11 */
#define ACCESS_NA      0U /* ap = 0 ap1 = 0 */
#define D_MANAGER      0
#define D_CLIENT       1
#define D_NA           2

#define DOMAIN0        0
#define DOMAIN1        1
#define DOMAIN2        2

#define MMU_AP_STATE(flag)      (((flag) & 0x1U) ? ACCESS_RW : ACCESS_RO)
#define MMU_CACHE_STATE(flag)   (((flag) >> 1) & 0x1U)
#define MMU_BUFFER_STATE(flag)  (((flag) >> 2) & 0x1U)
#define MMU_EXECUTE_STATE(flag) (((flag) >> 3) & 0x1U)
#define MMU_GET_AREA(flag)      ((flag) & (0x1U << 4))
#define MMU_DESC_LEN 4
#define MMU_DESC_OFFSET(vBase)  ((vBase) << 2)

/**
 * @ingroup mmu
 * The access permission mode is read-only.
 */
#define ACCESS_PERM_RO_RO 0

/**
 * @ingroup mmu
 * The access permission mode is read and write.
 */
#define ACCESS_PERM_RW_RW (1U << 0)

/**
 * @ingroup mmu
 * The cache enabled.
 */
#define CACHE_ENABLE (1U << 1)

/**
 * @ingroup mmu
 * The cache disabled.
 */
#define CACHE_DISABLE 0

/**
 * @ingroup mmu
 * The buffer enabled.
 */
#define BUFFER_ENABLE (1U << 2)

/**
 * @ingroup mmu
 * The buffer disabled.
 */
#define BUFFER_DISABLE 0

/**
 * @ingroup mmu
 * Set it non-executable.
 */
#define EXEC_DISABLE (1U << 3)

/**
 * @ingroup mmu
 * Set it executable.
 */
#define EXEC_ENABLE 0

/**
 * @ingroup mmu
 * The first section(1M/item).
 */
#define FIRST_SECTION (1U << 4)

/**
 * @ingroup mmu
 * The second page(4K/item).
 */
#define SECOND_PAGE 0

#ifdef LOSCFG_KERNEL_SMP
#define MMU_SHAREABLE 1
#else
#define MMU_SHAREABLE 0
#endif

#define MMU_1K  0x400U
#define MMU_4K  0x1000U
#define MMU_16K 0x4000U
#define MMU_64K 0x10000U
#define MMU_1M  0x100000U
#define MMU_4G  0x100000000ULL

#define SHIFT_1K  10
#define SHIFT_4K  12
#define SHIFT_16K 14
#define SHIFT_64K 16
#define SHIFT_1M  20
#define SHIFT_2M  21
#define SHIFT_1G  30

/**
 * @ingroup mmu
 * mmu second page information structure.
 *
 */
typedef struct {
    UINT32 page_addr;   /* The second page start addr */
    UINT32 page_length; /* The second page length */

    /*
     * The second page page table storage addr,
     * diff second page table can't be coincided
     */
    UINT32 page_descriptor_addr;

    /*
     * The second page type, it can be set
     * small page ID(4K)   : MMU_SECOND_LEVEL_SMALL_PAGE_TABLE_ID
     * or big page ID(64K) : MMU_SECOND_LEVEL_BIG_PAGE_TABLE_ID
     */
    UINT32 page_type;
} SENCOND_PAGE;

/**
 * @ingroup mmu
 * mmu param setting information structure.
 *
 */
typedef struct {
    UINT32 startAddr; /* Starting address of a section. */
    UINT32 endAddr;   /* Ending address of a section. */

    /*
     * Mode set.
     * bit0: ACCESS_PERM_RW_RW/ACCESS_PERM_RO_RO(1/0)
     * bit1: CACHE_ENABLE/CACHE_DISABLE(1/0)
     * bit2: BUFFER_ENABLE/BUFFER_DISABLE(1/0)
     * bit3: EXEC_DISENABLE/EXEC_ENABLE(1/0)
     * bit4: FIRST_SECTION/SECOND_PAGE(1/0)
     * bit5~7: ignore
     */
    UINT32 uwFlag;

    /*
     * the goal object of second page,
     * if uwFlag is FIRST_SECTION, stPage will be ignored, and you can set this member as NULL
     */
    SENCOND_PAGE *stPage;
} MMU_PARAM;

/* ARM Domain Access Control Bit Masks */
#define ACCESS_TYPE_NO_ACCESS(domainNum) (0x0U << ((domainNum) << 1))
#define ACCESS_TYPE_CLIENT(domainNum)    (0x1U << ((domainNum) << 1))
#define ACCESS_TYPE_MANAGER(domainNum)   (0x3U << ((domainNum) << 1))

#define MMU_FIRST_LEVEL_FAULT_ID      0x0
#define MMU_FIRST_LEVEL_PAGE_TABLE_ID 0x1
#define MMU_FIRST_LEVEL_SECTION_ID    0x2
#define MMU_FIRST_LEVEL_RESERVED_ID   0x3
/**
 * @ingroup mmu
 * The second page type select 64K
 */
#define MMU_SECOND_LEVEL_BIG_PAGE_TABLE_ID 0x1U

/**
 * @ingroup mmu
 * The second page type select 4K
 */
#define MMU_SECOND_LEVEL_SMALL_PAGE_TABLE_ID 0x2U

struct MMUFirstLevelFault {
    UINT32 id : 2;            /* [1 : 0] */
    UINT32 sbz : 30;          /* [31 : 2] */
};

struct MMUFirstLevelPageTable {
    UINT32 id : 2;            /* [1 : 0] */
    UINT32 pxn : 1;           /* [2] */
    UINT32 ns : 1;            /* [3] */
    UINT32 sbz : 1;           /* [4] */
    UINT32 domain : 4;        /* [8 : 5] */
    UINT32 imp : 1;           /* [9] */
    UINT32 baseAddress : 22;  /* [31 : 10] */
};

struct MMUSecondLevelBigPageTable {
    UINT32 id : 2;            /* [1 : 0] */
    UINT32 b : 1;             /* [2] */
    UINT32 c : 1;             /* [3] */
    UINT32 ap1 : 2;           /* [5 : 4] */
    UINT32 sbz : 3;           /* [8 : 6] */
    UINT32 ap : 1;            /* [9] */
    UINT32 s : 1;             /* [10] */
    UINT32 ng : 1;            /* [11] */
    UINT32 tex : 3;           /* [14 : 12] */
    UINT32 xn : 1;            /* [15] */
    UINT32 baseAddress : 16;  /* [31 : 16] */
};

struct MMUSecondLevelSmallPageTable {
    UINT32 xn : 1;            /* [0] */
    UINT32 id : 1;            /* [1] */
    UINT32 b : 1;             /* [2] */
    UINT32 c : 1;             /* [3] */
    UINT32 ap1 : 2;           /* [5 : 4] */
    UINT32 tex : 3;           /* [8 : 6] */
    UINT32 ap : 1;            /* [9] */
    UINT32 s : 1;             /* [10] */
    UINT32 ng : 1;            /* [11] */
    UINT32 baseAddress : 20;  /* [31 : 12] */
};

struct MMUFirstLevelSection {
    UINT32 id : 2;            /* [1 : 0] */
    UINT32 b : 1;             /* [2] */
    UINT32 c : 1;             /* [3] */
    UINT32 xn : 1;            /* [4] */
    UINT32 domain : 4;        /* [8 : 5] */
    UINT32 imp : 1;           /* [9] */
    UINT32 ap1 : 2;           /* [11 : 10] */
    UINT32 tex : 3;           /* [14 : 12] */
    UINT32 ap : 1;            /* [15] */
    UINT32 s : 1;             /* [16] */
    UINT32 ng : 1;            /* [17] */
    UINT32 revs : 1;          /* [18] */
    UINT32 ns : 1;            /* [19] */
    UINT32 baseAddress : 12;  /* [31 : 20] */
};

struct MMUFirstLevelReserved {
    UINT32 id : 2;            /* [1 : 0] */
    UINT32 sbz : 30;          /* [31 : 2] */
};

#define X_MMU_SET_AP_ALL(item, access) do { \
    (item).ap1 = (access) & 0x3U;           \
    (item).ap  = (access) >> 2;             \
} while (0)

#define X_MMU_SET_BCX(item, buff, cache, exeNever) do { \
    (item).b = (buff);                                  \
    (item).c = (cache);                                 \
    (item).xn = (exeNever);                             \
} while (0)

#define X_MMU_CHG_DESC(aBase, vBase, size, baseAddress, tableBase) do { \
    UINT32 i, j, k;                                                     \
    k = (tableBase) + MMU_DESC_OFFSET(vBase);                           \
    for (j = (aBase), i = 0; i < (size); ++i, ++j, k += MMU_DESC_LEN) { \
        (baseAddress) = j;                                              \
        *(UINTPTR *)(UINTPTR)k = desc.word;                             \
    }                                                                   \
} while (0)

#define X_MMU_CHG_DESC_64K(aBase, vBase, size, baseAddress, tableBase) do { \
    UINT32 i, j, k, n;                                                      \
    k = (tableBase) + MMU_DESC_OFFSET(vBase);                               \
    for (j = (aBase), i = 0; i < (size); ++i, ++j) {                        \
        (baseAddress) = j;                                                  \
        for (n = 0; n < (MMU_64K / MMU_4K); ++n, k += MMU_DESC_LEN) {       \
            *(UINTPTR *)(UINTPTR)k = desc.word;                             \
        }                                                                   \
    }                                                                       \
} while (0)

#define SECTION_CHANGE(item, cache, buff, access, exeNever) do { \
    union MMUFirstLevelDescriptor desc;                          \
    desc.word = (*(UINTPTR *)(item));                            \
    desc.section.s = (MMU_SHAREABLE);                            \
    X_MMU_SET_BCX(desc.section, (buff), (cache), (exeNever));    \
    X_MMU_SET_AP_ALL(desc.section, (access));                    \
    (*(UINTPTR *)(UINTPTR)(item)) = desc.word;                   \
} while (0)

#define X_MMU_SECTION(aBase, vBase, size, cache, buff, access, exeNever, sdomain) do { \
    union MMUFirstLevelDescriptor desc = { .word = 0 };                                \
    desc.section.id = MMU_FIRST_LEVEL_SECTION_ID;                                      \
    desc.section.s = (MMU_SHAREABLE);                                                  \
    desc.section.domain = (sdomain);                                                   \
    X_MMU_SET_BCX(desc.section, (buff), (cache), (exeNever));                          \
    X_MMU_SET_AP_ALL(desc.section, (access));                                          \
    X_MMU_CHG_DESC(aBase, vBase, size, desc.section.baseAddress, ttbBase);             \
} while (0)

#define X_MMU_ONE_LEVEL_PAGE(aBase, vBase, size, sdomain) do {               \
    union MMUFirstLevelDescriptor desc = { .word = 0 };                      \
    desc.pageTable.id = MMU_FIRST_LEVEL_PAGE_TABLE_ID;                       \
    desc.pageTable.domain = (sdomain);                                       \
    X_MMU_CHG_DESC(aBase, vBase, size, desc.pageTable.baseAddress, ttbBase); \
} while (0)

#define X_MMU_TWO_LEVEL_PAGE(aBase, vBase, size, cache, buff, access, exeNever) do { \
    union MMUFirstLevelDescriptor desc = { .word = 0 };                              \
    desc.smallPageTable.id = MMU_SECOND_LEVEL_SMALL_PAGE_TABLE_ID >> 1;              \
    desc.smallPageTable.s = (MMU_SHAREABLE);                                         \
    X_MMU_SET_BCX(desc.smallPageTable, (buff), (cache), (exeNever));                 \
    X_MMU_SET_AP_ALL(desc.smallPageTable, (access));                                 \
    X_MMU_CHG_DESC(aBase, vBase, size, desc.smallPageTable.baseAddress, sttBase);    \
} while (0)

#define X_MMU_TWO_LEVEL_PAGE64K(aBase, vBase, size, cache, buff, access, exeNever) do { \
    union MMUFirstLevelDescriptor desc = { .word = 0 };                                 \
    desc.bigPageTable.id = MMU_SECOND_LEVEL_BIG_PAGE_TABLE_ID;                          \
    desc.bigPageTable.s = (MMU_SHAREABLE);                                              \
    X_MMU_SET_BCX(desc.bigPageTable, (buff), (cache), (exeNever));                      \
    X_MMU_SET_AP_ALL(desc.bigPageTable, (access));                                      \
    X_MMU_CHG_DESC_64K(aBase, vBase, size, desc.bigPageTable.baseAddress, sttBase);     \
} while (0)

union MMUFirstLevelDescriptor {
    UINTPTR word;
    struct MMUFirstLevelFault fault;
    struct MMUFirstLevelPageTable pageTable;
    struct MMUFirstLevelSection section;
    struct MMUFirstLevelReserved reserved;
    struct MMUSecondLevelSmallPageTable smallPageTable;
    struct MMUSecondLevelBigPageTable bigPageTable;
};

STATIC INLINE VOID EnableAPCheck(VOID)
{
    UINT32 regDACR = ACCESS_TYPE_MANAGER(0) |
                     ACCESS_TYPE_CLIENT(1);
    __asm volatile("mcr    p15, 0, %0, c3, c0, 0"
                   :
                   : "r"(regDACR));
}

STATIC INLINE VOID DisableAPCheck(VOID)
{
    UINT32 regDACR = ACCESS_TYPE_MANAGER(0) |
                     ACCESS_TYPE_MANAGER(1);
    __asm volatile("mcr    p15, 0, %0, c3, c0, 0"
                   :
                   : "r"(regDACR));
}

STATIC INLINE VOID CleanTLB(VOID)
{
    /* replace TLBIALL with TLBIALLIS for multi-core sync */
    __asm volatile("mov    %0, #0\n"
                   "mcr    p15, 0, %0, c8, c3, 0\n"
                   :
                   : "r"(0));
}

/* Second page struct for OS and App */
extern SENCOND_PAGE g_mmuOsPage;
extern SENCOND_PAGE g_mmuAppPage;

#ifdef LOSCFG_KERNEL_DYNLOAD
extern SENCOND_PAGE g_mmuDlPage;
#endif

/**
 * @ingroup mmu
 * @brief Memory Management Unit Second page memory map.
 *
 * @par Description:
 * This API is used to set the second page memory map of a section that is specified by
 * a starting address and ending address.
 * @attention
 * <ul>
 * <li>The passed-in starting address and ending address must be aligned on a boundary of 1M.
 * The access permission mode can be only set to ACCESS_PERM_RO_RO and ACCESS_PERM_RW_RW.
 * </li>
 * </ul>
 *
 * @param SENCOND_PAGE  [IN] param for second page enable, the struct contains below members:
 * page_addr:               Starting address of the section.
 * page_length:             Length of the section.
 * page_descriptor_addr:    second page descriptor address for the section.
 * page_type:               The second page type, MMU_SECOND_LEVEL_SMALL_PAGE_TABLE_ID
 *                              or MMU_SECOND_LEVEL_BIG_PAGE_TABLE_ID.
 * @param flag          [IN] param for second page enable, the mode of the section.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>mmu.h: the header file that contains the API declaration.</li></ul>
 * @see
 * @since Huawei LiteOS V100R001C00
 */
VOID ArchSecPageEnable(SENCOND_PAGE *page, UINT32 flag);

/**
 * @ingroup mmu
 * @brief Memory Management Unit Cache/Buffer/Access Permission Setting.
 *
 * @par Description:
 * This API is used to set the Cache/Buffer/access permission mode of a section that is specified by
 * a starting address and ending address.
 * @attention
 * <ul>
 * <li>The passed-in starting address and ending address must be aligned on a boundary of 4K or 1M.
 * The access permission mode can be only set to ACCESS_PERM_RO_RO and ACCESS_PERM_RW_RW.
 * </li>
 * </ul>
 *
 * @param MMU_PARAM  [IN] param for mmu setting, the struct contains below members.
 * startAddr:       Starting address of a section.
 * endAddr:         Ending address of a section.
 * uwFlag:          Mode set. There are three func could be controlled with three bit.
 *                      bit0: ACCESS_PERM_RW_RW/ACCESS_PERM_RO_RO(1/0).
 *                      bit1: CACHE_ENABLE/CACHE_DISABLE(1/0).
 *                      bit2: BUFFER_ENABLE/BUFFER_DISABLE(1/0).
 *                      bit3: FIRST_SECTION/SECOND_PAGE(1/0) it need comfire your memory type, be descripted.
 *                      bit4~7: ignore.
 * stPage:          The goal object of second page, if uwFlag bit3 is FIRST_SECTION,
 *                  stPage will be ignored, and you can set this member as NULL.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>mmu.h: the header file that contains the API declaration.</li></ul>
 * @see
 * @since Huawei LiteOS V100R001C00
 */
VOID ArchMMUParamSet(MMU_PARAM *mPara);
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _ARCH_MMU_H */
