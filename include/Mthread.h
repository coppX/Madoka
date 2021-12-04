#ifndef __MTHREAD__
#define __MTHREAD__

#include <tuple>
#include <memory>
#include "Types.h"

#if defined (__clang__) || (__GUNC__)
#include <pthread.h>
#elif defined (_MSC_VER)
#include <process.h>
#endif

//remove const volatile and reference attribute
template<typename T>
struct uncvref{
	typedef remove_cv_t<remove_reference_t<T>> type;
};

template<typename T>
using uncvref_t = typename uncvref<T>::type;

template<size_t...> struct tuple_indices {};

template<size_t Sp, class IntTuple, size_t Ep> struct make_indices_imp;

template<size_t Sp, size_t... Indices, size_t Ep>
struct make_indices_imp<Sp, tuple_indices<Indices...>, Ep>
{ 
	typedef typename make_indices_imp<Sp + 1, tuple_indices<Indices..., Sp>, Ep>::type type; 
};

//recursion end Sp=Ep
template<size_t Ep, size_t... Indices>
struct make_indices_imp<Ep, tuple_indices<Indices...>, Ep>
{ 
	typedef tuple_indices<Indices...> type; 
};

//indices
template<size_t Ep, size_t Sp = 0>
struct make_tuple_indices {
    typedef typename make_indices_imp<Sp, tuple_indices<>, Ep>::type type;
};

template<typename F, typename... Args>
class MThread
{
public:
	//delete copy constructor/operator
  MThread(const MThread&) = delete;
  MThread& operator=(const MThread&) = delete;
	
	//typedef
  typedef std::tuple<decay_t<F>, decay_t<Args>...> Tp;
	typedef thread_t native_handle_type; 

	//constructor
	MThread();
	//#ifndef _LIBCPP_CXX03_LANG
	template<typename F, typename... Args, 
					enable_if_t<!is_same_v<uncvref_t<F>, MThread>>>
	MThread(F&& f, Args&&... args)
	{
		std::unique_ptr<Tp> tp(forward<F>(f), forward<Args>(args)...);

		int ec = -1;
		#if defined (__clang__) || defined (__GUNC__)
			ec = pthread_create(&ThreadId, &Invoke, tp.get());
		#elif defined (_MSC_VER)
			ec = _beginthreadex(nullptr, 0, &Invoke, tp.get(), 0, &ThreadId);
		#endif
		if(ec == 0)
		{
			p.release();
		}
		else
		{
			printf("thread constructor failed");
			std::abort();
		}
	}
	//#endif

	~Mthread();
	void operator=(MThread&& t);

	//observers
	bool joinable() const noexcept;
	ThreadId get_id() const noexcept;
	native_handle_type native_handle();
	static unsigned int hardware_concurrency() noexcept;

	//operations
	void join();
	void detach();
	void swap(MThread& t) noexcept;

private:
	ThreadId threadId_;
	native_handle_type type_;

	template<typename Tuple>
	inline int Invoke(void* rawVals) noexcept
	{
		std::unique_ptr<Tuple> tp(static_cast<Tuple>(rawVals));
		typedef typename make_tuple_indices<tuple_size_v<Tp>, Tuple> IndexType;
		thread_execute(tp, IndexType());
		return 0;
	}

	template<typename Tuple, size_t... Indices>
	inline void thread_execute(Tuple tp, tuple_indices<Indices...>)
	{
		invoke(std::move(std::get<0>(tp)), std::move(std::get<Indices)(tp))...);
	}
	
};

#endif