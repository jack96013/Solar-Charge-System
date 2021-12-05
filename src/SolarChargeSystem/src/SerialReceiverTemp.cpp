#include "SerialReceiverTemp.h"

template <class t> SerialReceiverT<t>::SerialReceiverT()
{
    
}

template <class t>
void SerialReceiverT<t>::begin(t& serial,uint16_t bufferLength)
{
    this->serial = serial;
    receiveBufferLength = bufferLength;
    receiveBuffer.reserve(bufferLength + 1);
    serial.println("Ready !");
    
    while (true)
    {
        serialCheck();
    }
}

template <class t>
void SerialReceiverT<t>::run()
{
    serialCheck();
}

template <class t>
void SerialReceiverT<t>::serialCheck()
{
    bool receiveFinishFlag = false;
    
    while (serial.available())
    {
        
        // 一個字節一個字節地讀,下一句是讀到的放入字符串數組中組成一個完成的數據包
        char incomingByte = serial.read();
        
        // 緩衝區已滿
        if (receiveBuffer.length() >= receiveBufferLength)
        {
            receiveBuffer = "";
        }
        receiveBuffer += (char)incomingByte; // 將字元一個個串接
        if (incomingByte == SM_ENDMARK)
            receiveFinishFlag = true;
    }
    if (receiveFinishFlag)
    {
        if (receiveBuffer.length() > 0)
        {
            // remove /r /n
            removeLineEnding(receiveBuffer);
            //onNewString();
        }
        receiveBuffer = "";
    }
}

template <class t>
void SerialReceiverT<t>::onNewString()
{
#ifdef SM_DEBUG
    Serial.println(receiveBuffer);
#endif
}

template <class t>
// Remove Line Ending
void SerialReceiverT<t>::removeLineEnding(String &str)
{
    // Remove \n \r
    if (str.endsWith("\n"))
        str.remove(str.length() - 1);
    ;
    if (str.endsWith("\r"))
        str.remove(str.length() - 1);
}
