#include "kernel.h"
#include "record.h"

using namespace Ilwis;

Record::Record()
{
}

Record::Record(const std::vector<QVariant>& data) : std::vector<QVariant>(data)
{
}

bool Record::isChanged() const
{
    return _changed;
}

void Record::changed(bool yesno)
{
    _changed = yesno;
}

bool Record::isValid() const
{
    return size() != 0;
}
