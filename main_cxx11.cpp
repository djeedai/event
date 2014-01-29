#include <iostream>
#include <typeinfo>
#include <vector>
#include <algorithm>

class Object
{
public:
};

class EventListener
{
public:
};

class A : public EventListener
{
public:
	A(const char* name) : name_(name) {}
	void callback(Object* sender, int arg1, int arg2);
protected:
	const char* name_;
};


void A::callback(Object* sender, int arg1, int arg2)
{
	std::cout << "Callback from " << sender << " in " << name_ << "! arg1=" << arg1 << " arg2=" << arg2 << "\n";
}

template <typename... Args> struct EventHandler
{
	typedef void (EventListener::*BaseCallback)(Object*, Args...);
	EventListener* listener_;
	BaseCallback   callback_;
	template <typename T> EventHandler(T& listener, void (T::* callback)(Object*, Args...))
	: listener_(&listener), callback_(static_cast<BaseCallback>(callback))
	{
	}
	bool operator==(const EventHandler& other) const
	{
		return (listener_ == other.listener_) && (callback_ == other.callback_);
	}
};

template <typename... Args> class Event
{
public:
	Event& operator+=(EventHandler<Args...> handler)
	{
		handlers_.push_back(handler);
	}
	Event& operator-=(EventHandler<Args...> handler)
	{
		handlers_.erase(std::remove(handlers_.begin(), handlers_.end(), handler), handlers_.end());
	}
	void operator()(Object* sender, Args... args)
	{
		for ( EventHandler<Args...>& handler : handlers_ )
			(handler.listener_->*handler.callback_)(sender, args...);
	}
protected:
	std::vector<EventHandler<Args...>> handlers_;
};

typedef Event<int, int> MyCustomEvent;

class MyCustomClass : public Object
{
public:
	MyCustomEvent onDoSomethingEvent;
	void doSomething()
	{
		std::cout << "MyCustomClass::doSomething()\n";
		onDoSomethingEvent(this, 2, 4);
	}
};

template <typename T, typename... Args>
inline EventHandler<Args...> make_handler(T& object, void (T::*callback)(Object*, Args...))
{
	return EventHandler<Args...>(object, callback);
}

int main(int argc, char **argv)
{
	std::cout << "Hello, world!" << std::endl;
	A a("a"), a2("a2");
	MyCustomClass c;
	
	c.onDoSomethingEvent += make_handler(a, &A::callback);
	std::cout << "Fire1:\n";
	c.doSomething();
	
	c.onDoSomethingEvent += make_handler(a2, &A::callback);
	std::cout << "Fire2:\n";
	c.doSomething();
	
	c.onDoSomethingEvent -= make_handler(a, &A::callback);
	std::cout << "Fire3:\n";
	c.doSomething();
	
	std::cout << "Fire4:\n";
	c.onDoSomethingEvent(NULL, 99, -42);
	
	return 0;
}
