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

struct EventArgs {};

class A : public EventListener
{
public:
	A(const char* name) : name_(name) {}
	void callback(Object* sender, EventArgs& args);
protected:
	const char* name_;
};

struct MyCustomEventArgs : public EventArgs
{
	int arg1;
	int arg2;
	MyCustomEventArgs(int a1, int a2) : arg1(a1), arg2(a2) {}
};

void A::callback(Object* sender, EventArgs& args)
{
	MyCustomEventArgs& mceargs = static_cast<MyCustomEventArgs&>(args);
	std::cout << "Callback from " << sender << " in " << name_ << "! arg1=" << mceargs.arg1 << " arg2=" << mceargs.arg2 << "\n";
}

struct EventHandler
{
	typedef void (EventListener::*BaseCallback)(Object*, EventArgs&);
	EventListener* listener_;
	BaseCallback   callback_;
	template <typename T> EventHandler(T& listener, void (T::* callback)(Object*, EventArgs&))
	: listener_(&listener), callback_(static_cast<BaseCallback>(callback))
	{
	}
	bool operator==(const EventHandler& other) const
	{
		return (listener_ == other.listener_) && (callback_ == other.callback_);
	}
};

class Event
{
public:
	Event& operator+=(EventHandler handler)
	{
		handlers_.push_back(handler);
	}
	Event& operator-=(EventHandler handler)
	{
		handlers_.erase(std::remove(handlers_.begin(), handlers_.end(), handler), handlers_.end());
	}
	void operator()(Object* sender, EventArgs& args)
	{
		for ( std::vector<EventHandler>::const_iterator it = handlers_.begin() ; it != handlers_.end() ; ++it )
			(it->listener_->*it->callback_)(sender, args);
	}
protected:
	std::vector<EventHandler> handlers_;
};

typedef Event MyCustomEvent;

class MyCustomClass : public Object
{
public:
	MyCustomEvent onDoSomethingEvent;
	void doSomething()
	{
		std::cout << "MyCustomClass::doSomething()\n";
		MyCustomEventArgs args(2, 4);
		onDoSomethingEvent(this, args);
	}
};

template <typename T>
inline EventHandler make_handler(T& object, void (T::*callback)(Object*, EventArgs&))
{
	return EventHandler(object, callback);
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
	MyCustomEventArgs args(99, -42);
	c.onDoSomethingEvent(NULL, args);
	
// 	std::cout << typeid(A::callback);
	
	return 0;
}
