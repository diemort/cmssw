/****************************************************************************
 *
 * This is a part of CTPPS offline software.
 * Authors:
 *   Laurent Forthomme (laurent.forthomme@cern.ch)
 *   Nicola Minafra
 *
 ****************************************************************************/

#include "RecoCTPPS/TotemRPLocal/interface/TotemTimingConversions.h"
//----------------------------------------------------------------------------------------------------

const float TotemTimingConversions::SAMPIC_SAMPLING_PERIOD_NS = 1. / 7.8;
const float TotemTimingConversions::SAMPIC_ADC_V = 1. / 256;
const int TotemTimingConversions::SAMPIC_MAX_NUMBER_OF_SAMPLES = 64;
const int TotemTimingConversions::SAMPIC_DEFAULT_OFFSET = 30;

//----------------------------------------------------------------------------------------------------

TotemTimingConversions::TotemTimingConversions()
    : calibrationFileOk_(false), calibrationFile_("/dev/null"), calibrationData_(CalibrationData()) {}

//----------------------------------------------------------------------------------------------------

TotemTimingConversions::TotemTimingConversions(const std::string& calibrationFile)
    : calibrationFileOk_(false), calibrationFile_(calibrationFile), calibrationData_(CalibrationData()) {}

//----------------------------------------------------------------------------------------------------

void TotemTimingConversions::openCalibrationFile(const std::string& calibrationFile){

  if (calibrationFile!="/dev/null"){ // if given, the filename here overwrites the filename in the ctor
    calibrationFile_ = calibrationFile;
  }

  if (calibrationFile_!="/dev/null"){
    try{
      calibrationData_.openFile(calibrationFile_);
      calibrationFileOk_ = true;
    }
    catch(boost::exception const &e){
      calibrationFileOk_ = false;
    }
  }
  calibrationFunction_ = TF1("calibrationFunction_",calibrationData_.getFormula().c_str()); //set formula

}

//----------------------------------------------------------------------------------------------------

const float TotemTimingConversions::getTimeOfFirstSample(const TotemTimingDigi& digi) const{
  unsigned int offsetOfSamples = digi.getEventInfo().getOffsetOfSamples();
  if (offsetOfSamples == 0)
    offsetOfSamples = SAMPIC_DEFAULT_OFFSET; // FW 0 is not sending this, FW > 0 yes
  int cell0TimeClock;
  float cell0TimeInstant;       // Time of first cell
  float firstCellTimeInstant; // Time of triggered cell

  unsigned int timestamp = digi.getCellInfo() <= SAMPIC_MAX_NUMBER_OF_SAMPLES/2 ?
                                                      digi.getTimestampA()
                                                    : digi.getTimestampB();

  cell0TimeClock = timestamp +
                   ((digi.getFPGATimestamp() - timestamp) & 0xFFFFFFF000) -
                   digi.getEventInfo().getL1ATimestamp() +
                   digi.getEventInfo().getL1ALatency();

  cell0TimeInstant = SAMPIC_MAX_NUMBER_OF_SAMPLES *
                     SAMPIC_SAMPLING_PERIOD_NS * cell0TimeClock;

  if (digi.getCellInfo() < offsetOfSamples)
    firstCellTimeInstant =
        cell0TimeInstant + digi.getCellInfo() * SAMPIC_SAMPLING_PERIOD_NS;
  else
    firstCellTimeInstant =
        cell0TimeInstant -
        (SAMPIC_MAX_NUMBER_OF_SAMPLES - digi.getCellInfo()) *
            SAMPIC_SAMPLING_PERIOD_NS;

  int db = digi.getHardwareBoardId();
  int sampic = digi.getHardwareSampicId();
  int channel = digi.getHardwareChannelId();
  //NOTE: If no time offset is set, getTimeOffset returns 0
  return firstCellTimeInstant + calibrationData_.getTimeOffset(db, sampic, channel);
}

//----------------------------------------------------------------------------------------------------

const float TotemTimingConversions::getTriggerTime(const TotemTimingDigi& digi) const{
  unsigned int offsetOfSamples = digi.getEventInfo().getOffsetOfSamples();
  if (offsetOfSamples == 0)
    offsetOfSamples = 30; // FW 0 is not sending this, FW > 0 yes

  float triggerCellTimeInstant = getTimeOfFirstSample(digi) +
        (SAMPIC_MAX_NUMBER_OF_SAMPLES - offsetOfSamples) *
        SAMPIC_SAMPLING_PERIOD_NS;

  return triggerCellTimeInstant;
}

//----------------------------------------------------------------------------------------------------

std::vector<float> TotemTimingConversions::getTimeSamples(const TotemTimingDigi& digi) const{
  std::vector<float> time(digi.getNumberOfSamples());
  // firstCellTimeInstant = 0;
  if (!calibrationFileOk_){
    for (unsigned int i = 0; i < time.size(); ++i)
      time.at(i) = getTimeOfFirstSample(digi) + i * SAMPIC_SAMPLING_PERIOD_NS;
  }
  return time;
}

//----------------------------------------------------------------------------------------------------

std::vector<float> TotemTimingConversions::getVoltSamples(const TotemTimingDigi& digi){
  std::vector<float> data;
  if (!calibrationFileOk_){
    for (auto it = digi.getSamplesBegin(); it != digi.getSamplesEnd(); ++it)
      data.emplace_back(SAMPIC_ADC_V * (*it));
  }
  else{
    unsigned int db = digi.getHardwareBoardId();
    unsigned int sampic = digi.getHardwareSampicId();
    unsigned int channel = digi.getHardwareChannelId();
    unsigned int cell = digi.getCellInfo();
    for (auto it = digi.getSamplesBegin(); it != digi.getSamplesEnd(); ++it, ++cell){
      auto parameters = calibrationData_.getParameters(db, sampic, channel, cell);
      for (unsigned int i=0; i<parameters.size(); ++i)
        calibrationFunction_.SetParameter(i, parameters.at(i));
      data.emplace_back(calibrationFunction_.Eval(*it));
    }
  }
  return data;
}
