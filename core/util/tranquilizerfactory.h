#ifndef TRANQUILIZERFACTORY_H
#define TRANQUILIZERFACTORY_H

namespace Ilwis {

class Tranquilizer;


typedef std::function<Tranquilizer *(const IOOptions &options)> CreateTranquilizer;

class KERNELSHARED_EXPORT TranquilizerFactory : public AbstractFactory
{
public:
    TranquilizerFactory();
    ~TranquilizerFactory();

    static void registerTranquilizerType(int runMode, CreateTranquilizer createFunc);

    template<class T=Tranquilizer> static T *create(int runMode, const IOOptions &options)
    {
       auto iter = _creators.find(runMode);
       if ( iter == _creators.end()){
           return _creators[rmEMPTY](options);
       }
       return static_cast<T *>((*iter).second(options));
    }

    bool prepare();

private:
    static std::map<int, CreateTranquilizer>  _creators;
};
}
#endif // TRANQUILIZERFACTORY_H
