#include "util.h"

//Linked list functions
int ll_get_length(LLnode * head)
{
  LLnode * tmp;
  int count = 1;
  if (head == NULL)
    return 0;
  else
  {
    tmp = head->next;
    while (tmp != head)
    {
      count++;
      tmp = tmp->next;
    }
    return count;
  }
}

void ll_append_node(LLnode ** head_ptr, 
    void * value)
{
  LLnode * prev_last_node;
  LLnode * new_node;
  LLnode * head;

  if (head_ptr == NULL)
  {
    return;
  }

  //Init the value pntr
  head = (*head_ptr);
  new_node = (LLnode *) malloc(sizeof(LLnode));
  new_node->value = value;

  //The list is empty, no node is currently present
  if (head == NULL)
  {
    (*head_ptr) = new_node;
    new_node->prev = new_node;
    new_node->next = new_node;
  }
  else
  {
    //Node exists by itself
    prev_last_node = head->prev;
    head->prev = new_node;
    prev_last_node->next = new_node;
    new_node->next = head;
    new_node->prev = prev_last_node;
  }
}


LLnode * ll_pop_node(LLnode ** head_ptr)
{
  LLnode * last_node;
  LLnode * new_head;
  LLnode * prev_head;

  prev_head = (*head_ptr);
  if (prev_head == NULL)
  {
    return NULL;
  }
  last_node = prev_head->prev;
  new_head = prev_head->next;

  //We are about to set the head ptr to nothing because there is only one thing in list
  if (last_node == prev_head)
  {
    (*head_ptr) = NULL;
    prev_head->next = NULL;
    prev_head->prev = NULL;
    return prev_head;
  }
  else
  {
    (*head_ptr) = new_head;
    last_node->next = new_head;
    new_head->prev = last_node;

    prev_head->next = NULL;
    prev_head->prev = NULL;
    return prev_head;
  }
}

void ll_destroy_node(LLnode * node)
{
  if (node->type == llt_string)
  {
    free((char *) node->value);
  }
  free(node);
}

//Compute the difference in usec for two timeval objects
long timeval_usecdiff(struct timeval *start_time, 
    struct timeval *finish_time)
{
  long usec;
  usec=(finish_time->tv_sec - start_time->tv_sec)*1000000;
  usec+=(finish_time->tv_usec- start_time->tv_usec);
  return usec;
}


//Print out messages entered by the user
void print_cmd(Cmd * cmd)
{
  fprintf(stderr, "src=%d, dst=%d, message=%s\n", 
      cmd->src_id,
      cmd->dst_id,
      cmd->message);
}


char * convert_frame_to_char(Frame * frame)
{
  //TODO: You should implement this as necessary
  char * char_buffer = (char *) malloc(MAX_FRAME_SIZE);
  memset(char_buffer,
      0,
      MAX_FRAME_SIZE);
  int position = 0;
  memcpy(char_buffer,
      &(frame->src_id),
      SRCID_SIZE);
  position = position + SRCID_SIZE;
  memcpy(&char_buffer[position],
      &(frame->dst_id),
      DSTID_SIZE);
  position += DSTID_SIZE;
  memcpy(&char_buffer[position],
      &(frame->sameMsg),
      BOOL_SIZE);
  position += BOOL_SIZE;
  memcpy(&char_buffer[position],
      &(frame->seqNum),
      SEQ_SIZE);
  position += SEQ_SIZE;
  memcpy(&char_buffer[position], 
      frame->data,
      FRAME_PAYLOAD_SIZE);
  position += FRAME_PAYLOAD_SIZE;
  memcpy(&char_buffer[position],
      &(frame->crc),
      CRC_SIZE);
  position += CRC_SIZE;
  /**
    memcpy(char_buffer,
    &(frame->beginSeq),
    SEQ_SIZE);
    position = position + SEQ_SIZE;
    memcpy(&char_buffer[position],
    &(frame->address_field),
    ADDRESS_FIELD_SIZE);
    position += ADDRESS_FIELD_SIZE;
    memcpy(&char_buffer[position],
    &(frame->control_field),
    CONTROL_FIELD_SIZE);
    position += CONTROL_FIELD_SIZE;
    memcpy(&char_buffer[position], 
    frame->data,
    FRAME_PAYLOAD_SIZE);
    position += FRAME_PAYLOAD_SIZE;
    memcpy(&char_buffer[position],
    frame->crc,
    CRC_SIZE);
    position += CRC_SIZE;
    memcpy(&char_buffer[position],
    &(frame->endSeq),
    SEQ_SIZE);
    position += SEQ_SIZE;
   */
  return char_buffer;
}


Frame * convert_char_to_frame(char * char_buf)
{
  //TODO: You should implement this as necessary
  Frame * frame = (Frame *) malloc(sizeof(Frame));
  memset(&(frame->src_id),
      0,
      sizeof(char));
  memset(&(frame->dst_id),
      0,
      sizeof(char));
  memset(&(frame->sameMsg),
      0,
      sizeof(char));
  memset(&(frame->seqNum),
      0,
      sizeof(char));
  memset(frame->data,
      0,
      sizeof(char)*sizeof(frame->data));
  memset(&(frame->crc),
      0,
      sizeof(char));
  int position = 0;
  memcpy(&(frame->src_id),
      char_buf,
      sizeof(char));
  position = position + sizeof(char);
  memcpy(&(frame->dst_id),
      &char_buf[position],
      sizeof(char));
  position = position + sizeof(char);
  memcpy(&(frame->sameMsg),
      &char_buf[position],
      sizeof(char));
  position = position + sizeof(char);
  memcpy(&(frame->seqNum),
      &char_buf[position],
      sizeof(char));
  position = position + sizeof(char);
  memcpy(frame->data, 
      &char_buf[position],
      sizeof(char)*sizeof(frame->data));
  position = position + sizeof(char)*sizeof(frame->data);
  memcpy(&(frame->crc),
      &char_buf[position],
      sizeof(char));
  position = position + sizeof(char);
  /**
    memset(&(frame->beginSeq),
    0,
    sizeof(char));
    memset(&(frame->endSeq),
    0,
    sizeof(char));
    memset(&(frame->address_field),
    0,
    sizeof(char));
    memset(&(frame->control_field),
    0,
    sizeof(char));
    memset(frame->crc,
    0,
    sizeof(char)*sizeof(frame->crc));
    memset(frame->data,
    0,
    sizeof(char)*sizeof(frame->data));
    int position = 0;
    memcpy(&(frame->beginSeq),
    char_buf,
    sizeof(char));
    position = position + sizeof(char);
    memcpy(&(frame->address_field),
    &char_buf[position],
    sizeof(char));
    position = position + sizeof(char);
    memcpy(&(frame->control_field),
    &char_buf[position],
    sizeof(char));
    position = position + sizeof(char);
    memcpy(frame->data, 
    &char_buf[position],
    sizeof(char)*sizeof(frame->data));
    position = position + sizeof(char)*sizeof(frame->data);
    memcpy(frame->crc,
    &char_buf[position],
    sizeof(char)*sizeof(frame->crc));
    position = position + sizeof(char)*sizeof(frame->crc);
    memcpy(&(frame->endSeq),
    &char_buf[position],
    sizeof(char));
    position = position + sizeof(char);
   */
  return frame;
}


