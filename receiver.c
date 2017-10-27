#include "receiver.h"
/**
TODO: if super big data, fragament, then need to figure out how to combine them into one message
 */

/**
TODO: CHeck the retransnit file
 */
char * msg = "";
void init_receiver(Receiver * receiver,
    int id)
{
  receiver->recv_id = id;
  receiver->input_framelist_head = NULL;
  // debug TODO
  int j = 0;
  for(j = 0; j < RECEIVER_SIZE; j++) {
    receiver->recvArr[j].NFE = 0;
    receiver->recvArr[j].LFR = -1;
    receiver->recvArr[j].LAF = receiver->recvArr[j].NFE + RWS - 1;

    int i;
    for (i = 0; i < BUFFER_SIZE; i++) 
    {
      //receiver->recvQ[i].received = 0;
      receiver->recvArr[j].recvQ[i].received = 0;
    }
  }
}


void handle_incoming_msgs(Receiver * receiver,
    LLnode ** outgoing_frames_head_ptr)
{
  int errorDetected = 0;
  int incoming_msgs_length = ll_get_length(receiver->input_framelist_head);
  while (incoming_msgs_length > 0)
  {
    //Pop a node off the front of the link list and update the count
    LLnode * ll_inmsg_node = ll_pop_node(&receiver->input_framelist_head);
    incoming_msgs_length = ll_get_length(receiver->input_framelist_head);
    char * raw_char_buf = (char *) ll_inmsg_node->value;
    Frame * inframe;
    //int array_len = sizeof(raw_char_buf) / sizeof(raw_char_buf[0]);
    int array_len = MAX_FRAME_SIZE;
    if(is_corrupted(raw_char_buf, array_len) == 0) 
    {
      // check whether the frame is corrupted
      // no error in crc
      inframe = convert_char_to_frame(raw_char_buf);
    }
    else 
    {
      free(ll_inmsg_node);
      continue;
    }
    if(receiver->recv_id == inframe->dst_id) 
    {
      uint8_t seqNo = inframe->seqNum;
      int send_id = inframe->src_id;

      // case 1: usual case 
      if((receiver->recvArr[send_id].LAF >= receiver->recvArr[send_id].NFE) && (seqNo >= receiver->recvArr[send_id].NFE) && (seqNo <= receiver->recvArr[send_id].LAF)) 
      { 
        if(seqNo != receiver->recvArr[send_id].NFE) 
        {
          errorDetected = 1;
        }
        if(errorDetected == 0) 
        {
          receiver->recvArr[send_id].NFE = seqNo + 1;
          receiver->recvArr[send_id].LFR = receiver->recvArr[send_id].NFE - 1;
          receiver->recvArr[send_id].LAF = receiver->recvArr[send_id].NFE + RWS - 1;

          // store the frame in the receive window and set the recived to 1
          receiver->recvArr[send_id].recvQ[receiver->recvArr[send_id].LFR % BUFFER_SIZE].frame = inframe;
          receiver->recvArr[send_id].recvQ[receiver->recvArr[send_id].LFR % BUFFER_SIZE].received = 1;
          if(inframe->sameMsg == 0) {
            printf("<RECV_%d>:[%s]\n", receiver->recv_id, inframe->data);
          }
          else if(inframe->sameMsg == 1)
          {
            msg = concat(msg, inframe->data);
          }
          else if(inframe->sameMsg == 2) 
          {
            msg = concat(msg, inframe->data);
            printf("<RECV_%d>:[%s]\n", receiver->recv_id, msg);
            msg = "";
          }

          // send back the ack since the sequence is in the window
          Frame * outgoing_frame = (Frame *) malloc(sizeof(Frame));
          memset(outgoing_frame, 0, sizeof(Frame));
          outgoing_frame->src_id = receiver->recv_id;
          outgoing_frame->dst_id = inframe->src_id;
          outgoing_frame->ackNum = receiver->recvArr[send_id].LFR; 
          outgoing_frame->crc = 0x00;
          char * outgoing_charbuf = convert_frame_to_char(outgoing_frame);
          //int array_len = sizeof(outgoing_charbuf) / sizeof(outgoing_charbuf[0]);
          int array_len = MAX_FRAME_SIZE;
          append_crc(outgoing_charbuf, array_len);
          ll_append_node(outgoing_frames_head_ptr, outgoing_charbuf);
        }
        else if(errorDetected == 1) 
        {
          // store the frame in the receive window and set the recived to 1
          receiver->recvArr[send_id].recvQ[inframe->seqNum % BUFFER_SIZE].frame = inframe;
          receiver->recvArr[send_id].recvQ[inframe->seqNum % BUFFER_SIZE].received = 1;
        }
        incoming_msgs_length = ll_get_length(receiver->input_framelist_head);
      }
      //case 2: sequence number wrap around
      else if((receiver->recvArr[send_id].LAF < receiver->recvArr[send_id].NFE) && ((seqNo >= receiver->recvArr[send_id].NFE) || (seqNo <= receiver->recvArr[send_id].LAF))) 
      {
        //printf("<RECV_%d>:[%s]\n", receiver->recv_id, inframe->data);
        if(seqNo != receiver->recvArr[send_id].NFE) 
        {
          errorDetected = 1;
        }
        if(errorDetected == 0) 
        {
          receiver->recvArr[send_id].NFE = seqNo + 1;
          receiver->recvArr[send_id].LFR = receiver->recvArr[send_id].NFE - 1;
          receiver->recvArr[send_id].LAF = receiver->recvArr[send_id].NFE + RWS - 1;

          // store the frame in the receive window and set the recived to 1
          receiver->recvArr[send_id].recvQ[receiver->recvArr[send_id].LFR % BUFFER_SIZE].frame = inframe;
          receiver->recvArr[send_id].recvQ[receiver->recvArr[send_id].LFR % BUFFER_SIZE].received = 1;
          if(inframe->sameMsg == 0) {
            printf("<RECV_%d>:[%s]\n", receiver->recv_id, inframe->data);
          }
          else if(inframe->sameMsg == 1)
          {
            msg = concat(msg, inframe->data);
          }
          else if(inframe->sameMsg == 2) 
          {
            msg = concat(msg, inframe->data);
            printf("<RECV_%d>:[%s]\n", receiver->recv_id, msg);
            msg = "";
          }

          // send back the ack since the sequence is in the window
          Frame * outgoing_frame = (Frame *) malloc(sizeof(Frame));
          outgoing_frame->src_id = receiver->recv_id;
          outgoing_frame->dst_id = inframe->src_id;
          outgoing_frame->ackNum = receiver->recvArr[send_id].LFR; 
          outgoing_frame->crc = 0x00;
          char * outgoing_charbuf = convert_frame_to_char(outgoing_frame);
          //int array_len = sizeof(outgoing_charbuf) / sizeof(outgoing_charbuf[0]);
          int array_len = MAX_FRAME_SIZE;
          append_crc(outgoing_charbuf, array_len);
          ll_append_node(outgoing_frames_head_ptr, outgoing_charbuf);
        }
        else if(errorDetected == 1) 
        {
          // store the frame in the receive window and set the recived to 1
          receiver->recvArr[send_id].recvQ[inframe->seqNum % BUFFER_SIZE].frame = inframe;
          receiver->recvArr[send_id].recvQ[inframe->seqNum % BUFFER_SIZE].received = 1;
        }
        incoming_msgs_length = ll_get_length(receiver->input_framelist_head);
      }
    }
    else 
    {
      free(ll_inmsg_node);
      continue;
    }
    free(ll_inmsg_node);
  }
}

