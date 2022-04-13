/*
 * @ FileName		:
 * @ Author			: TZU-CHIEH,HSU
 * @ Modified by	: TZU-CHIEH,HSU
 * @ Create Time	: 2021-12-10 00:04:59
 * @ Modified time	: 2021-12-30 16:46:49
 * @ Description	:
 */

#include "AsyncLTESerial.h"
#include "AsyncLTE.h"

void AsyncLTESerial::serialCheck()
{
    if (!enable)
        return;
        
    bool receiveFinishFlag = false;
    timeoutTimer.run();
    //DEBUG_PRINTSTR(timeoutTimer.getRemainingTime());
    //DEBUG_PRINTSTR(timeoutTimer.getRemainingTime());
    if (timeoutTimer.isExpired())
    {
        
        DEBUG_PRINTSTR(F("Response Timeout!"));
        //timeoutTimer.stop();
        setEnable(false);
        state = AsyncLTESerialState::TIMEOUT;
        return;
    }

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
        if (incomingByte == '>' && receiveBuffer.length() == 1)
        {
            // Wait for Input
            state = AsyncLTESerialState::OK;
            receiveFinishFlag = true;
            lineCount = 0;
            lastLineIndex = 0;
        }

        if (incomingByte == SM_ENDMARK)
        {
            lineCount++;
            
            if (lineCount==1)
            {
                receiveBuffer = "";
                continue;
            }
            
            // Serial.print("[deb] ");
            // Serial.println(receiveBuffer);

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
                state = AsyncLTESerialState::OK;
                receiveFinishFlag = true;
                lineCount = 0;
                lastLineIndex = 0;
            }
            else if (ending.equals(ASYNCLTE_ERROR))
            {
                state = AsyncLTESerialState::ERROR;
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
                timeoutTimer.stop();
                //DEBUG_PRINTSTR("<- ");
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
            onReceiveCallbackInvoke();
        }
       // receiveBuffer = "";
    }
}

/*
void AsyncLTESerial::serialCheck()
{
    if (!enable)
        return;
        
    bool receiveFinishFlag = false;
    timeoutTimer.run();
    if (timeoutTimer.isExpired())
    {
        DEBUG_PRINTSTR(F("Response Timeout!"));
        timeoutTimer.stop();
        //setEnable(false);
        state = AsyncLTESerialState::TIMEOUT;
        return;
    }

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
                state = AsyncLTESerialState::OK;
                receiveFinishFlag = true;
                lineCount = 0;
                lastLineIndex = 0;
            }
            else if (ending.equals(ASYNCLTE_ERROR))
            {
                state = AsyncLTESerialState::ERROR;
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
                timeoutTimer.stop();
                //DEBUG_PRINTSTR("<- ");
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
            onReceiveCallbackInvoke();
        }
       // receiveBuffer = "";
    }
}
*/


void AsyncLTESerial::onReceiveCallbackInvoke() 
{
    if (onReceiveCallback != nullptr)
    {
        onReceiveCallback(onReceiveCallbackArg,receiveBuffer,state);
        
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
    enable = true;
}

void AsyncLTESerial::setEnable(bool en) 
{
    enable = en;
}

bool AsyncLTESerial::isReceived() 
{
    return state == AsyncLTESerialState::OK || 
            state == AsyncLTESerialState::ERROR;
    
}

bool AsyncLTESerial::isOK() 
{
    return state == AsyncLTESerialState::OK;
}

void AsyncLTESerial::startWaitForResult(uint32_t timeout) 
{
    clearBuffer();
    state = AsyncLTESerialState::WAITING;
    //timeoutTimer.setRepeats(1);
    //timeoutTimer.setInterval(timeout);
    timeoutTimer.clearSettings();
    timeoutTimer.delay(timeout);
    //timeoutTimer.stop();
    
    DEBUG_PRINTSTR(F("Start Wait For Response!"));
}

AsyncLTESerialState AsyncLTESerial::getState() 
{
    return state;
}