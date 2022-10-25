#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <ctype.h>

#include "co_cdh.h"
#include "co_syi.h"
#include "rt_errh.h"
#include "rt_net.h"
#include "co_time.h"
#include "rt_stssrv_msg.h"
#include "statussrv_net.h"
#include "statussrv_client.h"

#if 0
int main()
{
  statussrv__client *cli = new statussrv_client();

  cli->server_connect();
  for (int i = 0; i < 20; i++) {
    cli->send_request(1);
    cli->receive();
    cli->send_request(2);
    cli->receive();

    sleep(1);
  }
  delete cli;
}
#endif

int statussrv_client::server_connect()
{
  pwr_tStatus sts;
  int buffsize = 10000;
  int flags;
  fd_set fdr; /* For select call */
  fd_set fdw; /* For select call */
  struct timeval tv;
  struct sockaddr_in addr;

  if (m_busid == 0) {
    m_busid = syi_Busid(&sts);
    if (EVEN(sts)) {
      //errh_Error("Connect error %m", sts);
      return -1;
    }
  }
  m_port = 56000 + m_busid;

  time_GetTimeMonotonic(&m_last_try_connect_time);

  /* Create socket */
  m_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (m_sock < 0) {
    //errh_Error("Error creating socket, %d", m_sock);
    return m_sock;
  }

  sts = setsockopt(
      m_sock, SOL_SOCKET, SO_RCVBUF, &buffsize, sizeof(buffsize));

  if (sts < 0) {
    printf("Error setting receive buffer size \n");
  }

  /* Initialize remote address structure */

  addr.sin_family = AF_INET;
  addr.sin_port = htons(m_port);

  if (isascii(m_address[0])) {
    sts = net_StringToAddr(m_address, &addr.sin_addr);
    if (EVEN(sts)) {
      //errh_Error("Address error %s", m_address);
      return sts;
    }
    addr.sin_addr.s_addr = ntohl(addr.sin_addr.s_addr);
  }
  else 
    addr.sin_addr.s_addr = inet_addr(m_address);

  /* Connect to remote address */
  fcntl(m_sock, F_SETFL, (flags = fcntl(m_sock, F_GETFL)) | O_NONBLOCK);

  sts = connect(m_sock, (struct sockaddr*)&addr, sizeof(addr));

  if (sts < 0) {

    FD_ZERO(&fdr);
    FD_ZERO(&fdw);
    FD_SET(m_sock, &fdr);
    FD_SET(m_sock, &fdw);

    if (m_reconnect_limit > 200) {
      tv.tv_sec = 0;
      tv.tv_usec = 1000 * m_reconnect_limit;
    } else {
      tv.tv_sec = 0;
      tv.tv_usec = 200000;
    }

    sts = select((int)m_sock + 1, &fdr, &fdw, NULL, &tv);

    if (sts <= 0) {
      m_sts = STSSRV__CONNDOWN;
      close(m_sock);
      //errh_Error("Error connecting remote socket, %d", sts);
      return -1;
    }
    if (sts == 1) // TODO
      m_sts = STSSRV__SUCCESS;
    else
      m_sts = STSSRV__CONNDOWN;
  }
  else
    m_sts = STSSRV__SUCCESS;

  fcntl(m_sock, F_SETFL, (flags = fcntl(m_sock, F_GETFL)) ^ O_NONBLOCK);

  return sts;
}


pwr_tStatus statussrv_client::send_request(stssrv_eMsgType type)
{
  pwr_tStatus sts = 0;

  /* Send request */
  stssrv_sRequest rr;

  m_trans_id++;

  rr.head.id = m_trans_id;
  rr.head.length = sizeof(rr);
  rr.head.type = (int)type;

  sts = send(m_sock, &rr, sizeof(rr), MSG_DONTWAIT);
  if (sts < 0) {
    m_sts = STSSRV__CONNDOWN;
    close(m_sock);
    //errh_Error("Connection down to status server");
    return STSSRV__SUCCESS;
  }
  m_expected_msgs++;

  return STSSRV__SUCCESS;
}

