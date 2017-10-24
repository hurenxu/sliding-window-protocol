#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <math.h>
#include <sys/time.h>

#define MAX_COMMAND_LENGTH 16
#define AUTOMATED_FILENAME 512
#define BUFFER_SIZE 16
#define RECEIVER_SIZE 255 
typedef unsigned char uchar_t;

//System configuration information
struct SysConfig_t
{
  float drop_prob;
  float corrupt_prob;
  unsigned char automated;
  char automated_file[AUTOMATED_FILENAME];
};
typedef struct SysConfig_t  SysConfig;

//Command line input information
struct Cmd_t
{
  uint16_t src_id;
  uint16_t dst_id;
  uint8_t sameMsg;
  char * message;
};
typedef struct Cmd_t Cmd;

//Linked list information
enum LLtype 
{
  llt_string,
  llt_frame,
  llt_integer,
  llt_head
} LLtype;

struct LLnode_t
{
  struct LLnode_t * prev;
  struct LLnode_t * next;
  enum LLtype type;

  void * value;
};
typedef struct LLnode_t LLnode;



enum SendFrame_DstType 
{
  ReceiverDst,
  SenderDst
} SendFrame_DstType ;


#define MAX_FRAME_SIZE 64
/**
#define ENCAPSULATE_SIZE 6
#define ADDRESS_FIELD_SIZE 1
#define SEQ_SIZE 1
#define CONTROL_FIELD_SIZE 1
#define FRAME_PAYLOAD_SIZE 58
#define CRC_SIZE 2
 */
#define ENCAPSULATE_SIZE 7
#define SRCID_SIZE 1
#define DSTID_SIZE 1
#define BOOL_SIZE 1
#define SEQ_SIZE 1
#define ACK_SIZE 1
#define TYPE_SIZE 1
#define FRAME_PAYLOAD_SIZE 57
#define CRC_SIZE 1

//uint8_t acknowNum = 0;

//TODO: You should change this!
//Remember, your frame can be AT MOST 64 bytes!
//#define FRAME_PAYLOAD_SIZE 64
struct Frame_t
{
  // one byte for source id
  uint8_t src_id;
  // one byte for destination id
  uint8_t dst_id;
  // one byte for sequence number
  uint8_t seqNum;
  uint8_t ackNum;
  // 0 represents seq frame, 1 represents ack frame
  uint8_t type;
  // if smaeMsg == 0 then it is different message
  uint8_t sameMsg;
  /**
  // one byte for both begin seq and end seq
  char beginSeq;
  char endSeq;
  // two bytes for header
  char address_field;
  char control_field;*/
  // payload
  char data[FRAME_PAYLOAD_SIZE];
  // two bytes for crc error detection
  char crc;
};
typedef struct Frame_t Frame;

// TODO: change the window size if works
#define SWS 8
#define RWS 8

// sender side state
struct Sender_sendQ_for_each_Receiver {
  // initialization in the header, or else compile error
  uint8_t seq; // sequence number for the frame that stored in the sender`
  uint8_t LAR; // last ACK received
  uint8_t LFS; // last frame sent
  struct sendQ_slot {
    struct timeval * timeout; /* event associated with send -timeout */
    Frame * frame;
  } sendQ[BUFFER_SIZE];
};
typedef struct Sender_sendQ_for_each_Receiver sendArray;

//Receiver and sender data structures
struct Receiver_t
{
  //DO NOT CHANGE:
  // 1) buffer_mutex
  // 2) buffer_cv
  // 3) input_framelist_head
  // 4) recv_id
  pthread_mutex_t buffer_mutex;
  pthread_cond_t buffer_cv;
  LLnode * input_framelist_head;

  int recv_id;

  struct recvQ_slot {
    int received; // is msg valid?
    // 1 means the message is valid and has received
    Frame * frame;
  } recvQ[BUFFER_SIZE];

  // receiver side state
  // initialization in the header, or else compile error
  uint8_t NFE; // next frame expected
  uint8_t LFR; // last frame recieved
  uint8_t LAF; // last acceptable frame
};

struct Sender_t
{
  //DO NOT CHANGE:
  // 1) buffer_mutex
  // 2) buffer_cv
  // 3) input_cmdlist_head
  // 4) input_framelist_head
  // 5) send_id
  pthread_mutex_t buffer_mutex;
  pthread_cond_t buffer_cv;    
  LLnode * input_cmdlist_head;
  LLnode * input_framelist_head;
  int send_id;
  int inputFrameSize;
  int start;

  sendArray sendArr[RECEIVER_SIZE];
};

typedef struct Sender_t Sender;
typedef struct Receiver_t Receiver;


//Declare global variables here
//DO NOT CHANGE: 
//   1) glb_senders_array
//   2) glb_receivers_array
//   3) glb_senders_array_length
//   4) glb_receivers_array_length
//   5) glb_sysconfig
//   6) CORRUPTION_BITS
Sender * glb_senders_array;
Receiver * glb_receivers_array;
int glb_senders_array_length;
int glb_receivers_array_length;
SysConfig glb_sysconfig;
int CORRUPTION_BITS;
#endif 
