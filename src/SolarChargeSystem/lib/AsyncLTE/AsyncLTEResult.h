/*
 * @ FileName		:
 * @ Author			: TZU-CHIEH,HSU
 * @ Modified by	: TZU-CHIEH,HSU
 * @ Create Time	: 2021-12-05 18:35:26
 * @ Modified time	: 2021-12-06 20:30:40
 * @ Description	:
 */

#ifndef __ASYNCLTERESULT_H__
#define __ASYNCLTERESULT_H__

#include <Arduino.h>

class AsyncLTEResultBase
{
public:
    typedef void (*Handler)(void *result, String *payload, void *arg);
    AsyncLTEResultBase(Handler handler, void *arg);
    bool isReceived();
    bool isCompleted();
    bool isFail();
    void *getResult();

    void onReceivedInvoke(String *string); // 當接收到訊息時，透過此委派
    void returnSuccessful();
    void returnFail();
    void returnResult(void *result);

    void getTimeout();
    void setTimeout();

    void finish();

private:
    enum
    {
        RECEIVED,
        SUCCESSFUL,
        FAIL
    };
    uint8_t status;
    Handler handler;
    void *arg;
    void *result;
};

template <typename TResult>
class AsyncLTEResult : public AsyncLTEResultBase
{
public:
    //typedef void (*Handler)(AsyncLTEResult<TResult>* result,String *payload,void* arg);
    AsyncLTEResult(AsyncLTEResultBase::Handler handler, void *arg) : AsyncLTEResultBase(handler, arg){};

    TResult* getResult();
    void returnResult(TResult* result);
    void finish();

private:

};

#endif // __ASYNCLTERESULT_H__ * @ Modified time	: 2021-12-05 18:36:15