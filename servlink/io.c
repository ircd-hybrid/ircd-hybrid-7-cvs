/************************************************************************
 *   IRC - Internet Relay Chat, servlink/io.c
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   $Id: io.c,v 1.8 2001/05/24 21:17:07 davidt Exp $
 */

#include "../include/setup.h"

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#ifdef HAVE_LIBCRYPTO
#include <openssl/evp.h>
#endif
#ifdef HAVE_LIBZ
#include <zlib.h>
#endif

#include "servlink.h"
#include "io.h"
#include "control.h"

static struct ctrl_command cmd = {0, 0, 0, 0, NULL};

#if defined( HAVE_LIBCRYPTO ) || defined( HAVE_LIBZ )
static unsigned char tmp_buf[BUFLEN];
#endif
#if defined( HAVE_LIBZ ) && defined( HAVE_LIBZ )
static unsigned char tmp2_buf[BUFLEN];
#endif

void send_data_blocking(int fd, unsigned char *data, int datalen)
{
  int ret;
  fd_set wfds;

  while (1)
  {
    ret = write(fd, data, datalen);

    if (ret == datalen)
      return;
    else if ( ret > 0)
    {
      data += ret;
      datalen -= ret;
    }

    ret = checkError(ret);

    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);

    /* block until we can write to the fd */
    /* check error exits on fatal error, else 1/0 for success/fail */
    while(!(ret = checkError(select(fd+1, NULL, &wfds, NULL, NULL))))
      ;
  }
}

/*
 * process_sendq:
 * 
 * used before CMD_INIT to pass contents of SendQ from ircd
 * to servlink.  This data must _not_ be encrypted/compressed.
 */
void process_sendq(unsigned char *data, unsigned int datalen)
{
  /* we can 'block' here, as we don't have to listen
   * to any other fds anyway
   */
  send_data_blocking(REMOTE_FD_W, data, datalen);
  LOG_IO(NOL, data, datalen);
}

/*
 * process_recvq:
 *
 * used before CMD_INIT to pass contents of RecvQ from ircd
 * to servlink.  This data must be decrypted/decopmressed before
 * sending back to the ircd.
 */
void process_recvq(unsigned char *data, unsigned int datalen)
{
  int ret;
  unsigned char *buf;
  unsigned int  blen;

  buf = data;
  blen = datalen;

  assert((datalen > 0) && (datalen <= READLEN));
#ifdef HAVE_LIBCRYPTO
  if (in_state.crypt)
  {
    assert(EVP_DecryptUpdate(&in_state.crypt_state.ctx,
                             tmp_buf, &blen,
                             data, datalen));
    assert(blen == datalen);
    buf = tmp_buf;
  }
#endif

#ifdef HAVE_LIBZ
  if (in_state.zip)
  {
    /* decompress data */
    in_state.zip_state.z_stream.next_in = buf;
    in_state.zip_state.z_stream.avail_in = blen;
    in_state.zip_state.z_stream.next_out = tmp2_buf;
    in_state.zip_state.z_stream.avail_out = BUFLEN;
    if ((ret = inflate(&in_state.zip_state.z_stream,
                       Z_NO_FLUSH)) != Z_OK)
      exit(ret);
    assert(in_state.zip_state.z_stream.avail_out);
    assert(in_state.zip_state.z_stream.avail_in == 0);
    blen = BUFLEN - in_state.zip_state.z_stream.avail_out;

    buf = tmp2_buf;

    /* did that generate any decompressed input? */
    if (!blen)
      return;
  }
#endif

  assert(blen);
  
  send_data_blocking(LOCAL_FD_W, buf, blen);
  LOG_IO(DOL, buf, blen);
}

/* read_ctrl
 *      called when a command is waiting on the control pipe
 */
