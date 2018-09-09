/****************************************************************************
 *
 * This is a part of CTPPS offline software.
 * Authors:
 *   Filip Dej
 *
 ****************************************************************************/

#ifndef RecoCTPPS_TotemRPLocal_TotemTimingParser
#define RecoCTPPS_TotemRPLocal_TotemTimingParser

#include <iostream>
#include <map>
#include <vector>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

class TotemTimingParser{
  private:
    //--------------------------------------------------------------------------
    // Helper structure for storing calibration data
    struct CalibrationKey{
      int db, sampic, channel, cell;
      bool operator< (const CalibrationKey &rhs) const {
        if(db == rhs.db) {
          if (sampic == rhs.sampic){
            if (channel == rhs.channel)
              return cell < rhs.cell;
            return channel < rhs.channel;
          }
          return sampic < rhs.sampic;
        }
        return db < rhs.db;
      }
      CalibrationKey(const int db=-1, const int sampic=-1, const int channel=-1, const int cell=-1)
        : db(db), sampic(sampic), channel(channel), cell(cell) {};
    };
    friend std::ostream& operator<<(std::ostream &strm, const TotemTimingParser::CalibrationKey &key);
    //--------------------------------------------------------------------------

    std::string formula_;
    std::map<CalibrationKey, std::vector<double>> parameters_;
    std::map<CalibrationKey, std::pair<double,double>> timeInfo_;

  public:
    TotemTimingParser();
    void parseFile(const std::string &file_name);
    std::vector<double> getParameters(int db, int sampic, int channel, int cell) const;
    std::string getFormula() const;
    double getTimeOffset(int db, int sampic, int channel) const;
    double getTimePrecision(int db, int sampic, int channel) const;
    void print();
    friend std::ostream& operator<<(std::ostream &strm, const TotemTimingParser &data);
};

#endif
