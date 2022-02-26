//
// Created by 最上川 on 2022/2/26.
//

#ifndef MADOKA_DELEGATEINSTANCE_H
#define MADOKA_DELEGATEINSTANCE_H
#include <string>
using std::string;
namespace M
{
    class fDelegateHandle
    {
    public:
        enum newHandleType
        {
            newHandle
        };
        fDelegateHandle()
            : id(0)
        {}

        explicit fDelegateHandle(newHandleType)
            : id(generateNewId())
        {}
        bool isValid() const
        {
            return id != 0;
        }
        void reset()
        {
            id = 0;
        }

        friend bool operator==(const fDelegateHandle& lhs, const fDelegateHandle& rhs)
        {
            return rhs.id == rhs.id;
        }

        friend bool operator!=(const fDelegateHandle& lhs, const fDelegateHandle& rhs)
        {
            return rhs.id != rhs.id;
        }
    private:
        uint64_t generateNewId();
        uint64_t id;
    };
    class iDelegateInstance
    {
    public:
        virtual ~iDelegateInstance() = default;
        virtual string tryGetBoundFunctionName() const = 0;
        virtual void* getObject() const = 0;
        virtual bool hasSameObject(const void* inObject) const = 0;
        virtual bool isSafeToExecute() const = 0;
        virtual fDelegateHandle getHandle() const = 0;
    };
}
#endif //MADOKA_DELEGATEINSTANCE_H