void read_ctrl(void)
{
  int ret;
  unsigned char tmp;
 
  while (1) /* read as many commands as possible */
  {
    if (cmd.command == 0) /* we don't have a command yet */
    {
      cmd.gotdatalen = 0;
      cmd.readdata = 0;
      cmd.data = NULL;

      /* read the command */
      if (!(ret = checkError(read(CONTROL_FD_R, &cmd.command, 1))))
        return;

      LOG_IO(CIL,&cmd.command,1);
    }

    /* read datalen for commands including data */
    switch (cmd.command)
    {
#ifdef HAVE_LIBCRYPTO
      case CMD_SET_CRYPT_IN_CIPHER:
      case CMD_SET_CRYPT_IN_KEY:
      case CMD_SET_CRYPT_OUT_CIPHER:
      case CMD_SET_CRYPT_OUT_KEY:
#endif
#ifdef HAVE_LIBZ
      case CMD_SET_ZIP_OUT_LEVEL:
#endif
      case CMD_INJECT_RECVQ:
      case CMD_INJECT_SENDQ:
        if (cmd.gotdatalen == 0)
        {
          if (!(ret = checkError(read(CONTROL_FD_R, &tmp, 1))))
            return;
          LOG_IO(CIL, &tmp, 1);

          cmd.datalen = tmp << 8;
          cmd.gotdatalen = 1;
        }
        if (cmd.gotdatalen == 1)
        {
          if (!(ret = checkError(read(CONTROL_FD_R, &tmp, 1))))
            return;
          LOG_IO(CIL, &tmp, 1);
          
          cmd.datalen |= tmp;
          cmd.gotdatalen = 2;

          if (cmd.datalen > 0)
            cmd.data = calloc(cmd.datalen, 1);
        }
        break;
/*
      case CMD_END_ZIP_IN:
      case CMD_END_ZIP_OUT:
      case CMD_END_CRYPT_IN:
      case CMD_END_CRYPT_OUT:
 */
#ifdef HAVE_LIBCRYPTO
      case CMD_START_CRYPT_IN:
      case CMD_START_CRYPT_OUT:
#endif
#ifdef HAVE_LIBZ
      case CMD_START_ZIP_IN:
      case CMD_START_ZIP_OUT:
#endif
      case CMD_INIT:
        cmd.datalen = -1;
        break;
      default:
        /* invalid command */
        exit(1);
        break;
    }

    if (cmd.readdata < cmd.datalen) /* try to get any remaining data */
    {
      if (!(ret = checkError(read(CONTROL_FD_R,
                                  (cmd.data + cmd.readdata),
                                  cmd.datalen - cmd.readdata))))
        return;

      LOG_IO(CIL,(cmd.data+cmd.readdata),ret);

      cmd.readdata += ret;
      if (cmd.readdata < cmd.datalen)
        return;
    }

    /* we now have the command and any data */
    process_command(&cmd);

    if (cmd.datalen > 0)
      free(cmd.data);
    cmd.command = 0;
  }
}

void read_data(void)
{
  int ret;
  unsigned char *buf = out_state.buf;
  int  blen;
  
  if (out_state.len)
    exit(1);

#if defined(HAVE_LIBZ) || defined(HAVE_LIBCRYPTO)
  if (out_state.zip || out_state.crypt)
    buf = tmp_buf;
#endif
    
  while ((ret = checkError(read(LOCAL_FD_R, buf, READLEN))))
  {
    blen = ret;
    LOG_IO(DIL, buf, ret);
#ifdef HAVE_LIBZ
    if (out_state.zip)
    {
      out_state.zip_state.z_stream.next_in = buf;
      out_state.zip_state.z_stream.avail_in = ret;

      buf = out_state.buf;
#ifdef HAVE_LIB_CRYPTO
      if (out_state.crypt)
        buf = tmp2_buf;
#endif
      out_state.zip_state.z_stream.next_out = buf;
      out_state.zip_state.z_stream.avail_out = BUFLEN;
      assert(deflate(&out_state.zip_state.z_stream,
                     Z_PARTIAL_FLUSH) == Z_OK);
      assert(out_state.zip_state.z_stream.avail_out);
      assert(out_state.zip_state.z_stream.avail_in == 0);
      blen = BUFLEN - out_state.zip_state.z_stream.avail_out;
      assert(blen);
    }
#endif

#ifdef HAVE_LIBCRYPTO
    if (out_state.crypt)
    {
      /* encrypt data */
      ret = blen;
      assert( EVP_EncryptUpdate(&out_state.crypt_state.ctx,
                                out_state.buf, &blen,
                                buf, ret) );
      assert(blen == ret);
    }
#endif
    
    assert(blen);
    ret = checkError(write(REMOTE_FD_W, out_state.buf, blen));

    LOG_IO(NOL,out_state.buf,blen);
    if (ret < blen)
    {
      /* write incomplete, register write cb */
      fds[REMOTE_FD_W].write_cb = write_net;
      /*  deregister read_cb */
      fds[LOCAL_FD_R].read_cb = NULL;
      out_state.ofs = ret;
      out_state.len = blen - ret;
      return;
    }
  }
}

