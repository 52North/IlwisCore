#ifndef SERIALIZATIONOPTIONS_H
#define SERIALIZATIONOPTIONS_H

namespace Ilwis {
/*!
A simple structure to pass options to the serialization. Normally an Ilwis object is fully serialized including the binary part
when going to a target. It is possible though to limit this. The following options are available
- FULL, the default. everything will be serialized
- BYREF, Only a reference will be serialized. This assumes the target has the option to load the object through the reference
- BYID, Only the id is used. This is similar to byref. This assumes that the target has the option to retrieve the object by id
 */
struct SerializationOptions {
    enum CompoundMode { cmFULL, cmBYREF, cmBYID};
    enum Format {fBINARY, fXML};
    SerializationOptions(bool binary=true, CompoundMode m=cmFULL) : includeBinary(binary), mode(m), format(fBINARY) { }

    bool includeBinary;
    CompoundMode mode;
    Format format;
};
}

#endif // SERIALIZATIONOPTIONS_H
