#include "OnMap16Save.h"


void OnMap16Save::onMap16Save(bool succeeded, LM& lm, const std::optional<const Config>& config)
{
    if (succeeded && config.has_value()) 
    {
        onSuccessfulMap16Save(lm, config.value());
    }
    else
    {
        onFailedMap16Save(lm);
    }
}

void OnMap16Save::onSuccessfulMap16Save(LM& lm, const Config& config)
{
    fs::path romPath = lm.getPaths().getRomDir();
    romPath += lm.getPaths().getRomName();

    if (lm.getLevelEditor().exportMap16(lm.getPaths().getLmExePath(), romPath, config.getMap16Path()))
        Logger::log_message(L"Successfully completed OnMap16Save");
    else
        Logger::log_error(L"Failed to export map 16 in OnMap16Save");
}

void OnMap16Save::onFailedMap16Save(LM& lm)
{
    Logger::log_error(L"Failed OnMap16Save call");
}
