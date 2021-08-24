#include "OnLevelSave.h"

void OnLevelSave::onLevelSave(bool succeeded, unsigned int savedLevelNumber, LM& lm, const std::optional<const Config>& config)
{
    if (succeeded && config.has_value()) 
    {
        onSuccessfulLevelSave(savedLevelNumber, lm, config.value());
    }
    else
    {
        onFailedLevelSave(savedLevelNumber, lm);
    }
}

void OnLevelSave::onSuccessfulLevelSave(unsigned int savedLevelNumber, LM& lm, const Config& config)
{
    fs::path mwlPath = config.getLevelDirectory();
    std::string mwlFileName = config.getMwlFileFormat();

    int indexToInsertLvlNum = mwlFileName.find("#");

    if (indexToInsertLvlNum != std::string::npos)
    {
        std::stringstream sstream;
        sstream << std::hex << std::uppercase << lm.getLevelEditor().getLevelNumberBeingSaved() << std::nouppercase << std::dec;
        std::string lvlNumString = sstream.str();

        while (lvlNumString.size() != 3)
            lvlNumString = "0" + lvlNumString;

        mwlFileName = std::regex_replace(mwlFileName, std::regex("\\#"), lvlNumString);
    }

    mwlPath /= mwlFileName;

    fs::path romPath = lm.getPaths().getRomDir();
    romPath += lm.getPaths().getRomName();

    lm.getLevelEditor().exportMwl(lm.getPaths().getLmExePath(), romPath, mwlPath, savedLevelNumber);
}

void OnLevelSave::onFailedLevelSave(unsigned int savedLevelNumber, LM& lm)
{

}
