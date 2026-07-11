#include "select_menu.hpp"
#include "utils.hpp"

SelectMenu::SelectMenu(int i, TemperaturePoint* fanCurveTable, bool* tableIsChanged) 
{
    this->_i = i;
    this->_fanCurveTable = fanCurveTable;
    this->_tableIsChanged = tableIsChanged;

    this->_saveBtn = new tsl::elm::ListItem("Save");
    this->_tempLabel = new tsl::elm::CategoryHeader(std::to_string((this->_fanCurveTable + this->_i)->temperature_c) + "°C", true);
    this->_fanLabel = new tsl::elm::CategoryHeader(std::to_string((int)((this->_fanCurveTable + this->_i)->fanLevel_f * 100)) + "%", true);
}

tsl::elm::Element* SelectMenu::createUI(){

    auto list = new tsl::elm::List();

    list->addItem(this->_tempLabel);
    auto trackTemp = new tsl::elm::TrackBar("°C");
    trackTemp->setValueChangedListener([this](u16 value)
    {
        this->_tempLabel->setText(std::to_string(value) + "°C");
        (this->_fanCurveTable + this->_i)->temperature_c = value;
        this->_saveBtn->setText("Save");
    });
    trackTemp->setProgress((this->_fanCurveTable + this->_i)->temperature_c);
    list->addItem(trackTemp);

    list->addItem(this->_fanLabel);
    auto trackFanL = new tsl::elm::TrackBar("%");
    trackFanL->setValueChangedListener([this](u16 value)
    {
        this->_fanLabel->setText(std::to_string(value) + "%");
        (this->_fanCurveTable + this->_i)->fanLevel_f = (float)value / 100;
        this->_saveBtn->setText("Save");
    });
    trackFanL->setProgress((int)((this->_fanCurveTable + this->_i)->fanLevel_f * 100));
    list->addItem(trackFanL);

    list->addItem(new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, s32 x, s32 y, s32 w, s32 h) {}), 20);

    this->_saveBtn->setClickListener([this](uint64_t keys) 
    {
        if (keys & HidNpadButton_A) 
        {
            WriteConfigFile(this->_fanCurveTable);

            if(IsRunning() != 0)
            {
                pmshellTerminateProgram(SysFanControlID);
                const NcmProgramLocation programLocation
                {
                    .program_id = SysFanControlID,
                    .storageID = NcmStorageId_None,
                };
                u64 pid = 0;
                pmshellLaunchProgram(0, &programLocation, &pid);
            }
                
            this->_saveBtn->setText("Saved!");
            *this->_tableIsChanged = true;
            return true;
        }
        
        return false;
        
    });

    list->addItem(this->_saveBtn);

    auto frame = new tsl::elm::OverlayFrame("NX-FanControl", APP_VERSION);
    frame->setContent(list);

    return frame;
}