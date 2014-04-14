#ifndef FEATURESPACE_H
#define FEATURESPACE_H

#include <unordered_map>

namespace std {

  template <>
  struct hash<Ilwis::Pixel>
  {
    std::size_t operator()(const Ilwis::Pixel& k) const
    {
      return ((hash<int>()(k.x)
               ^ (hash<int>()(k.y) << 1)) >> 1);
    }
  };

}

namespace Ilwis {

class SampleSet;

struct FSInfo {
    FSInfo() : _fsFrequency(0), _selectedFrequency(0), _classRaw(iUNDEF) {}
    FSInfo(Raw clsRaw) : _fsFrequency(0), _selectedFrequency(0), _classRaw(clsRaw){}
    int _fsFrequency;
    int _selectedFrequency;
    Raw _classRaw;
};

class FeatureSpace
{
public:
    FeatureSpace(SampleSet* sampleset, quint32 band1, quint32 band2);

    void addSample(const Ilwis::Pixel &xy, Raw classRaw);
    void compute();
    quint64 id() const;

private:
    std::unordered_map<Pixel, FSInfo> _fsInfo;
    std::unique_ptr<SampleSet> _sampleset;
    quint32 _band1;
    quint32 _band2;
    quint64 _id;
    static quint64 _featureSpaceIds;
};
}

#endif // FEATURESPACE_H