pwr_tStatus statussrv_client::receive(stssrv_eMsgType *type, char **msg)
{
  pwr_tStatus sts;
  fd_set fdr;
  struct timeval tv;
  int data_size;
  stssrv_sMsgHead* rb;
  static char rcv_buffer[12800];
  short int trans_id = 0;
  short int size_of_msg;

  short int remaining_data; // Data we have to get from the socket.
  short int received_data; // Data that has been received.

  sts = 1;
  rb = (stssrv_sMsgHead*)rcv_buffer;
  *msg = (char *)rb;

  while (sts > 0) { /* Receive answer */

    size_of_msg = 0;
    remaining_data = sizeof(stssrv_sMsgHead);
    received_data = 0;

    /*
      First read at least the MBAP header, and no more.
      Then, read the remaining bytes indicaterd in the header, but no more.
      We control the amount of data because could be more than one message in
      the socket buffer or
      only the first bytes of a packet.
    */

    while ((remaining_data > 0) && (sts > 0)) { // if there is data to read and
                                                // everything is ok, receive.

      FD_ZERO(&fdr);
      FD_SET(m_sock, &fdr);

      if (m_expected_msgs > 0) {
        tv.tv_sec = 0;
        tv.tv_usec = m_response_time * 1000;
      } else {
        tv.tv_sec = 0;
        tv.tv_usec = 0;
      }

      sts = select((int)m_sock + 1, &fdr, NULL, NULL, &tv);

      if (sts <= 0) { // Timeout or error.
        if (sts == 0 && m_expected_msgs > 0) { // Timeout but there are messages pending
          m_msgs_lost++;

          if (!m_timeout) {
            time_GetTimeMonotonic(&m_timeout_time);
            m_timeout = 1;
          } else {
            pwr_tTime now;
            pwr_tDeltaTime dt;
            pwr_tDeltaTime max_dt;
            float max_timeout = m_max_timeout;

            if (feqf(max_timeout, 0.0f))
              max_timeout = 2;
            time_FloatToD(&max_dt, max_timeout);
            time_GetTimeMonotonic(&now);
            time_Adiff(&dt, &now, &m_timeout_time);
            if (time_Dcomp(&dt, &max_dt) > 0) {
              m_sts = STSSRV__CONNDOWN;
              close(m_sock);
              //errh_Error("Data expected but timeout. Connection down to status server");
              m_timeout = 0;
            }
          }
          return STSSRV__SUCCESS;
        }

        if (sts < 0) { // Error in the socket
          m_sts = STSSRV__CONNLOST;
          close(m_sock);
          //errh_Error("Socket Error. Connection lost to status server");
          m_timeout = 0;
          return STSSRV__SUCCESS;
        }

      } else { // There are something to read (no timeout and no error). Could
               // be a closed socket too, so we have to check later anyway.
        data_size
            = recv(m_sock, &rcv_buffer[received_data], remaining_data, 0);

        if (data_size < 0) {
          m_sts = STSSRV__CONNLOST;
          close(m_sock);
          //errh_Error("Error reading data. Connection lost to status server");
          m_timeout = 0;
          return STSSRV__SUCCESS;
        }

        if (data_size == 0) {
          m_sts = STSSRV__CONNDOWN;
          close(m_sock);
          m_timeout = 0;
          //errh_Error("Error reading data. Connection down to status server");
          return STSSRV__SUCCESS;
        }

        remaining_data = remaining_data - data_size;
        received_data = received_data + data_size;

        if ((received_data >= sizeof(stssrv_sMsgHead)) && (size_of_msg == 0)) {
          // Compute the complete header
          trans_id = rb->id;
          size_of_msg = rb->length;

          // Check header data
          if (trans_id == 0 || size_of_msg < sizeof(stssrv_sMsgHead)) {
            m_sts = STSSRV__CONNDOWN;
            close(m_sock);
            //errh_Error("Invalid packet. Connection down to status server");
            m_timeout = 0;
            return STSSRV__SUCCESS;
          }

          // Update remaining data
          remaining_data = size_of_msg - received_data;
        }
      }
    } // while

    if (sts > 0) { // processing packet...

      m_msgs_lost = 0;
      m_timeout = 0;

      m_expected_msgs--;
      *type = (stssrv_eMsgType)rb->type;
      *msg = (char *)rb;
    }
  }

  return STSSRV__SUCCESS;
}

void get_port(pwr_tStatus *sts, unsigned short *port)
{
  int bus = syi_Busid(sts);
  if (EVEN(*sts))
    return;

  *port = 56000 + bus;
  *sts = STSSRV__SUCCESS;
}
