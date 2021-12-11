/*
 * @ FileName		:
 * @ Author			: TZU-CHIEH,HSU
 * @ Modified by	: TZU-CHIEH,HSU
 * @ Create Time	: 2021-12-05 18:35:26
 * @ Modified time	: 2021-12-11 15:23:35
 * @ Description	:
 */

#ifndef __ASYNCLTERESULT_H__
#define __ASYNCLTERESULT_H__

#include <Arduino.h>

class AsyncLTEResultBase
{
public:
    typedef void (*Handler)(void *result, String *payload, void *arg);
    AsyncLTEResultBase();

    void start();

    void setOnReceived(Handler handler, void *arg);
    bool isReceived();
    bool isCompleted();
    bool isFail();
    void *getResult();

    void onReceivedInvoke(String *string); // 當接收到訊息時，透過此委派
    void returnSuccessful();
    void returnFail();
    void returnResult(void *result);
    void returnResult(int16_t val);
    void returnResult(int32_t val);
    void returnResult(float val);
    void returnResult(bool val);

    void getTimeout();
    void setTimeout();

    void finish();

    int16_t getInt();
    int32_t getLong();
    float getFloat();
    bool getBool();

    void clear();
    

    typedef union Data
    {
        int16_t Int;
        int32_t Long;
        float Float;     
        bool Bool;   
    };

private:
    enum
    {
        WAIT,
        RECEIVED,
        SUCCESSFUL,
        FAIL
    };
    uint8_t status;
    Handler handler;
    void *arg;
    void *result;

    Data data;
};

// template <typename TResult>
// class AsyncLTEResult : public AsyncLTEResultBase
// {
// public:
//     //typedef void (*Handler)(AsyncLTEResult<TResult>* result,String *payload,void* arg);
//     AsyncLTEResult(AsyncLTEResultBase::Handler handler, void *arg) : AsyncLTEResultBase(handler, arg){};

//     TResult* getResult();
//     void returnResult(TResult* result);
//     void finish();

// private:

// };

#endif // __ASYNCLTERESULT_H__ * @ Modified time	: 2021-12-05 18:36:15