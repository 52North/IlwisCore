#ifndef ZONEUTIL_H
#define ZONEUTIL_H

namespace Ilwis {
namespace GiaCIS {

template <class T>
struct ZoneData
{
    int _zone;
    int _length;
    int _startIndex;
    std::vector<T> _data;

    ZoneData() {}

    bool operator < (const ZoneData& zs) const
    {
        return (_zone < zs._zone);
    }
};

template <class T>
class ZoneSeasons {
    std::vector<ZoneData<T>> _seasons;
    std::map<T, int> _lutSeason;
    int _totalSeasonRows;

public:
    ZoneSeasons() {}

    void loadFromTable(ITable seasonTable);

    bool isKnownZone(int zone) const { return _lutSeason.find(zone) != _lutSeason.end(); }
    ZoneData<T>& getSeasonData(int zone) { return _seasons[_lutSeason[zone]]; }
};

template <class T>
class ZoneLimits {
    std::vector<std::vector<std::pair<T, T>>> _limits;
    std::map<int, int> _lutLimits;

public:
    ZoneLimits() {}

    void loadFromTables(ITable low, ITable high);

    bool isKnownZone(int zone) { return _lutLimits.find(zone) != _lutLimits.end(); }
    std::vector<std::pair<T, T>> getLimits(int zone) { return _limits[_lutLimits[zone]]; }

};

template <class T>
void ZoneSeasons<T>::loadFromTable(ITable seasonTable) {
    // Read the seasons table
    _totalSeasonRows = seasonTable->recordCount();
    // The seasons table contains a one for all dekad's that belong to the growing season and
    // zeroes otherwise. The growing season is a consecutive series of ones (no breaks)
    // The table is assumed to start at the 1st of January
    _seasons.resize(_totalSeasonRows);
    for (int row = 0; row < _totalSeasonRows; ++row) {
        _seasons[row]._zone = seasonTable->cell(0, row).toInt();
        _seasons[row]._length = 0;
        for (int col = 1; col < seasonTable->columnCount(); ++col) {
            _seasons[row]._data.push_back(seasonTable->cell(col, row).toInt());
        }
        std::vector<int>::iterator sb = _seasons[row]._data.begin();
        std::vector<int>::iterator se = _seasons[row]._data.end();
        // Count the length of the season: this is at most only one consequtive period
        std::vector<int>::iterator first = std::find(sb, se, 1);
        std::vector<int>::iterator last = std::find_if(first, se, [] (int i) { return i == 0;} );
        _seasons[row]._length = last - first;
        _seasons[row]._startIndex = first - sb;
    }

    // setup the inverse LUT
    for (int i = 0; i < _seasons.size(); ++i)
        _lutSeason[_seasons[i]._zone] = i;
}

template <class T>
void ZoneLimits<T>::loadFromTables(ITable low, ITable high) {
    int totalRows = low->recordCount(); // same as high->recordCount()
    int totalCols = low->columnCount(); // same as high->columnCount()
    _limits.resize(totalRows);
    std::vector<int> zones;
    for (int row = 0; row < totalRows; ++row) {
        zones.push_back(row + 1);   // zone is implicit in the tables
        // Also order of the columns is implicit in the tables
        for (int col = 0; col < totalCols; ++col) {
            auto lim = std::make_pair(low->cell(col, row).toDouble(), high->cell(col, row).toDouble());
            _limits[row].push_back(lim);
        }
        // fill the inverse LUT
        _lutLimits[zones[row]] = row;
    }
}

}
}

#endif // ZONEUTIL_H