void * run_receiver(void * input_receiver)
{    
  struct timespec   time_spec;
  struct timeval    curr_timeval;
  const int WAIT_SEC_TIME = 0;
  const long WAIT_USEC_TIME = 100000;
  Receiver * receiver = (Receiver *) input_receiver;
  LLnode * outgoing_frames_head;

  pthread_cond_init(&receiver->buffer_cv, NULL);
  pthread_mutex_init(&receiver->buffer_mutex, NULL);

  while(1)
  {    
    //NOTE: Add outgoing messages to the outgoing_frames_head pointer
    outgoing_frames_head = NULL;
    gettimeofday(&curr_timeval, 
        NULL);
    time_spec.tv_sec  = curr_timeval.tv_sec;
    time_spec.tv_nsec = curr_timeval.tv_usec * 1000;
    time_spec.tv_sec += WAIT_SEC_TIME;
    time_spec.tv_nsec += WAIT_USEC_TIME * 1000;
    if (time_spec.tv_nsec >= 1000000000)
    {
      time_spec.tv_sec++;
      time_spec.tv_nsec -= 1000000000;
    }
    pthread_mutex_lock(&receiver->buffer_mutex);

    //Check whether anything arrived
    int incoming_msgs_length = ll_get_length(receiver->input_framelist_head);
    if (incoming_msgs_length == 0)
    {
      //Nothing has arrived, do a timed wait on the condition variable (which releases the mutex). Again, you don't really need to do the timed wait.
      //A signal on the condition variable will wake up the thread and reacquire the lock
      pthread_cond_timedwait(&receiver->buffer_cv, 
          &receiver->buffer_mutex,
          &time_spec);
    }

    handle_incoming_msgs(receiver,
        &outgoing_frames_head);

    pthread_mutex_unlock(&receiver->buffer_mutex);

    //CHANGE THIS AT YOUR OWN RISK!
    //Send out all the frames user has appended to the outgoing_frames list
    int ll_outgoing_frame_length = ll_get_length(outgoing_frames_head);
    while(ll_outgoing_frame_length > 0)
    {
      LLnode * ll_outframe_node = ll_pop_node(&outgoing_frames_head);
      char * char_buf = (char *) ll_outframe_node->value;

      //The following function frees the memory for the char_buf object
      send_msg_to_senders(char_buf);

      //Free up the ll_outframe_node
      free(ll_outframe_node);

      ll_outgoing_frame_length = ll_get_length(outgoing_frames_head);
    }
  }
  pthread_exit(NULL);

}
