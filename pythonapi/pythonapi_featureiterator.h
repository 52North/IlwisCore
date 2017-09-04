#ifndef PYTHONAPI_FEATUREITERATOR_H
#define PYTHONAPI_FEATUREITERATOR_H

#include "pythonapi_feature.h"

namespace Ilwis {
    class FeatureIterator;
}

namespace pythonapi {

    class FeatureCoverage;

    class FeatureIterator{
        public:
            FeatureIterator(FeatureCoverage* fc);
            FeatureIterator(const FeatureIterator& it);
            /**
             * @brief next confusingly returns current value bevore iterating to the next item
             * brings together C++ style "std::iterator it != it.end()" with end() pointing the back (after last entry)
             * and Python style "for it in it:" using the __next__() also to retrieve first item.
             * @return
             */
            Feature __next__();
            Feature current();
            Feature __getitem__(quint32 pos);
            FeatureIterator* __iter__();

            std::string __str__();
            bool __bool__() const;

            bool operator==(const FeatureIterator& other);
            bool operator!=(const FeatureIterator& other);

            FeatureIterator operator+(int n);
            FeatureIterator __radd__(int n);
            FeatureIterator operator-(int n);
    private:
            FeatureCoverage* _coverage;
            std::shared_ptr<Ilwis::FeatureIterator> _ilwisFeatureIterator;
            std::shared_ptr<Ilwis::FeatureIterator> _end;
            Ilwis::FeatureIterator& ptr() const;

    };

}
#endif // PYTHONAPI_FEATUREITERATOR_H
