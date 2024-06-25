/*
 * Copyright (c) 2023-2024 Ian Marco Moffett and the Osmora Team.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Hyra nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "sio.h"

static struct sio_bip pipeline;

static int
sio_txn_log(struct sio_bip *bip, struct sio_txn *txn)
{
    size_t len = SIO_TXN_READLEN(bip, txn);
    char *buf = txn->buf;

    for (size_t i = txn->pos; i < len; ++i) {
        printf("value: %d (index=%d)\n", buf[i], i);
    }

    return 0;
}

int
main(void)
{
    struct sio_txn *txn, *txn1;
    char buf[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    char buf1[8] = {109, 110, 111, 112, 113, 114, 115, 116};

    SIO_BIP_INIT(&pipeline, SIO_THRESHOLD, sio_txn_log);
    txn = sio_txn_alloc(buf, 0, sizeof(buf));
    txn1 = sio_txn_alloc(buf1, 0, sizeof(buf1));

    SIO_BIP_ENQ(&pipeline, txn);
    SIO_BIP_ENQ(&pipeline, txn1);
    sio_bip_process(&pipeline);

    free(txn);
    free(txn1);
    return 0;
}
