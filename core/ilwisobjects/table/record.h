#ifndef RECORD_H
#define RECORD_H

namespace Ilwis {


class KERNELSHARED_EXPORT Record : public std::vector<QVariant>
{
public:
    Record();
    Record(const std::vector<QVariant> &data);

    bool isChanged() const;
    void changed(bool yesno);
    bool isValid() const;


private:
    bool _changed = false;

};
}

#endif // RECORD_H
