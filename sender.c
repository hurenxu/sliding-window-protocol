#include "sender.h"

void init_sender(Sender * sender, int id)
{
    //TODO: You should fill in this function as necessary
    sender->send_id = id;
    sender->input_cmdlist_head = NULL;
    sender->input_framelist_head = NULL;

    sender->inputFrameSize = 0;
    // initilization, since i got a compile error initilizating in struct
    int i;
    for(i = 0; i < RECEIVER_SIZE; i++) 
    {
      sender->sendArr[i].seq = -1;
      sender->sendArr[i].LAR = -1;
      sender->sendArr[i].LFS = -1;
    }
    sender->start = 0;
}

void calculate_timeout(struct timeval * timeout){ 
  gettimeofday(timeout, NULL); 
  timeout->tv_usec+=100000; 
  if(timeout->tv_usec>=1000000){  
    timeout->tv_usec-=1000000;  
    timeout->tv_sec+=1;  
  } 
}

struct timeval * sender_get_next_expiring_timeval(Sender * sender)
{
    //TODO: You should fill in this function so that it returns the next timeout that should occur
    return NULL;
}


void handle_incoming_acks(Sender * sender,
                          LLnode ** outgoing_frames_head_ptr)
{
  //TODO: Suggested steps for handling incoming ACKs
  int incoming_acks_length = ll_get_length(sender->input_framelist_head);
  while (incoming_acks_length > 0)
  {
    //    1) Dequeue the ACK from the sender->input_framelist_head
    LLnode* ACK = ll_pop_node(&(sender->input_framelist_head));
    incoming_acks_length = ll_get_length(sender->input_framelist_head);
    //    2) Convert the char * buffer to a Frame data type
    char * raw_char_buf = (char*) ACK->value; 
    //int array_len = sizeof(raw_char_buf) / sizeof(raw_char_buf[0]);
    int array_len = MAX_FRAME_SIZE;
    Frame* ackFrame = convert_char_to_frame(raw_char_buf);
    //    3) Check whether the frame is corrupted
    if(is_corrupted(raw_char_buf, array_len) == 0) 
    {
      // no error in crc
    }
    else 
    {
      free(ackFrame);
      free(ACK);
      // continue the loop and ignore the data
      continue;
    }
    //    4) Check whether the frame is for this sende
    if(sender->send_id == ackFrame->dst_id) 
    {
      int recv_id = ackFrame->src_id;
      // standard message 
      //printf("<SEND_%d>:[%s] ACK\n", sender->send_id, ackFrame->data);
      //    5) Do sliding window protocol for sender/receiver pair   
      // the window is from [LAR+1, LFS]
      // TODO: wrap around
      if(sender->sendArr[recv_id].LFS < sender->sendArr[recv_id].LAR) 
      {
        if(sender->start == 0) 
        {
        
        }
        else 
        {
          sender->sendArr[recv_id].LFS += BUFFER_SIZE;
        }
      }
      uint8_t possible = sender->inputFrameSize - (sender->sendArr[recv_id].LFS - sender->sendArr[recv_id].LAR);
      // TODO: wrap around
      if(sender->start == 1) {
        sender->sendArr[recv_id].LFS %= BUFFER_SIZE;
      }
      uint8_t previousLAR = sender->sendArr[recv_id].LAR;
      // TODO: Wrap around
      uint8_t ackReceived = ackFrame->ackNum % BUFFER_SIZE - sender->sendArr[recv_id].LAR;
      // TODO: Wrap around
      if(ackFrame->ackNum % BUFFER_SIZE < sender->sendArr[recv_id].LAR) 
      {
        if(sender->start == 0) 
        {
        
        }
        else 
        {
          ackReceived = ackFrame->ackNum % BUFFER_SIZE + BUFFER_SIZE - sender->sendArr[recv_id].LAR;
        }
      }
      sender->sendArr[recv_id].LAR = ackFrame->ackNum;
      uint8_t ackNo = ackFrame->ackNum;
      // TODO: Wrap Around
      if(sender->sendArr[recv_id].LAR >= BUFFER_SIZE) 
      {
        if(sender->start == 0) 
        {
        
        }
        else 
        {
          sender->sendArr[recv_id].LAR %= BUFFER_SIZE;
        }
      }
      // deal with retransmit
      // last frame ack in the window should be LAR, check whether it equals to
      // LFS, since LFS should be the last acknoledgement receive.
      //TODO: debugging
      //if(sender->sendArr[recv_id].LAR != sender->sendArr[recv_id].LFS) 
      //if(sender->sendArr[recv_id].LAR != sender->sendArr[recv_id].LFS) 
      //{
      //uint8_t previousLAR = sender->sendArr[recv_id].LAR;
      if((previousLAR <= sender->sendArr[recv_id].LFS) && (ackNo > previousLAR) && (ackNo <= sender->sendArr[recv_id].LFS)) 
      { 
        sender->inputFrameSize = sender->inputFrameSize - ackReceived;
 /**
         // TODO
         //ackReceived = sender->sendArr[recv_id].LFS - sender->sendArr[recv_id].LAR;
         // first find the window inside the senderArr
         sendArray sendBuffer = sender->sendArr[recv_id];
         // send the next frame which squence number is right next to the LAR
         int seqNum = sender->sendArr[recv_id].LAR + 1;
         struct sendQ_slot *slot;
         //This is probably ONLY one step you want
         Frame * outgoing_frame = (Frame *) malloc (sizeof(Frame));
	       //slot = sendQ[seqNum];
         slot = &(sendBuffer.sendQ[seqNum % BUFFER_SIZE]);
         outgoing_frame = slot->frame;

         //Convert the message to the outgoing_charbuf
         char * outgoing_charbuf = convert_frame_to_char(outgoing_frame);
         ll_append_node(outgoing_frames_head_ptr,
                       outgoing_charbuf);
         //free(outgoing_frame);
         // after sending out the missing frame, upate the LAR, since we need to
         // do the selective transmission
         sender->sendArr[recv_id].LAR = sender->sendArr[recv_id].LFS;
         // check how many inputFrameSize received ack
         */
      }
      else if((previousLAR > sender->sendArr[recv_id].LFS) && ((ackNo > previousLAR) || (ackNo <= sender->sendArr[recv_id].LFS))) 
      {
        sender->inputFrameSize = sender->inputFrameSize - ackReceived;
      }
      else 
      {
         //TODO
         //ackReceived = sender->sendArr[recv_id].LFS - sender->sendArr[recv_id].LAR;
         // first find the window inside the senderArr
         sendArray sendBuffer = sender->sendArr[recv_id];
         // send the next frame which squence number is right next to the LAR
         int seqNum = sender->sendArr[recv_id].LAR + 1;
         struct sendQ_slot *slot;
         //This is probably ONLY one step you want
         Frame * outgoing_frame = (Frame *) malloc (sizeof(Frame));
	       //slot = sendQ[seqNum];
         slot = &(sendBuffer.sendQ[seqNum % BUFFER_SIZE]);
         outgoing_frame = slot->frame;

         //Convert the message to the outgoing_charbuf
         char * outgoing_charbuf = convert_frame_to_char(outgoing_frame);
         ll_append_node(outgoing_frames_head_ptr,
                       outgoing_charbuf);
         //free(outgoing_frame);
         // after sending out the missing frame, upate the LAR, since we need to
         // do the selective transmission
         sender->sendArr[recv_id].LAR = sender->sendArr[recv_id].LFS;
         // check how many inputFrameSize received ack

      }
      /**
      else 
      {
        //sender->inputFrameSize--;
        sender->inputFrameSize = sender->inputFrameSize - ackReceived;
      }*/
      // not run the program if all the frames have been sent
      //if(sender->inputFrameSize == 0) 
      if(sender->inputFrameSize <= 0) 
      {
        // continue, not run anything
        continue;
      }
      //LFS += SWS;
      // size that the send window can move
      // TODO: Wrap around
      if(sender->sendArr[recv_id].LFS < sender->sendArr[recv_id].LAR) 
      {
        if(sender->start == 0) 
        {
        
        }
        else 
        {
          sender->sendArr[recv_id].LFS += BUFFER_SIZE;
        }
      }
      uint8_t offset = SWS - (sender->sendArr[recv_id].LFS - sender->sendArr[recv_id].LAR);
      // TODO: Wrap around
      if(sender->start == 1) {
        sender->sendArr[recv_id].LFS %= BUFFER_SIZE;
      }
      /**
      if(offset > sender->inputFrameSize) 
      {
        offset = sender->inputFrameSize;
      }
      else 
      {
        sender->sendArr[recv_id].LFS += offset;
      }*/
      
      //if(offset > sender->inputFrameSize) 
      //{
        //offset = sender->inputFrameSize;
      //}
      //printf("%d offset & %d inputFrameSize\n", offset, sender->inputFrameSize);
      //TODO: check at the end when the size is less than the input frame size, 
      // what whould be the possible sliding window to update
      if(offset > possible) 
      {
        //offset = sender->inputFrameSize;
        offset = possible;
        sender->inputFrameSize = sender->inputFrameSize - ackReceived;
      }
      sender->sendArr[recv_id].LFS += offset;
      //TODO: wrap around
      if(sender->sendArr[recv_id].LFS >= BUFFER_SIZE) 
      {
        if(sender->start == 1) {
          sender->sendArr[recv_id].LFS %= BUFFER_SIZE;
        }
      }
      // the window should start from LAR + 1, and end in LFS
      // send the frame from LAR+1, LFS
      // case 1 usual case
      if(sender->sendArr[recv_id].LAR <= sender->sendArr[recv_id].LFS) {
        uint8_t i; 
        for(i = sender->sendArr[recv_id].LAR + 1; (i > sender->sendArr[recv_id].LAR) && (i <= sender->sendArr[recv_id].LFS); i++) 
        {
            // first find the window inside the senderArr
            sendArray sendBuffer = sender->sendArr[recv_id];
            struct sendQ_slot *slot;
            //This is probably ONLY one step you want
            Frame * outgoing_frame = (Frame *) malloc (sizeof(Frame));
	          //slot = sendQ[i];
            slot = &(sendBuffer.sendQ[i % BUFFER_SIZE]);
            outgoing_frame = slot->frame;

            //Convert the message to the outgoing_charbuf
            char * outgoing_charbuf = convert_frame_to_char(outgoing_frame);
            ll_append_node(outgoing_frames_head_ptr,
                           outgoing_charbuf);
            //free(outgoing_frame);
        }
      }
      // case 2 sequence number wrap around
      else if(sender->sendArr[recv_id].LAR > sender->sendArr[recv_id].LFS) {
        uint8_t i;
        for(i = sender->sendArr[recv_id].LAR + 1; (i > sender->sendArr[recv_id].LAR) || (i <= sender->sendArr[recv_id].LFS); i++) 
        {
            // first find the window inside the senderArr
            sendArray sendBuffer = sender->sendArr[recv_id];
            struct sendQ_slot *slot;
            //This is probably ONLY one step you want
            Frame * outgoing_frame = (Frame *) malloc (sizeof(Frame));
	          //slot = sendQ[i];
            slot = &(sendBuffer.sendQ[i % BUFFER_SIZE]);
            outgoing_frame = slot->frame;

            //Convert the message to the outgoing_charbuf
            char * outgoing_charbuf = convert_frame_to_char(outgoing_frame);
            ll_append_node(outgoing_frames_head_ptr,
                           outgoing_charbuf);
            //free(outgoing_frame);
        }
      }
    }
    else 
    {
      free(ackFrame);
      free(ACK);
      continue;
    }
    free(ackFrame);
    free(ACK);
  }
}


