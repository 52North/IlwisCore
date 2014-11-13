#ifndef FEATUREITERATOR_H
#define FEATUREITERATOR_H

namespace Ilwis {

/**
 * FeatureIterator
 * The primary access method to the features and indexes is through the featureiterator. The <br>
 * featureiterator abstracts the movement through the feature-elements by representing the whole <br>
 * set of feature-elements as one linear set of feature-elements. Within this space a traditional(c++) <br>
 * random access iterator has been defined with all the methods and operators traditionally associated <br>
 * with such a concept. Particularly: <br>
 * <br>
 * - Operators ‘++’and ‘—‘ , moving one step in the data-array.
 * - Operator ‘*’accessing data
 * - Operator ‘[]’ random access
 * <br>
 * Because this mechanism hides the actual organization of feature-elements and indexes, it allows in a <br>
 * large number of cases for the very efficient formulating of algorithms. <br>
 * <br>
 * There are three modes of movement : <br>
 * <br>
 * - Iterate through all feature-elements at the same index level and when done move to the
 * next index level. Or,
 * - Iterate through all feature-elements of a feature and then move to the next feature.
 * <br>
 * The first method is default.
 *
 *\sa FeatureCoverage
 */
class KERNELSHARED_EXPORT FeatureIterator  : public std::iterator<std::random_access_iterator_tag, int>
{
public:
    enum Flow{fFLAT, fBREADTHFIRST, fDEPTHFIRST};
    /**
     * Constructor for an empty FeatureIterator
     */
    FeatureIterator();

    /**
     * Creates a FeatureIterator on a FeatureCoverage, the created iterator will <br>
     * iterate over all the features in the supplied coverage. <br>
     * initialisation will fail if the supplied feature coverage is invalid. The iterator
     * will be at the beginning of the set of features contained in the feature coverage
     *
     * @param fcoverage The FeatureCoverage that should be iterated over
     */
    FeatureIterator(const Ilwis::IFeatureCoverage &fcoverage, quint32 level=0);

/**
 * Creates a FeatureIterator on a FeatureCoverage, the created iterator will <br>
 * iterate over all the features in the supplied coverage. <br>
 * initialisation will fail if the supplied feature coverage is invalid. The iterator
 * will be at the beginning of the set of features contained in the feature coverage
 *
 * @param fcoverage fcoverage The FeatureCoverage that should be iterated over
 * @param types only iterate over the types indicated by this mask. Geometry types outside the mask will not be considered
 */
    FeatureIterator(const Ilwis::IFeatureCoverage &fcoverage, IlwisTypes types, quint32 level=0);

    /**
     * Creates a FeatureIterator on a FeatureCoverage, the created iterator will <br>
     * iterate over all the features in the supplied subset of this coverage. <br>
     * initialisation will fail if the supplied feature coverage is invalid or if the subset is invalid(cannot be applied to the coverage).
     *
     * @param fcoverage The FeatureCoverage that should be iterated over
     * @param subset The subset that should be iterated
     */
    FeatureIterator(const Ilwis::IFeatureCoverage &fcoverage, const std::vector<quint32>& subset, quint32 level=0);

    /**
     * Copy constructor.<br>
     * Constructs a copy of the given FeatureIterator.<br>
     * if the supplied FeatureIterator is invalid, the result of this will also be invalid.
     *
     * @param iter The FeatureIterator that should be copied
     */
    FeatureIterator(const FeatureIterator& iter);

    /**
     * Override of the operator ++ <br>
     * Moves this FeatureIterator to the next feature.<br>
     * Returns a reference to this FeatureIterator.
     *
     * @return a reference to this FeatureIterator.
     */
    FeatureIterator& operator++() ;

    /**
     * Override of the operator ++<br>
     * Copies this FeatureIterator and moves the copy to the next feature. <br>
     * Returns the copy.
     *
     * @return a copy of this moved by 1
     */
    FeatureIterator operator++(int);


    /**
     * Override of the operator + <br>
     * Moves this FeatureIterator n features.<br>
     * Returns a reference to this FeatureIterator.
     *
     * @return a reference to this FeatureIterator.
     */
    FeatureIterator& operator+(int);

    /**
     * Override of the operator - <br>
     * Moves this FeatureIterator -n features.<br>
     * Returns a reference to this FeatureIterator.
     *
     * @return a reference to this FeatureIterator.
     */
    FeatureIterator& operator-(int);

    /**
     * Override of the == operator.<br>
     * Compares the current feature of the 2 FeatureIterators, if they are the same true will be returned.<br>
     * In all other cases false will be returned.
     *
     * @param rhs the FeatureIterator this one should be compared to
     * @return true when they both are at the same feature
     */
    bool operator==(const FeatureIterator& rhs) ;

    /**
     * Override of the != operator.<br>
     * Compares the current feature of the 2 FeatureIterators, if they are the same false will be returned.<br>
     * In all other cases true will be returned.
     *
     * @param rhs the FeatureIterator this one should be compared to
     * @return false when they both are at the same feature
     */
    bool operator!=(const FeatureIterator& rhs);

    /**
     * Creates and returns a reference to the features this FeatureIterator iterates over
     *
     * @return the features of this iterator
     */
    SPFeatureI operator*();

    /**
     * Returns a copy of this FeatureIterator which has as current feature the last feature in the coverage
     *
     * @return a FeatureIterator copy at the end of its coverage
     */
    FeatureIterator end() const;

    /**
     * Returns a copy of this FeatureIterator which has as current feature the last feature in the coverage
     *
     * @return a FeatureIterator copy at the end of its coverage
     */
    FeatureIterator end();

    Flow flow() const;
    void flow(const Flow &flow);

private:
    bool init();
    bool move(qint32 distance=1);
    IFeatureCoverage _fcoverage;
    Features::iterator _iterFeatures;
    std::vector<QString> _currentIndexes;
    boost::container::flat_map<quint32,SPFeatureI>::iterator _subIterator;
    bool _isInitial;
    std::vector<quint32> _subset;
    quint32 _iterPosition = iUNDEF;
    bool _useVectorIter = true;
    IlwisTypes _types;
    quint32 _level=0;
    quint32 _currentLevel = 0;
    Flow _flow = fFLAT;

    bool moveFlat(qint32 distance);
    bool moveBreadthFirst(qint32 distance);
    bool moveDepthFirst(qint32 distance);

    bool atEndOfFeatures();
};
}

inline Ilwis::FeatureIterator begin(const Ilwis::IFeatureCoverage &fcoverage) {
    return Ilwis::FeatureIterator(fcoverage);
}

inline Ilwis::FeatureIterator end(const Ilwis::IFeatureCoverage &fcoverage) {
    Ilwis::FeatureIterator iter(fcoverage);
    return iter.end();
}

#endif // FEATUREITERATOR_H
