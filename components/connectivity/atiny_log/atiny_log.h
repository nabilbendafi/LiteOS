/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2021. All rights reserved.
 * Description: Atiny Log HeadFile
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

#ifndef _ATINY_LOG_H
#define _ATINY_LOG_H

#include "osdepends/atiny_osdep.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERR,
    LOG_FATAL,
    LOG_MAX
} atiny_log_e;

/**
 *@ingroup agenttiny
 *@brief set log level.
 *
 *@par Description:
 *This API is used to set log level. The log informations whose level is not less than
  the level set up in this interface are displayed.
 *@attention none.
 *
 *@param level          [IN] Log level to be set up.
 *
 *@retval none.
 *@par Dependency: none.
 *@see atiny_get_log_level.
 */
void atiny_set_log_level(atiny_log_e level);

/**
 * @ingroup agenttiny
 * @brief get log level.
 *
 * @par Description:
 * This API is used to get log level set by atiny_set_log_level.
 * @attention none.
 *
 * @param none.
 *
 * @retval #atiny_log_e  Log level.
 * @par Dependency: none.
 * @see atiny_set_log_level.
 */
atiny_log_e atiny_get_log_level(void);

#ifdef LOSCFG_COMPONENTS_ATINY_LOG
const char *atiny_get_log_level_name(atiny_log_e log_level);

#define ATINY_LOG(level, fmt, ...)                                                               \
    do {                                                                                         \
        if ((level) >= atiny_get_log_level()) {                                                  \
            (void)atiny_printf("[%s][%u][%s:%d] " fmt "\r\n", atiny_get_log_level_name((level)), \
                (uint32_t)atiny_gettime_ms(), __FUNCTION__, __LINE__, ##__VA_ARGS__);            \
        }                                                                                        \
    } while (0)
#else
#define ATINY_LOG(level, fmt, ...)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _ATINY_LOG_H */
