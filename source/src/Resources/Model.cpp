#include <Model.hpp>
#include <Log.hpp>

void Model::LoadResources(const char* _name)
{
	std::fstream file;
	std::filesystem::path path = "assets/meshes/";
	path += _name;
	file.open(path);
	if (file.bad())
	{
		DEBUG_WARNING("Model File %s is BAD", _name);
		return;
	}
	if (file.fail())
	{
		DEBUG_ERROR("Model File %s opening has FAILED", _name);
		return;
	}
	if(file.is_open())
	{
		DEBUG_LOG("Model File %s has been opened",_name);
	}
}
