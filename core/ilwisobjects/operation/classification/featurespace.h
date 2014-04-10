#ifndef FEATURESPACE_H
#define FEATURESPACE_H

#include <unordered_map>

namespace std {

  template <>
  struct hash<Ilwis::XY>
  {
    std::size_t operator()(const Ilwis::XY& k) const
    {
      return ((hash<int>()(k.x)
               ^ (hash<int>()(k.y) << 1)) >> 1);
    }
  };

}

namespace Ilwis {

struct FSInfo {
    FSInfo() : _fsFrequency(0), _ssFrequency(0), _classRaw(iUNDEF) {}
    int _fsFrequency;
    int _ssFrequency;
    Raw _classRaw;
};

class FeatureSpace
{
public:
    FeatureSpace();

    void addSample(const XY& xy, Raw classRaw);
    void compute();

private:
    std::unordered_map<XY, FSInfo> _fsInfo;
};
}

#endif // FEATURESPACE_H