void handle_input_cmds(Sender * sender,
                       LLnode ** outgoing_frames_head_ptr)
{
    //TODO: Suggested steps for handling input cmd
    //    1) Dequeue the Cmd from sender->input_cmdlist_head *
    //    2) Convert to Frame *
    //    3) Set up the frame according to the sliding window protocol *
    //    4) Compute CRC and add CRC to Frame *
    sender->start = 1;
    int input_cmd_length = ll_get_length(sender->input_cmdlist_head);
    
        
    //Recheck the command queue length to see if stdin_thread dumped a command on us
    ll_split_head(&sender->input_cmdlist_head, FRAME_PAYLOAD_SIZE - 1); 
    input_cmd_length = ll_get_length(sender->input_cmdlist_head);
    int sendFrames = 0;
    while (input_cmd_length > 0)
    {
        //Pop a node off and update the input_cmd_length
        LLnode * ll_input_cmd_node = ll_pop_node(&sender->input_cmdlist_head);
        input_cmd_length = ll_get_length(sender->input_cmdlist_head);

        //Cast to Cmd type and free up the memory for the node
        Cmd * outgoing_cmd = (Cmd *) ll_input_cmd_node->value;
        free(ll_input_cmd_node);
            

        //DUMMY CODE: Add the raw char buf to the outgoing_frames list
        //NOTE: You should not blindly send this message out!
        //      Ask yourself: Is this message actually going to the right receiver (recall that default behavior of send is to broadcast to all receivers)?
        //                    Does the receiver have enough space in in it's input queue to handle this message?
        //                    Were the previous messages sent to this receiver ACTUALLY delivered to the receiver?
        int msg_length = strlen(outgoing_cmd->message);
        //if (msg_length > MAX_FRAME_SIZE)
        if (msg_length > FRAME_PAYLOAD_SIZE)
        {
            //Do something about messages that exceed the frame size
            //printf("<SEND_%d>: sending messages of length greater than %d is not implemented\n", sender->send_id, MAX_FRAME_SIZE);
            ll_split_head(&sender->input_cmdlist_head, FRAME_PAYLOAD_SIZE - 1); 
            input_cmd_length = ll_get_length(sender->input_cmdlist_head);
        }
        else
        {
            struct sendQ_slot *slot;
            //This is probably ONLY one step you want
            Frame * outgoing_frame = (Frame *) malloc (sizeof(Frame));
            strcpy(outgoing_frame->data, outgoing_cmd->message);
            outgoing_frame->src_id = outgoing_cmd->src_id;
            outgoing_frame->dst_id = outgoing_cmd->dst_id;
            if((outgoing_cmd->sameMsg != 2) && (outgoing_cmd->sameMsg != 1)) {
              outgoing_frame->sameMsg = 0; 
            }
            else {
              outgoing_frame->sameMsg = outgoing_cmd->sameMsg;
            }
            int recv_id = outgoing_frame->dst_id;
            //outgoing_frame->seqNum = ++LFS;
            sender->sendArr[recv_id].seq += 1;
            outgoing_frame->seqNum = sender->sendArr[recv_id].seq;
	          // the window for the sender
	          outgoing_frame->crc = 0x00;
            //sequenceNum++;
            //Convert the message to the outgoing_charbuf
            char * outgoing_charbuf = convert_frame_to_char(outgoing_frame);
	          //int array_len = sizeof(outgoing_charbuf) / sizeof(outgoing_charbuf[0]);
            int array_len = MAX_FRAME_SIZE;
            //already has extended 8 bits since crc == 0
	          append_crc(outgoing_charbuf, array_len);
            outgoing_frame = convert_char_to_frame(outgoing_charbuf);
	          slot = &(sender->sendArr[recv_id].sendQ[outgoing_frame->seqNum % BUFFER_SIZE]);
	          slot->frame = outgoing_frame;
            slot->timeout = malloc(sizeof(struct timeval));
            calculate_timeout(slot->timeout);
            //At this point, we don't need the outgoing_cmd
            sendFrames++;
            //sender->inputFrameSize = sendFrames;
            sender->inputFrameSize++;
            if(sendFrames <= RWS) {
              sender->sendArr[recv_id].LFS = sender->sendArr[recv_id].LFS + 1;
              // TODO: Wrap Around
              if(sender->sendArr[recv_id].LFS >= BUFFER_SIZE) 
              {
                if(sender->start == 1) {
                  sender->sendArr[recv_id].LFS %= BUFFER_SIZE;
                }
              }
              //sender->sendArr[recv_id].LFS = outgoing_frame->seqNum;
              ll_append_node(outgoing_frames_head_ptr,
                            outgoing_charbuf);
            }
            free(outgoing_cmd->message);
            free(outgoing_cmd);
            //free(outgoing_frame);
        }
    }   
}

