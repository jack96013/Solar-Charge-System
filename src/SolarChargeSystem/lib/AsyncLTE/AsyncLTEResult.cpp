
#include "AsyncLTEResult.h"

AsyncLTEResultBase::AsyncLTEResultBase(Handler handler,void* arg) 
{
    this->handler = handler;
    this->arg = arg;
}

bool AsyncLTEResultBase::isReceived() 
{
    return status == RECEIVED;
}

bool AsyncLTEResultBase::isCompleted() 
{
    return status == SUCCESSFUL || status == FAIL;
}

bool AsyncLTEResultBase::isFail() 
{
    return status == FAIL;
}


void AsyncLTEResultBase::onReceivedInvoke(String* string) 
{
    status = RECEIVED;
    if (handler != nullptr)
        handler(this,string,arg);
}

void AsyncLTEResultBase::returnSuccessful() 
{
    status = SUCCESSFUL;
}

void AsyncLTEResultBase::returnFail() 
{
    status = FAIL;
}

void AsyncLTEResultBase::finish() 
{
    delete this;
}

//===

template<typename TResult>
TResult* AsyncLTEResult<TResult>::getResult() 
{
    return result;
}

template<typename TResult>
void AsyncLTEResult<TResult>::returnResult(TResult* result) 
{
    this->returnSuccessful();
    this->result = result;
}

template<typename TResult>
void AsyncLTEResult<TResult>::finish() 
{
    delete result;
    delete this;
}