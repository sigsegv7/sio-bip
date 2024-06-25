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

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sio.h"

struct sio_txn *
sio_txn_alloc(void *buf, off_t off, size_t len)
{
    struct sio_txn *txn;

    txn = calloc(1, sizeof(*txn));
    if (txn == NULL)
        return NULL;

    txn->buf = buf;
    txn->offset = off;
    txn->len = len;
    txn->rescount = len;
    txn->pos = 0;
    return txn;
}

int
sio_bip_process(struct sio_bip *bip)
{
    struct sio_txn *txn;
    int status;

    for (;;) {
        if (TAILQ_EMPTY(&bip->q))
            break;

        TAILQ_FOREACH(txn, &bip->q, link) {
            if (txn->rescount == 0)
                TAILQ_REMOVE(&bip->q, txn, link);
            if ((status = bip->io_op(bip, txn)) != 0)
                return status;

            /*
             * If the residual byte count is less than the
             * threshold then it is safe to say all bytes in
             * this entire transaction have been processed.
             */
            if (txn->rescount < bip->threshold) {
                txn->rescount = 0;
                txn->pos = txn->len;
            } else {
                txn->rescount -= bip->threshold;
                txn->pos += bip->threshold;
            }
        }
    }
    return 0;
}
