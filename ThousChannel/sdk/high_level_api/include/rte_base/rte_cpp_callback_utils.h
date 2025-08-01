#pragma once
#include <functional>
#include "rte_cpp_error.h"
#include "rte_base/c/handle.h"

/**
 * @technical preview
 */
namespace rte {


template<typename T, typename... Args>
class SingleUseCallbackWithArgs {

 public:
  using CallbackType = std::function<void(T* self, Args... args, void* cb_data, RteError* err)>;

  SingleUseCallbackWithArgs(){
    cb_ = nullptr;
    cb_data_ = nullptr;
    self_ = nullptr;
  };

  SingleUseCallbackWithArgs(SingleUseCallbackWithArgs<T, Args...>& other){
    cb_ = other.cb_;
    cb_data_ = other.cb_data_;
    self_ = other.self_;

    other.Clear();
  }


  SingleUseCallbackWithArgs(SingleUseCallbackWithArgs<T, Args...>&& other){
    cb_ = other.cb_;
    cb_data_ = other.cb_data_;
    self_ = other.self_;

    other.Clear();
  }

  SingleUseCallbackWithArgs<T, Args...> &operator=(SingleUseCallbackWithArgs<T, Args...>&& other){
    cb_ = other.cb_;
    cb_data_ = other.cb_data_;
    self_ = other.self_;

    other.Clear();

    return *this;
  }

  void Store(T* self, CallbackType cb,  void* cb_data){
    self_ = self;
    cb_ = cb;
    cb_data_ = cb_data;
  }

  void Invoke(Args... args, RteError* err){
    if(cb_ != nullptr){
      cb_(self_, args..., cb_data_, err);

      self_ = nullptr;
      cb_ = nullptr;
      cb_data_ = nullptr;
    }
  }

  bool Invalid(){
    return cb_ == nullptr;
  }

  void Clear(){
    self_ = nullptr;
    cb_ = nullptr;
    cb_data_ = nullptr;
  }

  CallbackType cb_;
  void* cb_data_;
  T* self_;
};  // class SingleUseCallback

template<typename T>
class SingleUseCallback {

 public:
  using CallbackType = std::function<void(T* self, void* cb_data, RteError* err)>;

  SingleUseCallback(){
    cb_ = nullptr;
    cb_data_ = nullptr;
    self_ = nullptr;
  };

  SingleUseCallback(SingleUseCallback<T>& other){
    cb_ = other.cb_;
    cb_data_ = other.cb_data_;
    self_ = other.self_;

    other.Clear();
  }


  SingleUseCallback(SingleUseCallback<T>&& other){
    cb_ = other.cb_;
    cb_data_ = other.cb_data_;
    self_ = other.self_;

    other.Clear();
  }

  SingleUseCallback<T> &operator=(SingleUseCallback<T>&& other){
    cb_ = other.cb_;
    cb_data_ = other.cb_data_;
    self_ = other.self_;

    other.Clear();

    return *this;
  }

  void Store(T* self, CallbackType cb,  void* cb_data){
    self_ = self;
    cb_ = cb;
    cb_data_ = cb_data;
  }

  void Invoke(RteError* err){
    if(cb_ != nullptr){
      cb_(self_, cb_data_, err);

      self_ = nullptr;
      cb_ = nullptr;
      cb_data_ = nullptr;
    }
  }

  bool Invalid(){
    return cb_ == nullptr;
  }

  void Clear(){
    self_ = nullptr;
    cb_ = nullptr;
    cb_data_ = nullptr;
  }

  CallbackType cb_ = nullptr;
  void* cb_data_ = nullptr;
  T* self_ = nullptr;
};  // class SingleUseCallback




template<typename T>
class CallbackContext {
  public:

  using CallbackTypeOnlyError = std::function<void(RteError* err)>;
  using CallbackTypeOnlyErrorWithCppError = std::function<void(rte::Error* err)>;

  using CallbackType = std::function<void(T* self, void* cb_data, RteError* err)>;
  using CallbackTypeWithCppError = std::function<void(T* self, void* cb_data, rte::Error* err)>;

  CallbackContext(T* self, CallbackTypeOnlyError cb)
      :self_(self), cb_only_error_(cb) {}

  CallbackContext(T* self, CallbackTypeOnlyErrorWithCppError cb)
      :self_(self), cb_only_error_with_cpp_error_(cb) {}

  CallbackContext(T* self, CallbackType cb,  void* cb_data)
      :self_(self), cb_(cb), cb_data_(cb_data) {}

  CallbackContext(T* self, CallbackTypeWithCppError cb,  void* cb_data)
      :self_(self), cb_with_cpp_error_(cb), cb_data_(cb_data) {}

  CallbackTypeOnlyError cb_only_error_ = nullptr;
  CallbackTypeOnlyErrorWithCppError cb_only_error_with_cpp_error_ = nullptr;

  CallbackType cb_ = nullptr;
  CallbackTypeWithCppError cb_with_cpp_error_ = nullptr;

