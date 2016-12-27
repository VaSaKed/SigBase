#include <vector>
#include <algorithm>
#include <iostream>

template<typename RET, typename ...ARGS>
class Signal{
public:
    using FP = RET(*)(ARGS...);
    RET operator ()(ARGS...args) {
        for (auto& _slot: _slots) { 
            if (_slot.first)
                asm("movl %0, %%ecx"::"r"(_slot.first):"%ecx");
            _slot.second(args...); /*RET*/ 
        }
    }
    
    void connectTo( FP slotFunction ) { _slots.push_back( {nullptr, slotFunction} ); }
    
    template<typename _class>
    void connectTo(_class& c, RET(_class::*mfp)(ARGS...)){
        _slots.push_back( {reinterpret_cast<void*>(&c), reinterpret_cast<FP>(mfp)} ); 
    }
        
    template<typename _functor>
    void connectTo(_functor& c ){
        connectTo(c, &_functor::operator());
    }

private:
    std::vector<std::pair<void*,FP>> _slots; // pair of <class object*,member function*>
};

int aFunction(int a){std::cout << "Function:\n Parameter: " << a << std::endl;}

class aClass{
public:
    int aMember(int){std::cout << "Object:\n Look at this: " << this
                               << "\n Closure: " << aMemberValue << std::endl;}
    int aMemberValue;
};

int main(){

    Signal<int,int> sig;
    sig.connectTo(aFunction);
    sig(11);

    aClass anObject{23};
    sig.connectTo(anObject, &aClass::aMember);
    sig(23);

    Signal<int,int> sig2;
    sig.connectTo(sig2);
    sig.connectTo([](int)->int{return 12;});
}
