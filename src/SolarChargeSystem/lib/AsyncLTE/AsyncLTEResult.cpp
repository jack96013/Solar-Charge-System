
#include "AsyncLTEResult.h"

AsyncLTEResultBase::AsyncLTEResultBase() 
{

}

void AsyncLTEResultBase::start()
{
    status = WAIT;
}

void AsyncLTEResultBase::setOnReceived(Handler handler, void *arg)
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
    //status = RECEIVED;
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

void AsyncLTEResultBase::returnResult(int16_t val) 
{
    data.Int = val;
    returnSuccessful();
}

void AsyncLTEResultBase::returnResult(int32_t val) 
{
    data.Long = val;
}

void AsyncLTEResultBase::returnResult(float val) 
{
    data.Float = val;
}

void AsyncLTEResultBase::returnResult(bool val) 
{
    data.Bool = val;
}   

int16_t AsyncLTEResultBase::getInt()
{
    return data.Int;
}
int32_t AsyncLTEResultBase::getLong()
{
    return data.Long;
}
float AsyncLTEResultBase::getFloat()
{
    return data.Float;
}
bool AsyncLTEResultBase::getBool()
{
    return data.Bool;
}


void AsyncLTEResultBase::finish() 
{
    delete this;
}

void AsyncLTEResultBase::clear() 
{
    status = SUCCESSFUL;
    handler = nullptr;
    arg = nullptr;
    data.Int = 0;
}

//===

// template<typename TResult>
// TResult* AsyncLTEResult<TResult>::getResult() 
// {
//     return result;
// }

// template<typename TResult>
// void AsyncLTEResult<TResult>::returnResult(TResult* result) 
// {
//     this->returnSuccessful();
//     this->result = result;
// }

// template<typename TResult>
// void AsyncLTEResult<TResult>::finish() 
// {
//     delete result;
//     delete this;
// }