  void* cb_data_ = nullptr;
  T* self_ = nullptr;
};

template<typename FromeType, typename ToType>
void CallbackFunc(FromeType* self, void* cb_data, RteError* err){
  auto *ctx = static_cast<CallbackContext<ToType>*>(cb_data);

  if(ctx->cb_only_error_ != nullptr){
    ctx->cb_only_error_(err);
  }

  if(ctx->cb_only_error_with_cpp_error_ != nullptr){
    rte::Error cpp_err(err);
    ctx->cb_only_error_with_cpp_error_(&cpp_err);
  }

  if(ctx->cb_with_cpp_error_ != nullptr){
    rte::Error cpp_err(err);
    ctx->cb_with_cpp_error_( self != nullptr ? ctx->self_ : nullptr, ctx->cb_data_, &cpp_err);
  }

  if(ctx->cb_ != nullptr){
    ctx->cb_(self != nullptr ? ctx->self_ : nullptr, ctx->cb_data_, err);
  }
  
  delete ctx;
}

template<typename T, typename...Args>
class CallbackContextWithArgs {
  public:

  using CallbackTypeOnlyError = std::function<void(RteError* err)>;
  using CallbackTypeOnlyErrorWithCppError = std::function<void(rte::Error* err)>; 

  using CallbackTypeWithArgs = std::function<void(Args... args, RteError* err)>;
  using CallbackTypeWithArgsAndWithCppError = std::function<void(Args... args, rte::Error* err)>;

  using CallbackType = std::function<void(T* self, Args... args, void* cb_data, RteError* err)>;
  using CallbackTypeWithCppError = std::function<void(T* self, Args... args, void* cb_data, rte::Error* err)>;

   CallbackContextWithArgs(T* self, CallbackTypeOnlyError cb, bool only_error)
       :self_(self), cb_only_error_(cb){}

   CallbackContextWithArgs(T* self, CallbackTypeOnlyErrorWithCppError cb, bool only_error)
       :self_(self), cb_only_error_with_cpp_error_(cb) {}

  CallbackContextWithArgs(T* self, CallbackTypeWithArgs cb)
      :self_(self), cb_with_args_(cb) {}

  CallbackContextWithArgs(T* self, CallbackTypeWithArgsAndWithCppError cb)
      :self_(self), cb_with_args_and_with_cpp_error_(cb) {}

  CallbackContextWithArgs(T* self, CallbackType cb,  void* cb_data)
      :self_(self), cb_(cb), cb_data_(cb_data) {}

  CallbackContextWithArgs(T* self, CallbackTypeWithCppError cb,  void* cb_data)
      :self_(self), cb_with_cpp_error_(cb), cb_data_(cb_data) {}


  CallbackTypeOnlyError cb_only_error_ = nullptr;
  CallbackTypeOnlyErrorWithCppError cb_only_error_with_cpp_error_ = nullptr;
    
  CallbackTypeWithArgs cb_with_args_ = nullptr;
  CallbackTypeWithArgsAndWithCppError cb_with_args_and_with_cpp_error_ = nullptr;

  CallbackType cb_ = nullptr;
  CallbackTypeWithCppError cb_with_cpp_error_ = nullptr;
  void* cb_data_ = nullptr;
  T* self_ = nullptr;
};

template<typename FromeType, typename ToType, typename...Args>
void CallbackFuncWithArgs(FromeType* self, Args... args, void* cb_data, RteError* err){
  auto *ctx = static_cast<CallbackContextWithArgs<ToType, Args...>*>(cb_data);

   if(ctx->cb_only_error_ != nullptr){
     ctx->cb_only_error_(err);
   }

   if(ctx->cb_only_error_with_cpp_error_ != nullptr){
     rte::Error cpp_err(err);
     ctx->cb_only_error_with_cpp_error_(&cpp_err);
   }

  if(ctx->cb_with_args_ != nullptr){
    ctx->cb_with_args_(args..., err);
  }

  if(ctx->cb_with_args_and_with_cpp_error_ != nullptr){
    rte::Error cpp_err(err);
    ctx->cb_with_args_and_with_cpp_error_(args..., &cpp_err);
  }

  if(ctx->cb_with_cpp_error_ != nullptr){
    rte::Error cpp_err(err);
    ctx->cb_with_cpp_error_( self != nullptr ? ctx->self_ : nullptr, args..., ctx->cb_data_, &cpp_err);
  }  

  if(ctx->cb_ != nullptr){
    ctx->cb_(self != nullptr ? ctx->self_ : nullptr, args..., ctx->cb_data_, err);
  }
  delete ctx;
}

template<typename Type>
class ObserverDestroyContext {
  public:

  using ObserverDestroyer = std::function<void(Type* observer, void* cb_data)>;

  ObserverDestroyContext(ObserverDestroyer destroyer, void* cb_data)
      :destroyer_(destroyer), cb_data_(cb_data) {}

  ObserverDestroyer destroyer_;
  void* cb_data_;
};

template<typename FromeType, typename ToType>
void ObserverDestroyProxy(FromeType* observer, void* cb_data){
  auto *ctx = static_cast<ObserverDestroyContext<ToType>*>(cb_data);
  if(ctx->destroyer_ != nullptr){
    ctx->destroyer_(static_cast<ToType*>(observer->base_observer.me_in_target_lang), ctx->cb_data_);
  }
  delete ctx;
}

}  // namespace rte
