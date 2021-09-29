/*----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: NB IOT API
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

#include "los_nb_api.h"
#include "at_api.h"
#ifdef LOSCFG_COMPONENTS_NET_AT_BC95
#include "bc95.h"
#endif

int los_nb_init(const int8_t *host, const int8_t *port, sec_param_s *psk)
{
    int ret;
    int timecnt = 0;

    /* when used nb with agenttiny */
    /* the following para is replaced by call nb_int() */
    at_config at_user_conf = {
        .name = AT_MODU_NAME,
        .usart_port = AT_USART_PORT,
        .buardrate = AT_BUARDRATE,
        .linkid_num = AT_MAX_LINK_NUM,
        .user_buf_len = MAX_AT_USERDATA_LEN,
        .cmd_begin = AT_CMD_BEGIN,
        .line_end = AT_LINE_END,
        .mux_mode = 1, // support multi connection mode
        .timeout = AT_CMD_TIMEOUT, // ms
    };

    at.init(&at_user_conf);

    nb_reboot();
    if (psk != NULL) {
        if (psk->setpsk) {
            nb_send_psk(psk->pskid, psk->psk);
        } else {
            nb_set_no_encrypt();
        }
    }

    while (1) {
        ret = nb_hw_detect();
        printf("call nb_hw_detect,ret is %d\n", ret);
        if (ret == AT_OK) {
            break;
        }
    }
    while (timecnt < 120) {
        ret = nb_get_netstat();
        nb_check_csq();
        if (ret != AT_FAILED) {
            ret = nb_query_ip();
            break;
        }
        timecnt++;
    }
    if (ret != AT_FAILED) {
        nb_query_ip();
    }
    ret = nb_set_cdpserver((char *)host, (char *)port);
    return ret;
}

int los_nb_report(const char *buf, int len)
{
    if ((buf == NULL) || (len <= 0)) {
        return -1;
    }
    return nb_send_payload(buf, len);
}

int los_nb_notify(char *featurestr, int cmdlen, oob_callback callback, oob_cmd_match cmd_match)
{
    if ((featurestr == NULL) || (cmdlen <= 0) || (cmdlen >= OOB_CMD_LEN - 1)) {
        return -1;
    }
    return at.oob_register(featurestr, cmdlen, callback, cmd_match);
}

int los_nb_deinit(void)
{
    nb_reboot();
    at.deinit();
    return 0;
}
