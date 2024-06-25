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

#ifndef _SIO_H_
#define _SIO_H_

#include <sys/queue.h>
#include <stddef.h>

#define SIO_THRESHOLD 4     /* Default */
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

struct sio_txn;
struct sio_bip;
typedef int(*sio_op_t)(struct sio_bip *, struct sio_txn *);

/*
 * System I/O transaction
 *
 * This structure describes a system I/O transaction
 * and contains common fields that are used
 * with system I/O.
 */
struct sio_txn {
    void *buf;              /* Source/dest buffer */
    off_t offset;           /* Transfer offset */
    size_t len;             /* Length in bytes */
    size_t rescount;        /* Residual byte count */
    size_t pos;             /* Position in buffer */
    TAILQ_ENTRY(sio_txn) link;
};

/*
 * System I/O pipeline
 */
struct sio_bip {
    size_t txn_cnt;
    size_t threshold;
    sio_op_t io_op;
    TAILQ_HEAD(, sio_txn) q;
};

/*
 * Get the count of bytes to process in this
 * BIP stage.
 */
#define SIO_TXN_READLEN(BIPP, TXNP) \
    MIN(txn->pos + (BIPP)->threshold, txn->len);

/*
 * Initialize a BIP
 */
#define SIO_BIP_INIT(BIPP, THRESHOLD, IOOP)     \
    (BIPP)->txn_cnt = 0;                        \
    (BIPP)->threshold = THRESHOLD;              \
    (BIPP)->io_op = IOOP;                       \
    TAILQ_INIT(&(BIPP)->q);

/*
 * Enqueue an SIO transaction into a BIP
 */
#define SIO_BIP_ENQ(BIPP, TXNP)                 \
    TAILQ_INSERT_HEAD(&(BIPP)->q, TXNP, link);

struct sio_txn *sio_txn_alloc(void *buf, off_t off, size_t len);
int sio_bip_process(struct sio_bip *bip);

#endif  /* _SIO_H_ */
