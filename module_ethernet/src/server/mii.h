// Copyright (c) 2011, XMOS Ltd., All rights reserved
// This software is freely distributable under a derivative of the
// University of Illinois/NCSA Open Source License posted in
// LICENSE.txt and at <http://github.xcore.com/>

#ifndef __mii_h__
#define __mii_h__
#include <xs1.h>
#include <xccompat.h>

#ifdef __ethernet_conf_h_exists__
#include "ethernet_conf.h"
#endif

#ifndef MAX_ETHERNET_PACKET_SIZE
#define MAX_ETHERNET_PACKET_SIZE (1518)
#endif

#ifndef NUM_MII_RX_BUF 
#define NUM_MII_RX_BUF 5
#endif

#ifndef NUM_MII_TX_BUF 
#define NUM_MII_TX_BUF 5
#endif

#ifndef MAC_REQUIRED_WORDS_TO_FILTER
#define MAC_REQUIRED_WORDS_TO_FILTER (4)
#endif

#ifdef ETHERNET_HP_QUEUE

#ifndef MII_RX_BUFSIZE_HIGH_PRIORITY
#define MII_RX_BUFSIZE_HIGH_PRIORITY (2048)
#endif

#ifndef MII_RX_BUFSIZE_LOW_PRIORITY
#define MII_RX_BUFSIZE_LOW_PRIORITY (4096)
#endif

#else 

#ifndef MII_RX_BUFSIZE
#define MII_RX_BUFSIZE (4096)
#endif

#define MII_RX_BUFSIZE_LOW_PRIORITY MII_RX_BUFSIZE

#endif



#ifndef MII_TX_BUFSIZE
#define MII_TX_BUFSIZE (2048)
#endif

#ifndef ETHERNET_MAX_TX_PACKET_SIZE
#define ETHERNET_MAX_TX_PACKET_SIZE (1518)
#endif

#include "mii_queue.h"



#ifdef __XC__
/** Structure containing resources required for the MII ethernet interface.
 *
 *  This structure contains resources required to make up an MII interface. 
 *  It consists of 7 ports and 2 clock blocks.
 *
 *  The clock blocks can be any available clock blocks and will be clocked of 
 *  incoming rx/tx clock pins.
 *
 *  \sa ethernet_server()
 **/
typedef struct mii_interface_t {
  clock clk_mii_rx;            /**< MII RX Clock Block **/
  clock clk_mii_tx;            /**< MII TX Clock Block **/

  in port p_mii_rxclk;         /**< MII RX clock wire */
  in port p_mii_rxer;          /**< MII RX error wire */
  in buffered port:32 p_mii_rxd; /**< MII RX data wire */
  in port p_mii_rxdv;          /**< MII RX data valid wire */


  in port p_mii_txclk;       /**< MII TX clock wire */
  out port p_mii_txen;       /**< MII TX enable wire */
  out buffered port:32 p_mii_txd; /**< MII TX data wire */
} mii_interface_t;

void mii_init(REFERENCE_PARAM(mii_interface_t, m));
#endif


typedef struct mii_packet_t {
  #define BUF_OFFSET_length 0
  int length;
  #define BUF_OFFSET_complete 1
  int complete;
  #define BUF_OFFSET_timestamp 2
  int timestamp;
  #define BUF_OFFSET_filter_result 3
  int filter_result;
  #define BUF_OFFSET_src_port 4
  int src_port;
  #define BUF_OFFSET_timestamp_id 5
  int timestamp_id; 
  #define BUF_OFFSET_stage 6
  int stage;
  #define BUF_OFFSET_tcount 7
  int tcount;
  #define BUF_OFFSET_crc 8
  int crc;
  #define BUF_DATA_OFFSET 9
  unsigned int data[(MAX_ETHERNET_PACKET_SIZE+3)/4];
} mii_packet_t;

#define STRINGIFY0(x) #x
#define STRINGIFY(x) STRINGIFY0(x)



#ifdef __XC__
#define create_buf_getset(field) \
  inline int get_buf_##field (int buf) { \
    int x; \
    asm("ldw %0,%1[" STRINGIFY(BUF_OFFSET_ ## field) "]":"=r"(x):"r"(buf)); \
    return x; \
 } \
 inline void set_buf_##field (int buf, int x) { \
  asm("stw %1, %0[" STRINGIFY(BUF_OFFSET_ ## field) "]"::"r"(buf),"r"(x)); \
 }

create_buf_getset(length)
create_buf_getset(complete)
create_buf_getset(timestamp)
create_buf_getset(filter_result)
create_buf_getset(src_port)
create_buf_getset(timestamp_id)
create_buf_getset(stage)
create_buf_getset(crc)
  
inline int get_buf_data_ptr(int buf) {
  return (buf+BUF_DATA_OFFSET*4);
}

inline void set_data_word(int data, int n, int v) {
  asm("stw %0,%1[%2]"::"r"(v),"r"(data),"r"(n));
}

inline int get_data_word(int data, int n) {
  int x;
  asm("ldw %0,%1[%2]":"=r"(x):"r"(data),"r"(n));
  return x;
}

#define set_data_word_imm(data, n, v) \
  asm("stw %0,%1[" STRINGIFY(n) "]"::"r"(v),"r"(data));


inline int get_buf_data(int buf, int n) {
  int x;
  asm("ldw %0,%1[%2]":"=r"(x):"r"(buf),"r"(n+BUF_DATA_OFFSET));
  return x;
}

inline void set_buf_data(int buf, int n, int v) {
  asm("stw %0,%1[%2]"::"r"(v),"r"(buf),"r"(n+BUF_DATA_OFFSET));
  return;
}




#endif


#ifdef __XC__
void mii_rx_pins(int rxmem_hp,
                 int rxmem_lp,
                 in port p_mii_rxdv,
                 in buffered port:32 p_mii_rxd,
                 int ifnum,
                 streaming chanend c);
#else
void mii_rx_pins(int rxmem_hp,
                 int rxmem_lp,
                 port p_mii_rxdv,
                 port p_mii_rxd,
                 int ifnum,
                 chanend c);
#endif

#ifdef __XC__
void mii_tx_pins(int mempool,
                 mii_queue_t &in_queue,
                 mii_queue_t &ts_queue,
                 out buffered port:32 p_mii_txd,
                 int ifnum);
#else
void mii_tx_pins(int mempool,
                 mii_queue_t *in_queue,
                 mii_queue_t *ts_queue,
                 port p_mii_txd,
                 int ifnum);
#endif



#endif