void handle_timedout_frames(Sender * sender,
                            LLnode ** outgoing_frames_head_ptr)
{
    //TODO: Suggested steps for handling timed out datagrams

    //    1) Iterate through the sliding window protocol information you maintain for each receiver
    // so all the senders and get their sliding window protocol which is the 
    // sendQ in the sendArr
    int i;
    for(i = 0; i < RECEIVER_SIZE; i++) 
    {
      // this is the send array with the sendQ inside
      uint8_t LAR = sender->sendArr[i].LAR;
      uint8_t LFS = sender->sendArr[i].LFS;

      // this means all the send data has its acknoledgement
      if(LAR == LFS) 
      {
        continue;
      }

      if(LFS > MAX_FRAME_SIZE) 
      {
        continue;
      }

      //TODO: for wrap around if the LFS is less than LAR then just + the buffersize
      // check all the frame that is afte the LAR, and before the LFS
      // the same idea as this for(int j = LAR; j <= LFS; j++), but
      // avoid wrap around

      //    2) Locate frames that are timed out and add them to the outgoing frames
      int j;
      for(j = LAR + 1; j != LFS + 1; j++) 
      {
        if(j >= BUFFER_SIZE) 
        {
          j = j % BUFFER_SIZE; 
        }
        struct timeval * currtime = malloc(sizeof(struct timeval));
        gettimeofday(currtime, NULL);
        
        long diff = timeval_usecdiff(currtime, sender->sendArr[i].sendQ[j % BUFFER_SIZE].timeout);
        // since the diff is currtime - timeout, so if the diff is either 0 or
        // greater than zero(currtime is later than timeout)
        if (diff <= 0) 
        {
          //This is probably ONLY one step you want
          Frame * outgoing_frame = sender->sendArr[i].sendQ[j % BUFFER_SIZE].frame;
          //Convert the message to the outgoing_charbuf
          char * outgoing_charbuf = convert_frame_to_char(outgoing_frame);
          ll_append_node(outgoing_frames_head_ptr,
                         outgoing_charbuf);

          //    3) Update the next timeout field on the outgoing frames
          calculate_timeout(sender->sendArr[i].sendQ[j % BUFFER_SIZE].timeout);
          //free(outgoing_frame);
        }
      }
    }
}


