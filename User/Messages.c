#include "Messages.h"
static QueueType queue={0};
/**
  * @Features   消息发布
  *         
  * @parameter  messages：消息类型
  * @return     None
  * @Remarks    None
  */
void PutMessage(messages m)
{
	queue.Messages[queue.MessageTail] = m;
	if(++queue.MessageTail>=QUEUELEN) 
		queue.MessageTail =0;
}


/**
  * @Features   获得消息
  *         
  * @parameter  None
  * @return     消息类型
  * @Remarks    本函数不检验队列的安全性所以使用之前一点要确认队列不为空
  */
messages GetMessage(void)
{
	if(queue.MessageHead+1>=QUEUELEN){
		queue.MessageHead = 0;
	}
	return queue.Messages[queue.MessageHead++];
}

/**
  * @Features   判断队列是否空或出错
  *         
  * @parameter  None
  * @return     0：正常；1：空或出错
  * @Remarks    如果队列头或队列尾相等，
  * 		    则有可能是队列没有消息，
  *		    也可能是消息过多导致溢出，如果继续使用将导致信息丢失
  */
u8 QueueEmptyOrError(void)
{
	return (queue.MessageHead == queue.MessageTail)?1:0;
}











































