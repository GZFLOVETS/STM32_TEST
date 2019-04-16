#include "Messages.h"
static QueueType queue={0};
/**
  * @Features   ��Ϣ����
  *         
  * @parameter  messages����Ϣ����
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
  * @Features   �����Ϣ
  *         
  * @parameter  None
  * @return     ��Ϣ����
  * @Remarks    ��������������еİ�ȫ������ʹ��֮ǰһ��Ҫȷ�϶��в�Ϊ��
  */
messages GetMessage(void)
{
	if(queue.MessageHead+1>=QUEUELEN){
		queue.MessageHead = 0;
	}
	return queue.Messages[queue.MessageHead++];
}

/**
  * @Features   �ж϶����Ƿ�ջ����
  *         
  * @parameter  None
  * @return     0��������1���ջ����
  * @Remarks    �������ͷ�����β��ȣ�
  * 		    ���п����Ƕ���û����Ϣ��
  *		    Ҳ��������Ϣ���ർ��������������ʹ�ý�������Ϣ��ʧ
  */
u8 QueueEmptyOrError(void)
{
	return (queue.MessageHead == queue.MessageTail)?1:0;
}











































