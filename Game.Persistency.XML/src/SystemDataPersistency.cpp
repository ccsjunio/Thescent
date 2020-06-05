#include "SystemDataPersistency.h"

LevelFactory* SystemDataPersistency::GetLevelFactory()
{
    return levelFactory;
}

SystemDataPersistency::SystemDataPersistency()
{
    this->isUpdateComplete = false;
    this->isActive = true;

    levelFactory = new LevelFactory();
    if (!levelFactory)
    {
        printf("Could not create a LevelFactory.\n");
    }


}

SystemDataPersistency::~SystemDataPersistency()
{
  
}

const bool SystemDataPersistency::IsUpdateComplete()
{
	return this->isUpdateComplete;
}

const bool SystemDataPersistency::IsActive()
{
	return this->isActive;
}

void SystemDataPersistency::Activate(const bool& value)
{
    this->isActive = value;
}

void SystemDataPersistency::BindOnFileLoadCallback(std::function<void(const OnFileLoad_s&)> fn)
{
    if (!fn) return;

    this-> onFileLoadCallback = fn;

    if (levelFactory)
    {
        levelFactory->BindOnFileLoadCallback(fn);
    }
}

void* SystemDataPersistency::GetPointer(ParameterContainer& input, ParameterContainer& output)
{
    void* ptr = nullptr;

    Parameter what;
    bool rc = input.FindParameterByName("what", what);
    if (!rc) return nullptr;
    
    if (what.value == "levelfactory")
    {
        ptr = GetLevelFactory();
    }
    else if (what.value == "shaderfilereader")
    {
        IFileReader* container = new ShaderFileReader();
        ptr = container;
    }

	return ptr;
}

void SystemDataPersistency::Update(const FrameTime& frameTime)
{
    if (!this->isActive) return;
    this->isUpdateComplete = false;

    SetFrameTime(frameTime);

    this->isUpdateComplete = true;
}
