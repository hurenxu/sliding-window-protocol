#include "receiver.h"
/**
TODO: if super big data, fragament, then need to figure out how to combine them into one message
*/

/**
TODO: CHeck the retransnit file
*/

void init_receiver(Receiver * receiver,
                   int id)
{
    receiver->recv_id = id;
    receiver->input_framelist_head = NULL;
    receiver->NFE = 0;
    receiver->LFR = -1;
    receiver->LAF = receiver->NFE + RWS - 1;
    int i;
    for (i = 0; i < BUFFER_SIZE; i++) 
    {
      receiver->recvQ[i].received = 0;
    }
}


void handle_incoming_msgs(Receiver * receiver,
                          LLnode ** outgoing_frames_head_ptr)
{
    //TODO: Suggested steps for handling incoming frames
    //    1) Dequeue the Frame from the sender->input_framelist_head *
    //    2) Convert the char * buffer to a Frame data type *
    //    3) Check whether the frame is corrupted *
    //    4) Check whether the frame is for this receiver *
    //    5) Do sliding window protocol for sender/receiver pair
    int errorDetected = 0;
    int incoming_msgs_length = ll_get_length(receiver->input_framelist_head);
    while (incoming_msgs_length > 0)
    {
        //Pop a node off the front of the link list and update the count
        LLnode * ll_inmsg_node = ll_pop_node(&receiver->input_framelist_head);
        incoming_msgs_length = ll_get_length(receiver->input_framelist_head);

        //DUMMY CODE: Print the raw_char_buf
        //NOTE: You should not blindly print messages!
        //      Ask yourself: Is this message really for me? *
        //                    Is this message corrupted? *
        //                    Is this an old, retransmitted message?           
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


        // check whether the frame is for this receiver
        if(receiver->recv_id == inframe->dst_id) 
        {
           uint8_t seqNo = inframe->seqNum;

           // TODO: When the seqNo is not the NFE
           // case 1: usual case 
           if((receiver->LAF >= receiver->NFE) && (seqNo >= receiver->NFE) && (seqNo <= receiver->LAF)) 
           {
             printf("<RECV_%d>:[%s]\n", receiver->recv_id, inframe->data);
             if(seqNo != receiver->NFE) 
             {
               errorDetected = 1;
             }
             if(errorDetected == 0) 
             {
               receiver->NFE = seqNo + 1;
               receiver->LFR = receiver->NFE - 1;
               receiver->LAF = receiver->NFE + RWS - 1;
             
               // store the frame in the receive window and set the recived to 1
               receiver->recvQ[receiver->LFR % BUFFER_SIZE].frame = inframe;
               receiver->recvQ[receiver->LFR % BUFFER_SIZE].received = 1;
             }
             else if(errorDetected == 1) 
             {
               // store the frame in the receive window and set the recived to 1
               receiver->recvQ[inframe->seqNum % BUFFER_SIZE].frame = inframe;
               receiver->recvQ[inframe->seqNum % BUFFER_SIZE].received = 1;
             }
             incoming_msgs_length = ll_get_length(receiver->input_framelist_head);
             if(incoming_msgs_length == 0) 
             {            
               // send back the ack since the sequence is in the window
               Frame * outgoing_frame = (Frame *) malloc(sizeof(Frame));
               outgoing_frame->src_id = receiver->recv_id;
               outgoing_frame->dst_id = inframe->src_id;
               outgoing_frame->ackNum = receiver->LFR; 
               outgoing_frame->crc = 0x00;
               char * outgoing_charbuf = convert_frame_to_char(outgoing_frame);
               //int array_len = sizeof(outgoing_charbuf) / sizeof(outgoing_charbuf[0]);
               int array_len = MAX_FRAME_SIZE;
               append_crc(outgoing_charbuf, array_len);
               ll_append_node(outgoing_frames_head_ptr, outgoing_charbuf);
             }
           /**
           // TODO: When the seqNo is not the NFE
           // case 1: usual case 
           if((receiver->LAF >= receiver->NFE) && (seqNo >= receiver->NFE) && (seqNo <= receiver->LAF)) 
           {
             // send back the ack since the sequence is in the window
             Frame * outgoing_frame = (Frame *) malloc(sizeof(Frame));
             outgoing_frame->src_id = receiver->recv_id;
             outgoing_frame->dst_id = inframe->src_id;
             if(seqNo != receiver->NFE) 
             {
               outgoing_frame->ackNum = inframe->LFR; 
               receiver->NFE = receiver->NFE;
               receiver->LFR = receiver->LFR;
               receiver->LAF = receiver->LAF;
             }
             else 
             {
               outgoing_frame->ackNum = inframe->seqNum; 
               receiver->LAF += 1; 
               receiver->NFE += 1; 
               receiver->LFR += 1; 
             }
             outgoing_frame->seqNum = inframe->seqNum; 
             outgoing_frame->crc = 0x00;
             //strcpy(outgoing_frame->data, inframe->data);
             char * outgoing_charbuf = convert_frame_to_char(outgoing_frame);
             int array_len = sizeof(outgoing_charbuf) / sizeof(outgoing_charbuf[0]);
             append_crc(outgoing_charbuf, array_len);
             outgoing_frame = convert_char_to_frame(outgoing_charbuf);
             ll_append_node(outgoing_frames_head_ptr, outgoing_charbuf);
             // TODO: Free or not?





             // store the frame in the receive window and set the recived to 1
             receiver->recvQ[outgoing_frame->ackNum % BUFFER_SIZE].frame = outgoing_frame;
             receiver->recvQ[outgoing_frame->ackNum % BUFFER_SIZE].received = 1;
             // update the number for the window (slding window)
             */
           }
           //case 2: sequence number wrap around
           else if((receiver->LAF < receiver->NFE) && ((seqNo >= receiver->NFE) || (seqNo <= receiver->LAF))) 
           {
             printf("<RECV_%d>:[%s]\n", receiver->recv_id, inframe->data);
             if(seqNo != receiver->NFE) 
             {
               errorDetected = 1;
             }
             if(errorDetected == 0) 
             {
               receiver->NFE = seqNo + 1;
               receiver->LFR = receiver->NFE - 1;
               receiver->LAF = receiver->NFE + RWS - 1;
             
               // store the frame in the receive window and set the recived to 1
               receiver->recvQ[receiver->LFR % BUFFER_SIZE].frame = inframe;
               receiver->recvQ[receiver->LFR % BUFFER_SIZE].received = 1;
             }
             else if(errorDetected == 1) 
             {
               // store the frame in the receive window and set the recived to 1
               receiver->recvQ[inframe->seqNum % BUFFER_SIZE].frame = inframe;
               receiver->recvQ[inframe->seqNum % BUFFER_SIZE].received = 1;
             }
             incoming_msgs_length = ll_get_length(receiver->input_framelist_head);
             if(incoming_msgs_length == 0) 
             {            
               // send back the ack since the sequence is in the window
               Frame * outgoing_frame = (Frame *) malloc(sizeof(Frame));
               outgoing_frame->src_id = receiver->recv_id;
               outgoing_frame->dst_id = inframe->src_id;
               outgoing_frame->ackNum = receiver->LFR; 
               outgoing_frame->crc = 0x00;
               char * outgoing_charbuf = convert_frame_to_char(outgoing_frame);
               //int array_len = sizeof(outgoing_charbuf) / sizeof(outgoing_charbuf[0]);
               int array_len = MAX_FRAME_SIZE;
               append_crc(outgoing_charbuf, array_len);
               ll_append_node(outgoing_frames_head_ptr, outgoing_charbuf);
             }
             // send back the ack since the sequence is in the window
             /**
             Frame * outgoing_frame = (Frame *) malloc(sizeof(Frame));
             outgoing_frame->src_id = receiver->recv_id;
             outgoing_frame->dst_id = inframe->src_id;
             outgoing_frame->ackNum = inframe->seqNum; 
             outgoing_frame->type = 1; 
             strcpy(outgoing_frame->data, inframe->data);
             char * outgoing_charbuf = convert_frame_to_char(outgoing_frame);
             int array_len = sizeof(outgoing_charbuf) / sizeof(outgoing_charbuf[0]);
             append_crc(outgoing_charbuf, array_len);
             outgoing_frame = convert_char_to_frame(outgoing_charbuf);
             ll_append_node(outgoing_frames_head_ptr, outgoing_charbuf);
             // TODO: Free or not?
             // store the frame in the receive window and set the recived to 1
             receiver->recvQ[outgoing_frame->ackNum % BUFFER_SIZE].frame = outgoing_frame;
             receiver->recvQ[outgoing_frame->ackNum % BUFFER_SIZE].received = 1;
             // update the number for the window (slding window)
             receiver->LAF += 1; 
             receiver->NFE += 1; 
             receiver->LFR += 1; */
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


    //This incomplete receiver thread, at a high level, loops as follows:
    //1. Determine the next time the thread should wake up if there is nothing in the incoming queue(s)
    //2. Grab the mutex protecting the input_msg queue
    //3. Dequeues messages from the input_msg queue and prints them
    //4. Releases the lock
    //5. Sends out any outgoing messages

    pthread_cond_init(&receiver->buffer_cv, NULL);
    pthread_mutex_init(&receiver->buffer_mutex, NULL);

    while(1)
    {    
        //NOTE: Add outgoing messages to the outgoing_frames_head pointer
        outgoing_frames_head = NULL;
        gettimeofday(&curr_timeval, 
                     NULL);

        //Either timeout or get woken up because you've received a datagram
        //NOTE: You don't really need to do anything here, but it might be useful for debugging purposes to have the receivers periodically wakeup and print info
        time_spec.tv_sec  = curr_timeval.tv_sec;
        time_spec.tv_nsec = curr_timeval.tv_usec * 1000;
        time_spec.tv_sec += WAIT_SEC_TIME;
        time_spec.tv_nsec += WAIT_USEC_TIME * 1000;
        if (time_spec.tv_nsec >= 1000000000)
        {
            time_spec.tv_sec++;
            time_spec.tv_nsec -= 1000000000;
        }

        //*****************************************************************************************
        //NOTE: Anything that involves dequeing from the input frames should go 
        //      between the mutex lock and unlock, because other threads CAN/WILL access these structures
        //*****************************************************************************************
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
