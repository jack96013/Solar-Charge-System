#include "SerialReceiver.h"

SerialReceiver::SerialReceiver(Stream &serial)
{
    this->serial = serial;
}

void SerialReceiver::begin(uint16_t bufferLength)
{
    receiveBufferLength = bufferLength;
    receiveBuffer.reserve(bufferLength + 1);
}

void SerialReceiver::run()
{
    serialCheck();
}



void SerialReceiver::serialCheck()
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
            onNewString();
        }

        receiveBuffer = "";
    }
}

void SerialReceiver::onNewString()
{
#ifdef SM_DEBUG
    serial.println(receiveBuffer);
#endif
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
