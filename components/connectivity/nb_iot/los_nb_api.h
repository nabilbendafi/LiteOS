/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2021. All rights reserved.
 * Description: NB API HeadFile
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

/**@defgroup nbiot
 * @ingroup nbiot
 */

#ifndef _NB_IOT_H
#define _NB_IOT_H
#include "at_main.h"

typedef struct sec_param {
char *psk;
char *pskId;
uint8_t setPskFlag;
} SecureParam;

/*
Func Name: los_nb_init

@par Description
    This API is used to init nb module and connect to cloud.
@param[in]  host  cloud ip
@param[in]  port  cloud port
@param[in]  psk   if not null,the security param
@par Return value
*  0:on success
*  negative value: on failure
*/
int los_nb_init(const int8_t *host, const int8_t *port, SecureParam *psk);
/*
Func Name: los_nb_report

@par Description
    This API is used for nb module to report data to cloud.
@param[in] buf point to data to be reported
@param[in] buflen data length
@par Return value
*  0:on success
*  negative value: on failure
*/
int los_nb_report(const char *buf, int buflen);
/*
Func Name: los_nb_notify

@par Description
    This API is used to regist callback when receive the cmd from cloud.
@param[in] featurestr feature string that in cmd
@param[in] cmdlen length of feature string
@param[in] callback callback of device
@par Return value
*  0:on success
*  negative value: on failure
*/

int los_nb_notify(char *featureStr, int cmdLen, OobCallback callback, OobCmdMatch cmdMatch);
/*
Func Name: los_nb_deinit

@par Description
    This API is used to deinit the nb module.
@param[in] NULL
@par Return value
*  0:on success
*  negative value: on failure
*/

int los_nb_deinit(void);

#endif /* _NB_IOT_H */