void write_net(void)
{
  int ret;

  if (!out_state.len)
    exit(1);

  if (!(ret = checkError(write(REMOTE_FD_W,
                               (out_state.buf + out_state.ofs),
                               out_state.len))))
    return; /* no data waiting */

  out_state.len -= ret;

  if (!out_state.len)
  {
    /* write completed, de-register write cb */
    fds[REMOTE_FD_W].write_cb = NULL;
    /* reregister read_cb */
    fds[LOCAL_FD_R].read_cb = read_data;
    out_state.ofs = 0;
  }
  else
    out_state.ofs += ret;
}

void read_net(void)
{
  int ret;
  int ret2;
  unsigned char *buf = in_state.buf;
  int  blen;

  if (in_state.len)
    exit(1);

#if defined(HAVE_LIBCRYPTO) || defined(HAVE_LIBZ)
  if (in_state.crypt || in_state.zip)
    buf = tmp_buf;
#endif

  while ((ret = checkError(read(REMOTE_FD_R, buf, READLEN))))
  {
    blen = ret;
    LOG_IO(NIL,buf,ret);
#ifdef HAVE_LIBCRYPTO
    if (in_state.crypt)
    {
      /* decrypt data */
      buf = in_state.buf;
#ifdef HAVE_LIBZ
      if (in_state.zip)
        buf = tmp2_buf;
#endif
      assert(EVP_DecryptUpdate(&in_state.crypt_state.ctx,
                               buf, &blen,
                               tmp_buf, ret));
      assert(blen == ret);
    }
#endif
    
#ifdef HAVE_LIBZ
    if (in_state.zip)
    {
      /* decompress data */
      in_state.zip_state.z_stream.next_in = buf;
      in_state.zip_state.z_stream.avail_in = ret;
      in_state.zip_state.z_stream.next_out = in_state.buf;
      in_state.zip_state.z_stream.avail_out = BUFLEN*2;
      if ((ret2 = inflate(&in_state.zip_state.z_stream,
                          Z_NO_FLUSH)) != Z_OK)
        exit(ret2);
      assert(in_state.zip_state.z_stream.avail_out);
      assert(in_state.zip_state.z_stream.avail_in == 0);
      blen = (BUFLEN*2) - in_state.zip_state.z_stream.avail_out;

      assert(blen >= 0);
      if (!blen)
        return; /* that didn't generate any decompressed input.. */
    }
#endif

    assert(blen);
    
    ret = checkError(write(LOCAL_FD_W, in_state.buf, blen));
    LOG_IO(DOL,in_state.buf,blen);

    if (ret < blen)
    {
      in_state.ofs = ret;
      in_state.len = blen - ret;
      /* write incomplete, register write cb */
      fds[LOCAL_FD_W].write_cb = write_data;
      /* deregister read_cb */
      fds[REMOTE_FD_R].read_cb = NULL;
      return;
    }
  }
}

void write_data(void)
{
  int ret;

  if (!in_state.len)
    exit(1);

  if (!(ret = checkError(write(LOCAL_FD_W,
                               (in_state.buf + in_state.ofs),
                               in_state.len))))
    return;

  in_state.len -= ret;

  if (!in_state.len)
  {
    /* write completed, de-register write cb */
    fds[LOCAL_FD_W].write_cb = NULL;
    /* reregister read_cb */
    fds[REMOTE_FD_R].read_cb = read_net;
    in_state.ofs = 0;
  }
  else
    in_state.ofs += ret;
}