void * run_sender(void * input_sender)
{    
    struct timespec   time_spec;
    struct timeval    curr_timeval;
    const int WAIT_SEC_TIME = 0;
    const long WAIT_USEC_TIME = 100000;
    Sender * sender = (Sender *) input_sender;    
    LLnode * outgoing_frames_head;
    struct timeval * expiring_timeval;
    long sleep_usec_time, sleep_sec_time;
    
    //This incomplete sender thread, at a high level, loops as follows:
    //1. Determine the next time the thread should wake up
    //2. Grab the mutex protecting the input_cmd/inframe queues
    //3. Dequeues messages from the input queue and adds them to the outgoing_frames list
    //4. Releases the lock
    //5. Sends out the messages

    pthread_cond_init(&sender->buffer_cv, NULL);
    pthread_mutex_init(&sender->buffer_mutex, NULL);

    while(1)
    {    
        outgoing_frames_head = NULL;

        //Get the current time
        gettimeofday(&curr_timeval, 
                     NULL);

        //time_spec is a data structure used to specify when the thread should wake up
        //The time is specified as an ABSOLUTE (meaning, conceptually, you specify 9/23/2010 @ 1pm, wakeup)
        time_spec.tv_sec  = curr_timeval.tv_sec;
        time_spec.tv_nsec = curr_timeval.tv_usec * 1000;

        //Check for the next event we should handle
        expiring_timeval = sender_get_next_expiring_timeval(sender);

        //Perform full on timeout
        if (expiring_timeval == NULL)
        {
            time_spec.tv_sec += WAIT_SEC_TIME;
            time_spec.tv_nsec += WAIT_USEC_TIME * 1000;
        }
        else
        {
            //Take the difference between the next event and the current time
            sleep_usec_time = timeval_usecdiff(&curr_timeval,
                                               expiring_timeval);

            //Sleep if the difference is positive
            if (sleep_usec_time > 0)
            {
                sleep_sec_time = sleep_usec_time/1000000;
                sleep_usec_time = sleep_usec_time % 1000000;   
                time_spec.tv_sec += sleep_sec_time;
                time_spec.tv_nsec += sleep_usec_time*1000;
            }   
        }

        //Check to make sure we didn't "overflow" the nanosecond field
        if (time_spec.tv_nsec >= 1000000000)
        {
            time_spec.tv_sec++;
            time_spec.tv_nsec -= 1000000000;
        }

        
        //*****************************************************************************************
        //NOTE: Anything that involves dequeing from the input frames or input commands should go 
        //      between the mutex lock and unlock, because other threads CAN/WILL access these structures
        //*****************************************************************************************
        pthread_mutex_lock(&sender->buffer_mutex);

        //Check whether anything has arrived
        int input_cmd_length = ll_get_length(sender->input_cmdlist_head);
        int inframe_queue_length = ll_get_length(sender->input_framelist_head);
        
        //Nothing (cmd nor incoming frame) has arrived, so do a timed wait on the sender's condition variable (releases lock)
        //A signal on the condition variable will wakeup the thread and reaquire the lock
        if (input_cmd_length == 0 &&
            inframe_queue_length == 0)
        {
            
            pthread_cond_timedwait(&sender->buffer_cv, 
                                   &sender->buffer_mutex,
                                   &time_spec);
        }
        //Implement this
        handle_incoming_acks(sender,
                             &outgoing_frames_head);

        //Implement this
        handle_input_cmds(sender,
                          &outgoing_frames_head);

        pthread_mutex_unlock(&sender->buffer_mutex);


        //Implement this
        handle_timedout_frames(sender,
                               &outgoing_frames_head);

        //CHANGE THIS AT YOUR OWN RISK!
        //Send out all the frames
        int ll_outgoing_frame_length = ll_get_length(outgoing_frames_head);
        
        while(ll_outgoing_frame_length > 0)
        {
            LLnode * ll_outframe_node = ll_pop_node(&outgoing_frames_head);
            char * char_buf = (char *)  ll_outframe_node->value;

            //Don't worry about freeing the char_buf, the following function does that
            send_msg_to_receivers(char_buf);

            //Free up the ll_outframe_node
            free(ll_outframe_node);

            ll_outgoing_frame_length = ll_get_length(outgoing_frames_head);
        }
    }
    pthread_exit(NULL);
    return 0;
}

 
