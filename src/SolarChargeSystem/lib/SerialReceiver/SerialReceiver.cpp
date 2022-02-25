#include "SerialReceiver.h"

SerialReceiver::SerialReceiver()
{
    
}

void SerialReceiver::begin(Stream& serial,uint16_t bufferLength)
{
    this->serial = &serial;
    receiveBufferLength = bufferLength;
    receiveBuffer.reserve(bufferLength + 1);
    //serial.println("Ready !");
    
}

void SerialReceiver::run()
{
    serialCheck();
}

void SerialReceiver::serialCheck()
{
    bool receiveFinishFlag = false;
    
    while (serial->available())
    {
        
        // 一個字節一個字節地讀,下一句是讀到的放入字符串數組中組成一個完成的數據包
        char incomingByte = serial->read();
        
        // 緩衝區已滿
        if (receiveBuffer.length() >= receiveBufferLength)
        {
            receiveBuffer = "";
        }
        receiveBuffer += (char)incomingByte; // 將字元一個個串接
        if (incomingByte == SM_ENDMARK)
        {
            receiveFinishFlag = true;
            break;
        }
    }
    if (receiveFinishFlag)
    {
        if (receiveBuffer.length() > 0)
        {
            // remove /r /n
            removeLineEnding(receiveBuffer);
            #ifdef SM_DEBUG
            serial->println(receiveBuffer);
            #endif
            onReceiveCallbackInvoke();
        }
        receiveBuffer = "";
    }
}



// Remove Line Ending
void SerialReceiver::removeLineEnding(String &str)
{
    // Remove \n \r
    if (str.endsWith("\n"))
        str.remove(str.length() - 1);
    ;
    if (str.endsWith("\r"))
        str.remove(str.length() - 1);
}

void SerialReceiver::onReceiveCallbackInvoke()
{
    if (onReceiveCallback != nullptr)
        onReceiveCallback(onReceiveCallbackArg,receiveBuffer);
}

void SerialReceiver::setOnReceiveCallback(OnReceiveCallback callback,void* arg)
{
    this->onReceiveCallback = callback;
    this->onReceiveCallbackArg = arg;
}

void SerialReceiver::clearBuffer()
{
    receiveBuffer = "";
    serial->flush();
}

String* SerialReceiver::getBuffer()
{
    return &receiveBuffer;
}