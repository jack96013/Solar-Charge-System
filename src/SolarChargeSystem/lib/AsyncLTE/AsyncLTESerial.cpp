/*
 * @ FileName		:
 * @ Author			: TZU-CHIEH,HSU
 * @ Modified by	: TZU-CHIEH,HSU
 * @ Create Time	: 2021-12-10 00:04:59
 * @ Modified time	: 2021-12-11 14:39:51
 * @ Description	:
 */

#include "AsyncLTESerial.h"
#include "AsyncLTE.h"

void AsyncLTESerial::serialCheck()
{
    bool receiveFinishFlag = false;
    AsyncLTEReponseType status;

    while (serial->available())
    {

        // 一個字節一個字節地讀,下一句是讀到的放入字符串數組中組成一個完成的數據包
        char incomingByte = serial->read();

        // 緩衝區已滿
        if (receiveBuffer.length() >= receiveBufferLength)
        {
            clearBuffer();
        }
        
        receiveBuffer += (char)incomingByte; // 將字元一個個串接

        if (incomingByte == SM_ENDMARK)
        {
            lineCount++;
            
            if (lineCount==1)
            {
                receiveBuffer = "";
                continue;
            }
            
            //Serial.print("[deb] ");
            //Serial.println(receiveBuffer);

            // OK\r\n
            size_t current_length = receiveBuffer.length();
            //lastLineIndex = receiveBuffer.length()-1;
            
            int startIndex = 0; //firstIndex
            size_t endIndex = current_length - 2;               // /r/n = 2
            
    
            // int firstNewLineIndex = receiveBuffer.indexOf(F("\r\n\r\n"));
            // Serial.print("[fl] ");
            // Serial.println(firstNewLineIndex);

            // if (firstNewLineIndex !=-1)
            // {
            //     startIndex = firstNewLineIndex + 4;
            // }
                
            startIndex = lastLineIndex;
            lastLineIndex = current_length;
            String ending = receiveBuffer.substring(startIndex, endIndex);
            // Serial.print("[lineCount] ");
            // Serial.println(lineCount);
            
            // Serial.print("[endsi] ");
            // Serial.println(startIndex);
            // Serial.print("[endei] ");
            // Serial.println(endIndex);
            // Serial.print("[end] ");
            // Serial.println(ending);            

            bool isEnding = true;

            if (ending.equals(ASYNCLTE_OK))
            {
                status = AsyncLTEReponseType::OK;
                receiveFinishFlag = true;
                lineCount = 0;
                lastLineIndex = 0;
            }
            else if (ending.equals(ASYNCLTE_ERROR))
            {
                status = AsyncLTEReponseType::ERROR;
                receiveFinishFlag = true;
                lineCount = 0;
                lastLineIndex = 0;
            }
            else
            {
                isEnding = false;
            }
            
            if (isEnding)
            {
                Serial.print("[isEnd] ");
                receiveBuffer = receiveBuffer.substring(0, startIndex - 4);
            }
        }
    }
    if (receiveFinishFlag)
    {
        if (receiveBuffer.length() > 0)
        {
            // remove /r /n
            //removeLineEnding(receiveBuffer);
#ifdef SM_DEBUG
            serial->println(receiveBuffer);
#endif
            onReceiveCallbackInvoke(status);
        }
        receiveBuffer = "";
    }
}



void AsyncLTESerial::onReceiveCallbackInvoke(AsyncLTEReponseType status) 
{
    if (onReceiveCallback != nullptr)
    {
        onReceiveCallback(onReceiveCallbackArg,receiveBuffer,status);
        
    }
        
}

void AsyncLTESerial::setOnReceiveCallback(OnReceiveCallback callback, void *arg) 
{
    this->onReceiveCallback = callback;
    this->onReceiveCallbackArg = arg;
}

void AsyncLTESerial::clearBuffer()
{
    receiveBuffer = "";
    serial->flush();
    lineCount = 0;
    lastLineIndex =0;
}